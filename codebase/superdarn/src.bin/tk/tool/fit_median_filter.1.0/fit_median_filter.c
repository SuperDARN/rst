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

#define tmax 2000
#define maxbm 30
#define maxch 3
#define maxrng 250

int fnum=0;

struct RadarParm *prm;
struct FitData *fit;

struct OptionData opt;


int rst_opterr (char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: trim_fit --help\n");
  return(-1);
}


int get_index(int a, int b, int c, int d, int aSize, int bSize, int cSize) {
    return (d * aSize * bSize * cSize) + (c * aSize * bSize) + (b * aSize) + a; 
}

typedef struct {
  int *value;
  size_t used;
  size_t size;
} qflgData;



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

  // Initial memory allocation  
  qflgData *qflgs=malloc(sizeof(qflgData));
  qflgs->value = malloc(tmax*maxrng*maxch*maxbm * sizeof(int));
  qflgs->used = 0;
  qflgs->size = tmax*maxrng*maxch*maxbm;
  
  
  // Read the FITACF file to populate the qflg array
  int index;
  int maxindex=-1;
  int rng,bm,ch;
  int tcnt[maxbm][maxch];
  memset(tcnt,0,sizeof(tcnt));
  
  do {
  
    bm=prm->bmnum;
    ch=prm->channel;
    for (rng=0;rng<prm->nrang;rng++) {
      index=get_index(rng,bm,ch,tcnt[bm][ch],maxrng,maxbm,maxch);
      if (maxindex < index) maxindex=index;
      if (qflgs->size < maxindex) {
        qflgs->size += tmax*maxrng*maxch*maxbm;
        qflgs->value = realloc(qflgs->value, qflgs->size * sizeof(int));
      }
      qflgs->value[index] = fit->rng[rng].qflg;
      qflgs->used = maxindex;
      
    }

    tcnt[bm][ch]++;
  
  } while (FitFread(fp,prm,fit) !=-1);
  
  
  // Do the median filtering
  //   Since qflg can only be 0 or 1, the median of the neighbouring cells can be 
  //   calculated using the test ( #neighbour_cells > (#neighbour_cells-1)/2 ).
  //   The median calculation does not include the center cell

  int median=0;
  int index_list[9];  
  memset(index_list,-1,sizeof(index_list));
  int *qflg_filtered=malloc(qflgs->size*sizeof(int));

  
  
  int t=0;
  for (bm=0;bm<maxbm;bm++) {
    for (ch=0;ch<maxch;ch++) {
      if (tcnt[bm][ch]==0) continue;
      for (rng=0;rng<maxrng;rng++) {
        for (t=0;t<tcnt[bm][ch];t++) { 
          
          // get the indices for the current 3x3 grid
          index_list[0]=get_index(rng,  bm,ch,t,maxrng,maxbm,maxch);
          index_list[1]=get_index(rng-1,bm,ch,t,maxrng,maxbm,maxch);
          index_list[2]=get_index(rng+1,bm,ch,t,maxrng,maxbm,maxch);
          index_list[3]=get_index(rng,  bm,ch,t-1,maxrng,maxbm,maxch);
          index_list[4]=get_index(rng-1,bm,ch,t-1,maxrng,maxbm,maxch);
          index_list[5]=get_index(rng+1,bm,ch,t-1,maxrng,maxbm,maxch);
          index_list[6]=get_index(rng,  bm,ch,t+1,maxrng,maxbm,maxch);
          index_list[7]=get_index(rng-1,bm,ch,t+1,maxrng,maxbm,maxch);
          index_list[8]=get_index(rng+1,bm,ch,t+1,maxrng,maxbm,maxch);
          
          // skip if there's no scatter in this cell
          if (qflgs->value[index_list[0]]==0) continue;
          
          
          // corners
          if (t==0 && rng==0)
            median=(qflgs->value[index_list[2]]+qflgs->value[index_list[6]]+qflgs->value[index_list[8]]) > 1;
          else if (t==0 && rng==maxrng-1)
            median=(qflgs->value[index_list[1]]+qflgs->value[index_list[6]]+qflgs->value[index_list[7]]) > 1;
          else if (t==tcnt[bm][ch] && rng==0)
            median=(qflgs->value[index_list[2]]+qflgs->value[index_list[3]]+qflgs->value[index_list[5]]) > 1;
          else if (t==tcnt[bm][ch] && maxrng-1)
            median=(qflgs->value[index_list[1]]+qflgs->value[index_list[3]]+qflgs->value[index_list[4]]) > 1;
          
          // edges
          else if (t==0)
            median=(qflgs->value[index_list[1]]+qflgs->value[index_list[2]]+qflgs->value[index_list[6]]+qflgs->value[index_list[7]]+qflgs->value[index_list[8]]) > 2;
          else if (t==tcnt[bm][ch])
            median=(qflgs->value[index_list[1]]+qflgs->value[index_list[2]]+qflgs->value[index_list[3]]+qflgs->value[index_list[4]]+qflgs->value[index_list[5]]) > 2;
          else if (rng==0)
            median=(qflgs->value[index_list[2]]+qflgs->value[index_list[3]]+qflgs->value[index_list[5]]+qflgs->value[index_list[6]]+qflgs->value[index_list[8]]) > 2;
          else if (rng==maxrng-1)
            median=(qflgs->value[index_list[1]]+qflgs->value[index_list[3]]+qflgs->value[index_list[4]]+qflgs->value[index_list[6]]+qflgs->value[index_list[7]]) > 2;
            
            // all other cells
          else median=(qflgs->value[index_list[1]]+qflgs->value[index_list[2]]+qflgs->value[index_list[3]]+qflgs->value[index_list[4]]+qflgs->value[index_list[5]]+qflgs->value[index_list[6]]+qflgs->value[index_list[7]]+qflgs->value[index_list[8]]) > 3;
          
          qflg_filtered[index_list[0]]=median;
         
        }
      }
    }
  }
  
  
  //** Read the file again to apply the median filter and write a new file
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
      index=get_index(rng,bm,ch,tcnt[bm][ch],maxrng,maxbm,maxch);
      
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
  

  // Free memory
  if (prm != NULL) RadarParmFree(prm);
  if (fit != NULL) FitFree(fit); 
  free(qflgs->value);
  qflgs->value = NULL;
  free(qflg_filtered);
  
  
  return 0;
}
