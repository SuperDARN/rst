/* fit_median_filter.c
   ==========
   Author: E.C.Bland
   
   TODO add license notices
   
*/



#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <zlib.h>

#include "rtypes.h"
#include "rtime.h"
#include "option.h"
#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"

#include "fitread.h"
#include "fitwrite.h"

#include "errstr.h"
#include "hlpstr.h"


#define maxbm 30
#define maxch 3

int fnum=0;

struct RadarParm *prm;
struct FitData *fit;

struct OptionData opt;


int rst_opterr (char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: trim_fit --help\n");
  return(-1);
}


int get_index(int a, int b, int c, int d, int aSize, int bSize, int cSize, int dSize) {
    return (d * aSize * bSize * cSize) + (c * aSize * bSize) + (b * aSize) + a; 
}

int main (int argc,char *argv[]) {

  int arg;

  int status=0;
  FILE *fp=NULL;
  
  unsigned char vb=0;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);
  OptionAdd(&opt,"vb",'x',&vb);

  arg=OptionProcess(1,argc,argv,&opt,rst_opterr);

  if (arg==-1) {
    exit(-1);
  }

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }
  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  if (version==1) {
    OptionVersion(stdout);
    exit(0);
  }
  
  
  prm=RadarParmMake();
  fit=FitMake();

  if (arg==argc) fp=stdin;
  else fp=fopen(argv[arg],"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }
  if (FitFread(fp,prm,fit)==-1) {
    fprintf(stderr,"Error reading file\n");
    exit(-1);
  }
  

  // Read the FITACF file to determine number records for each beam & channel
  //   Also determine maximum number of range gates in the file
  int maxrng=-1;
  int tcnt[maxbm][maxch];
  memset(tcnt,0,sizeof(tcnt));

  do {
    
    tcnt[prm->bmnum][prm->channel]++;
    if (prm->nrang > maxrng) maxrng=prm->nrang;
    
  } while (FitFread(fp,prm,fit) !=-1);
  
  
  // Determine required array size for median filtering and allocate memory
  int bm,ch;
  int tmax=-1;
  for (bm=0;bm<maxbm;bm++) {
    for (ch=0;ch<maxch;ch++) {
      if (tcnt[bm][ch] > tmax) tmax=tcnt[bm][ch];
    }
  }
  int *qflg=malloc(tmax*maxrng*maxbm*maxch*sizeof(int));
  int *qflg_filtered=malloc(tmax*maxrng*maxbm*maxch*sizeof(int));
  

  
  // rewind the file pointer
  rewind(fp); 
  if (FitFread(fp,prm,fit)==-1) {
    fprintf(stderr,"Error reading file\n");
    exit(-1);
  }
  
  // Read the FITACF file again to populate the qflg array
  memset(tcnt,0,sizeof(tcnt));
  int index;
  int rng;
  do {
  
    bm=prm->bmnum;
    ch=prm->channel;
    for (rng=0;rng<prm->nrang;rng++) {
      index=get_index(tcnt[bm][ch],rng,bm,ch,tmax,maxrng,maxbm,maxch);
      qflg[index]=fit->rng[rng].qflg;
      qflg_filtered[index]=fit->rng[rng].qflg;
    }
    tcnt[bm][ch]++;
  
  
  } while (FitFread(fp,prm,fit) !=-1);
  
  
  // Do the median filtering
  //   Since qflg can only be 0 or 1, the median of the neighbouring cells can be 
  //   calculated using the test ( #neighbour_cells > (#neighbour_cells-1)/2 ).
  //   The median calculation does not include the center cell
  int median=0;
  int t;
  int index_list[9];  
  memset(index_list,-1,sizeof(index_list));
  for (bm=0;bm<maxbm;bm++) {
    for (ch=0;ch<maxch;ch++) {
      for (rng=0;rng<maxrng;rng++) {
        for (t=0;t<tcnt[bm][ch];t++) {
        
          // get the indices for the current 3x3 grid
          index_list[0]=get_index(t,  rng,  bm,ch,tmax,maxrng,maxbm,maxch);
          index_list[1]=get_index(t,  rng-1,bm,ch,tmax,maxrng,maxbm,maxch);
          index_list[2]=get_index(t,  rng+1,bm,ch,tmax,maxrng,maxbm,maxch);
          index_list[3]=get_index(t-1,rng,  bm,ch,tmax,maxrng,maxbm,maxch);
          index_list[4]=get_index(t-1,rng-1,bm,ch,tmax,maxrng,maxbm,maxch);
          index_list[5]=get_index(t-1,rng+1,bm,ch,tmax,maxrng,maxbm,maxch);
          index_list[6]=get_index(t+1,rng,  bm,ch,tmax,maxrng,maxbm,maxch);
          index_list[7]=get_index(t+1,rng-1,bm,ch,tmax,maxrng,maxbm,maxch);
          index_list[8]=get_index(t+1,rng+1,bm,ch,tmax,maxrng,maxbm,maxch);
          
          
          // skip if there's no scatter in this cell
          if (qflg[index_list[0]]==0) continue;
          
          
          // corners
          if (t==0 && rng==0)
            median=(qflg[index_list[2]]+qflg[index_list[6]]+qflg[index_list[8]]) > 1;
          else if (t==0 && rng==maxrng-1)
            median=(qflg[index_list[1]]+qflg[index_list[6]]+qflg[index_list[7]]) > 1;
          else if (t==tcnt[bm][ch] && rng==0)
            median=(qflg[index_list[2]]+qflg[index_list[3]]+qflg[index_list[5]]) > 1;
          else if (t==tcnt[bm][ch] && maxrng-1)
            median=(qflg[index_list[1]]+qflg[index_list[3]]+qflg[index_list[4]]) > 1;
          
          // edges
          else if (t==0)
            median=(qflg[index_list[1]]+qflg[index_list[2]]+qflg[index_list[6]]+qflg[index_list[7]]+qflg[index_list[8]]) > 2;
          else if (t==tcnt[bm][ch])
            median=(qflg[index_list[1]]+qflg[index_list[2]]+qflg[index_list[3]]+qflg[index_list[4]]+qflg[index_list[5]]) > 2;
          else if (rng==0)
            median=(qflg[index_list[2]]+qflg[index_list[3]]+qflg[index_list[5]]+qflg[index_list[6]]+qflg[index_list[8]]) > 2;
          else if (rng==maxrng-1)
            median=(qflg[index_list[1]]+qflg[index_list[3]]+qflg[index_list[4]]+qflg[index_list[6]]+qflg[index_list[7]]) > 2;
          
          // all other cells
          else median=(qflg[index_list[1]]+qflg[index_list[2]]+qflg[index_list[3]]+qflg[index_list[4]]
                       +qflg[index_list[5]]+qflg[index_list[6]]+qflg[index_list[7]]+qflg[index_list[8]]) > 3;
         
         // reject cell if it has fewer than 4 neighbours (i.e. median qflg in the 3x3 window is zero)
         if (median !=1 ) qflg_filtered[index_list[0]]=0;
         
        }
      }
    }
  }
  
  
  
  //** Read the file one last time to apply the median filter and write a new file
  // rewind the file pointer
  rewind(fp);
  if (FitFread(fp,prm,fit)==-1) {
    fprintf(stderr,"Error reading file\n");
    exit(-1);
  }
  memset(tcnt,0,sizeof(tcnt));
  
  do {
  
    if (vb) {
      fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d %.2d %.2d\n",
              prm->time.yr,prm->time.mo,prm->time.dy,
              prm->time.hr,prm->time.mt,prm->time.sc,
              prm->channel,prm->bmnum);
    }
    
    bm=prm->bmnum;
    ch=prm->channel;
    for (rng=0;rng<prm->nrang;rng++) {
      index=get_index(tcnt[bm][ch],rng,bm,ch,tmax,maxrng,maxbm,maxch);
      
      // Remove the data to be filtered
      if (fit->rng[rng].qflg==1 && qflg_filtered[index]==0) fit->rng[rng].qflg=0;
    }
    tcnt[bm][ch]++;
    
    // TODO Modify origin.command to indicate filtering has been performed 
    
    // write the output file
    status=FitFwrite(stdout,prm,fit);
    if (status ==-1) {
        fprintf(stderr,"Error writing output file.\n");
        exit(-1);
    }
  
  } while (FitFread(fp,prm,fit) !=-1);
  if (fp !=stdin) fclose(fp);
  
  free(qflg);
  free(qflg_filtered);
  if (prm != NULL) RadarParmFree(prm);
  if (fit != NULL) FitFree(fit);
  
  return 0;
}
