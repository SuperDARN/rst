/* fit_median_filter.c
   ==========
   
Removes salt & pepper noise from a fitacf file using median filtering. 
Filtering is performed separately for each beam and channel. 
Output is a fitacf file.

(C) Copyright 2021 E.C.Bland
author: E.C.Bland

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
       
Modifications:

   
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

// function to return the index in an array
int get_index(int a, int b, int c, int d, int aSize, int bSize, int cSize) {
    return (d * aSize * bSize * cSize) + (c * aSize * bSize) + (b * aSize) + a; 
}

// structure to store the qflgs. Can be dynamically resized
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

  // Initial memory allocation to store qflgs
  qflgData *qflgs=malloc(sizeof(qflgData));
  qflgs->value = malloc(tmax*maxrng*maxch*maxbm * sizeof(int));
  qflgs->used = 0;
  qflgs->size = tmax*maxrng*maxch*maxbm;
  
  
  // Read the FITACF file to populate the qflg array
  int index;
  int maxindex=-1;
  int rng,bm,ch;
  int nrec[maxbm][maxch]; // counts number of records for each beam/channel
  memset(nrec,0,sizeof(nrec));
  
  do {
  
    bm=prm->bmnum;
    ch=prm->channel;
    for (rng=0;rng<prm->nrang;rng++) {
      index=get_index(rng,bm,ch,nrec[bm][ch],maxrng,maxbm,maxch);
      if (maxindex < index) maxindex=index;
      if (qflgs->size < maxindex) {
        qflgs->size += tmax*maxrng*maxch*maxbm;
        qflgs->value = realloc(qflgs->value, qflgs->size * sizeof(int));
      }
      qflgs->value[index] = fit->rng[rng].qflg;
      qflgs->used = maxindex;
      
    }
    nrec[bm][ch]++;
  
  } while (FitFread(fp,prm,fit) !=-1);
  
  
  // Do the median filtering
  //   Since qflg can only be 0 or 1, the median qflg of the 3x3 grid can be 
  //   calculated using the test ( sum_of_qflgs >= 5 )
  int sum;
  int index_list[9];  
  memset(index_list,-1,sizeof(index_list));
  
  
  //** Read the file again, apply the median filter, and write a new file
  // rewind the file pointer
  rewind(fp);
  if (FitFread(fp,prm,fit)==-1) {
    fprintf(stderr,"Error reading file\n");
    exit(-1);
  }
  memset(nrec,0,sizeof(nrec));
  int t[maxbm][maxch];
  memset(t,0,sizeof(t));
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
      
      if (fit->rng[rng].qflg==1) {
        index_list[0]=get_index(rng  ,bm,ch,t[bm][ch],maxrng,maxbm,maxch);
        index_list[1]=get_index(rng-1,bm,ch,t[bm][ch],maxrng,maxbm,maxch);
        index_list[2]=get_index(rng+1,bm,ch,t[bm][ch],maxrng,maxbm,maxch);
        index_list[3]=get_index(rng,  bm,ch,t[bm][ch]-1,maxrng,maxbm,maxch);
        index_list[4]=get_index(rng-1,bm,ch,t[bm][ch]-1,maxrng,maxbm,maxch);
        index_list[5]=get_index(rng+1,bm,ch,t[bm][ch]-1,maxrng,maxbm,maxch);
        index_list[6]=get_index(rng,  bm,ch,t[bm][ch]+1,maxrng,maxbm,maxch);
        index_list[7]=get_index(rng-1,bm,ch,t[bm][ch]+1,maxrng,maxbm,maxch);
        index_list[8]=get_index(rng+1,bm,ch,t[bm][ch]+1,maxrng,maxbm,maxch);
        
        // corners
        if (t[bm][ch]==0 && rng==0)
          sum=3*qflgs->value[index_list[0]] + 2*qflgs->value[index_list[2]] + 2*qflgs->value[index_list[6]]+ qflgs->value[index_list[8]];
        else if (t[bm][ch]==0 && rng==maxrng-1)
          sum=3*qflgs->value[index_list[0]] + 2*qflgs->value[index_list[1]] + 2*qflgs->value[index_list[6]] + qflgs->value[index_list[7]];
        else if (t[bm][ch]==nrec[bm][ch] && rng==0)
          sum=3*qflgs->value[index_list[0]] + 2*qflgs->value[index_list[2]] + 2*qflgs->value[index_list[3]] + qflgs->value[index_list[5]];
        else if (t[bm][ch]==nrec[bm][ch] && rng==maxrng-1)
          sum=3*qflgs->value[index_list[0]] + 2*qflgs->value[index_list[1]] + 2*qflgs->value[index_list[3]] + qflgs->value[index_list[4]];
      
        // edges
        else if (t[bm][ch]==0)
          sum=2*qflgs->value[index_list[0]] + 2*qflgs->value[index_list[1]] + 2*qflgs->value[index_list[2]] + qflgs->value[index_list[6]] + qflgs->value[index_list[7]] + qflgs->value[index_list[8]];
        else if (t[bm][ch]==nrec[bm][ch])
          sum=2*qflgs->value[index_list[0]] + 2*qflgs->value[index_list[1]] + 2*qflgs->value[index_list[2]] + qflgs->value[index_list[3]] + qflgs->value[index_list[4]] + qflgs->value[index_list[5]];
        else if (rng==0)
          sum=2*qflgs->value[index_list[0]] + qflgs->value[index_list[2]] + 2*qflgs->value[index_list[3]] + qflgs->value[index_list[5]] + 2*qflgs->value[index_list[6]] + qflgs->value[index_list[8]];
        else if (rng==maxrng-1)
          sum=2*qflgs->value[index_list[0]] + qflgs->value[index_list[1]] + 2*qflgs->value[index_list[3]] + qflgs->value[index_list[4]] + 2*qflgs->value[index_list[6]] + qflgs->value[index_list[7]];
        
        // all other cells
        else sum=qflgs->value[index_list[0]] + qflgs->value[index_list[1]] + qflgs->value[index_list[2]] + qflgs->value[index_list[3]] + qflgs->value[index_list[4]] + qflgs->value[index_list[5]] + qflgs->value[index_list[6]] + qflgs->value[index_list[7]] + qflgs->value[index_list[8]];
        
        // Remove record if median=0 (sum of qflgs < 5)
        if (sum < 5) fit->rng[rng].qflg=0;
      }
    }
    t[bm][ch]++;
    
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
  
  
  return 0;
}
