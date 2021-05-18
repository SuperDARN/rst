/* make_fitinx.c
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
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "rconvert.h"
#include "option.h"
#include "rtime.h"
#include "rprm.h"
#include "fitdata.h"

#include "fitread.h"
#include "fitwrite.h"
#include "fitindex.h"

#include "oldfitread.h"
#include "oldfitwrite.h"

#include "errstr.h"
#include "hlpstr.h"




struct RadarParm *pbuf[2];
struct FitData *fbuf[2];

struct FitData *fit;
struct RadarParm *prm;

int fnum=0;

struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: make_fitinx --help\n");
  return(-1);
}

int main (int argc,char *argv[]) {

  int old=0;

  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  unsigned char vb=0;

  int i=0;

 
  FILE *fp;

  int sze=0;
  double tval;

  struct OldFitFp *fitfp=NULL;
  FILE *inxfp=NULL;
  
  int irec=1;
  int drec=2;
  int dnum=0;
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"old",'x',&old);
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


  if (old) {

    pbuf[0]=RadarParmMake();
    pbuf[1]=RadarParmMake();

    fbuf[0]=FitMake();
    fbuf[1]=FitMake();
 
    if (argc-arg<2) {
      OptionPrintInfo(stdout,errstr);
      exit(-1);
    }
     	  
    inxfp=fopen(argv[argc-1],"w");
    if (inxfp==NULL) {
      fprintf(stderr,"could not create index file.\n");
      exit(-1);  
    }

  
    fitfp=OldFitOpen(argv[argc-2],NULL); 
    if (fitfp==NULL) {
      fprintf(stderr,"file %s not found\n",argv[i]);
      exit(-1);
    }

 
    while (OldFitRead(fitfp,pbuf[fnum],fbuf[fnum]) !=-1) {
      fit=fbuf[fnum];
      prm=pbuf[fnum];
      fnum=(fnum+1) % 2;
      if (irec==1) OldFitInxHeaderFwrite(inxfp,prm);
      dnum=OldFitFwrite(NULL,prm,fit,NULL);
      OldFitInxFwrite(inxfp,drec,dnum,prm);
      drec+=dnum;
      irec++;
      if (vb) 
        fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",prm->time.yr,
             prm->time.mo,prm->time.dy,prm->time.hr,prm->time.mt,
             prm->time.sc,prm->bmnum);


    } 
    OldFitClose(fitfp);
  
    fclose(inxfp);
    inxfp=fopen(argv[argc-1],"r+");
    OldFitInxFclose(inxfp,prm,irec-1);
    fclose(inxfp);
  } else {
    prm=RadarParmMake();
    fit=FitMake();

    if (arg==argc) fp=stdin;
    else fp=fopen(argv[arg],"r");
    if (fp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(-1);
    }
    
    while (FitFread(fp,prm,fit) !=-1) {
      if (vb) 
        fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",prm->time.yr,prm->time.mo,
	     prm->time.dy,prm->time.hr,prm->time.mt,prm->time.sc,prm->bmnum);

      tval=TimeYMDHMSToEpoch(prm->time.yr,
		             prm->time.mo,
                             prm->time.dy,
                             prm->time.hr,
		             prm->time.mt,
                             prm->time.sc+prm->time.us/1.0e6);
      ConvertFwriteDouble(stdout,tval);
      ConvertFwriteInt(stdout,sze);
      sze+=FitWrite(-1,prm,fit);
    }
    if (fp !=stdin) fclose(fp);
  }   




  return 0;
} 






















