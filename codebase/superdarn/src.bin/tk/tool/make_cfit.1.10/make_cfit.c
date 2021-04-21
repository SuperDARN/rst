/* make_cfit.c
   ===========
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
#include "option.h"
#include "rtypes.h"
#include "rtime.h"
#include "dmap.h"
#include "option.h"
#include "rprm.h"
#include "fitdata.h"
#include "cfitdata.h"
#include "fitread.h"
#include "fitcfit.h"
#include "cfitwrite.h"

#include "oldfitread.h"

#include "hlpstr.h"
#include "errstr.h"

struct RadarParm *prm;
struct FitData *fit;
struct CFitdata *cfit;

struct OptionData opt;




double strdate(char *text) {
  double tme;
  int val;
  int yr,mo,dy;
  val=atoi(text);
  dy=val % 100;
  mo=(val / 100) % 100;
  yr=(val / 10000);
  if (yr<1970) yr+=1900;  
  tme=TimeYMDHMSToEpoch(yr,mo,dy,0,0,0);

  return tme;
}

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: make_cfit --help\n");
  return(-1);
}

int main (int argc,char *argv[]) {

  int old=0;

  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;
  unsigned char vb=0;

  char *dtestr=NULL;
  double dtval=-1;

  struct OldFitFp *fitfp=NULL;
  FILE *fp=NULL;
  gzFile gzfp;
  int c;
  double minpwr=0.0; /* default behaviour is not to filter */

  prm=RadarParmMake();
  fit=FitMake();
  cfit=CFitMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"old",'x',&old); 

  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"p",'d',&minpwr);

  OptionAdd(&opt,"d",'t',&dtestr); /* hidden d option to filter out data
                                      not from this day 
				   */
 
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


  if ((old) && (arg==argc)) {
    OptionPrintInfo(stdout,errstr);
    exit(-1);
  }


  if (dtestr !=NULL) dtval=strdate(dtestr);

  gzfp=gzdopen(fileno(stdout),"w");


  if (old) {
    for (c=arg;c<argc;c++) {
      fitfp=OldFitOpen(argv[c],NULL); 
      fprintf(stderr,"Opening file %s\n",argv[c]);
      if (fitfp==NULL) {
        fprintf(stderr,"file %s not found\n",argv[c]);
        continue;
      }
 
      while (OldFitRead(fitfp,prm,fit) !=-1) {
	
        FitToCFit(minpwr,cfit,prm,fit);
        if (  (dtval !=-1) && 
	      ( ((cfit->time-dtval)<0) || ((cfit->time-dtval)>24*3600) )) {
          if (vb) fprintf(stderr,"%d-%d-%d %d:%d:%d excluded\n",
                   prm->time.yr,prm->time.mo,prm->time.dy,
                   prm->time.hr,prm->time.mt,
                   prm->time.sc);
          continue;
	}  
	
        if (vb) fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d ranges=%d\n",
                   prm->time.yr,prm->time.mo,prm->time.dy,
                   prm->time.hr,prm->time.mt,
                   prm->time.sc,prm->bmnum,cfit->num);
	
        CFitWrite(gzfp,cfit);
	
      } 
      OldFitClose(fitfp);
    }
  } else {
    if (arg==argc) {
      while (FitFread(stdin,prm,fit) !=-1) {
        FitToCFit(minpwr,cfit,prm,fit);
        if (  (dtval !=-1) && 
	      ( ((cfit->time-dtval)<0) || ((cfit->time-dtval)>24*3600) )) {
          if (vb) fprintf(stderr,"%d-%d-%d %d:%d:%d excluded\n",
                   prm->time.yr,prm->time.mo,prm->time.dy,
                   prm->time.hr,prm->time.mt,
                   prm->time.sc);
          continue;
	}  

   
        if (vb) fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d ranges=%d\n",
                   prm->time.yr,prm->time.mo,prm->time.dy,
                   prm->time.hr,prm->time.mt,
                   prm->time.sc,prm->bmnum,cfit->num);

        CFitWrite(gzfp,cfit);
      }
    } else {
      for (c=arg;c<argc;c++) {
        fprintf(stderr,"Opening file %s\n",argv[c]);
        fp=fopen(argv[c],"r");
        if (fp==NULL) {
          fprintf(stderr,"Could not open file.\n");
          continue;
	}
        while (FitFread(fp,prm,fit) !=-1) {
          FitToCFit(minpwr,cfit,prm,fit);

          if (vb) fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d ranges=%d\n",
                   prm->time.yr,prm->time.mo,prm->time.dy,
                   prm->time.hr,prm->time.mt,
                   prm->time.sc,prm->bmnum,cfit->num);

          CFitWrite(gzfp,cfit);
	}
        fclose(fp);
      }
    }
  }
  gzclose(gzfp);
  return 0;
} 






















