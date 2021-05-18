/* cat_fit.c
   =========
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
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <zlib.h>
#include "rtypes.h"
#include "option.h"
#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"

#include "oldfitread.h"
#include "oldfitwrite.h"

#include "errstr.h"
#include "hlpstr.h"

struct RadarParm *pbuf[2];
struct FitData *fbuf[2];

struct RadarParm *prm=NULL;
struct FitData *fit=NULL;

int fnum=0;

struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: cat_fit --help\n");
  return(-1);
}

int main (int argc,char *argv[]) {
  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  int old=0;

  int i;
  
  struct OldFitFp *fp=NULL;
  FILE *fitfp=NULL;
  FILE *inxfp=NULL;
  int irec=1;
  int drec=2;
  
  unsigned char inx=0; 
  int dnum=0;

  pbuf[0]=RadarParmMake();
  pbuf[1]=RadarParmMake();
  fbuf[0]=FitMake();
  fbuf[1]=FitMake();
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"old",'x',&old);
  OptionAdd(&opt,"i",'x',&inx);

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

  if (old==0) {
    fprintf(stderr,
	    "New format files can be concatenated using the cat command\n");
    exit(-1);
  }

  if (argc-arg<2+inx) {
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }
      	  
  fitfp=fopen(argv[argc-1-inx],"w");
  if (fitfp==NULL) {
    fprintf(stderr,"Could not create fit file.\n");
    exit(-1);
  }
  OldFitHeaderFwrite(fitfp,"cat_fit","fitacf","4.0");

  if (inx !=0) {
    inxfp=fopen(argv[argc-1],"w");
    if (inxfp==NULL) fprintf(stderr,"Could not create index file.\n");
  }
 
  for (i=arg;i<argc-1-inx;i++) {
    fp=OldFitOpen(argv[i],NULL); 
    if (fp==NULL) {
      fprintf(stderr,"file %s not found\n",argv[i]);
      continue;
    }
    while (OldFitRead(fp,pbuf[fnum],fbuf[fnum]) !=-1) {
      prm=pbuf[fnum];
      fit=fbuf[fnum];
      fnum=(fnum+1) % 2;
      if ((inxfp !=NULL) && (irec==1)) OldFitInxHeaderFwrite(inxfp,prm);
      dnum=OldFitFwrite(fitfp,prm,fit,NULL);
      if (inxfp !=NULL) OldFitInxFwrite(inxfp,drec,dnum,prm);
      drec+=dnum;
      irec++;
    } 
    OldFitClose(fp);
  }
  if (inxfp !=NULL) {
    fclose(inxfp);
    inxfp=fopen(argv[argc-1],"r+");
    OldFitInxFclose(inxfp,prm,irec-1);
    fclose(inxfp);
  }
  fclose(fitfp);
  return 0;
} 






















