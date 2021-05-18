/* make_smr.c
   ==========
   Author: R.J.Barnes
*/

/*
  Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "option.h"
#include "rtime.h"
#include "rprm.h"
#include "fitdata.h"
#include "smrwrite.h"
#include "fitread.h"

#include "oldfitread.h"

#include "errstr.h"
#include "hlpstr.h"

struct RadarParm *prm;
struct FitData *fit;
struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: make_smr --help\n");
  return(-1);
}

int main (int argc,char *argv[]) {

  int old=0;

  int arg;
  int i;
  int c=1;
  char *bstr=NULL;
  struct OldFitFp *fitfp=NULL;
  FILE *fp;
  double min_pwr=3.0;
  char bflg[16];
  int bnum=0,b;
  unsigned char vb=0;
  unsigned char help=0;
  unsigned char version=0;
  unsigned char option=0;

  prm=RadarParmMake();
  fit=FitMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"old",'x',&old); 
  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"t",'d',&min_pwr);
  OptionAdd(&opt,"b",'t',&bstr); 

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



  if (min_pwr<0) min_pwr=0;
  if (min_pwr>30) min_pwr=30;

  for (i=0;i<16;i++) bflg[i]=0;
  bnum=0;

  if (bstr !=NULL) {
    char *tmp;
    tmp=strtok(bstr,",");
    do {
       b=atoi(tmp);
       if (bflg[b]==0) {
         bflg[b]=1;
         bnum++;
       }
     } while ((tmp=strtok(NULL,",")) !=NULL);
  }

  if ((old) && (arg==argc)) {
     OptionPrintInfo(stdout,errstr);
     exit(-1);
  }
    
  if (bnum==0) {
     bflg[8]=1;
     bnum=1;
  }
  if (old) {
    for (c=arg;c<argc;c++) {
      fitfp=OldFitOpen(argv[c],NULL); 
      if (vb) fprintf(stderr,"Opening file %s\n",argv[c]);
      if (fitfp==NULL) {
        if (vb) fprintf(stderr,"file %s not found\n",argv[c]);
        continue;
      }
 
      SmrHeaderFwrite(stdout,"1.00","make_smr","make_smr");
    

      while (OldFitRead(fitfp,prm,fit) !=-1) {

        if (bflg[prm->bmnum]==0) continue;
 
        if (vb) fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",
		      prm->time.dy,prm->time.mo,prm->time.dy,prm->time.hr,
                      prm->time.mt,prm->time.sc,prm->bmnum);
        SmrFwrite(stdout,prm,fit,min_pwr);            
      } 
      OldFitClose(fitfp);
    }
  } else {
    if (arg==argc) fp=stdin;
    else {
      fp=fopen(argv[arg],"r");
      if (fp==NULL) {
        fprintf(stderr,"Could not open file.\n");
        exit(-1);
      }
    }
    SmrHeaderFwrite(stdout,"1.00","make_smr","make_smr");
    while (FitFread(fp,prm,fit) !=-1) {
      if (bflg[prm->bmnum]==0) continue;
 
        if (vb) fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",
		      prm->time.dy,prm->time.mo,prm->time.dy,prm->time.hr,
                      prm->time.mt,prm->time.sc,prm->bmnum);
        SmrFwrite(stdout,prm,fit,min_pwr);            
      }
    if (fp !=stdin) fclose(fp);
  } 
  return 0;
} 






















