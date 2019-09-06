/* make_fitex.c
   ===========
   Author: R.J.Barnes
*/

/*
 LICENSE AND DISCLAIMER

 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

 This file is part of the Radar Software Toolkit (RST).

 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.

 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.



*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <zlib.h>

#include "rtypes.h"
#include "option.h"

#include "dmap.h"
#include "rprm.h"
#include "rawdata.h"
#include "fitblk.h"
#include "fitdata.h"
#include "radar.h"

#include "fitacfex.h"
#include "rawread.h"
#include "fitwrite.h"

#include "oldrawread.h"
#include "oldfitwrite.h"

#include "errstr.h"
#include "hlpstr.h"

struct RadarParm *prm;
struct RawData *raw;
struct FitData *fit;

struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: make_fitex --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {

  unsigned char old=0;

  int status;
  int arg;

  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  unsigned char vb=0;

  FILE *fp=NULL;
  struct OldRawFp *rawfp=NULL;
  FILE *fitfp=NULL;
  FILE *inxfp=NULL;
  int irec=1;
  int drec=2;
  int dnum=0;

  time_t ctime;
  int c,n;
  char command[128];
  char tmstr[40];

  prm=RadarParmMake();
  raw=RawMake();
  fit=FitMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"vb",'x',&vb);

  OptionAdd(&opt,"old",'x',&old);

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




  if ((old) && (argc-arg<2)) {
    OptionPrintInfo(stdout,hlpstr);
    exit(-1);
  }

  if (old) {
    rawfp=OldRawOpen(argv[arg],NULL);
    if (rawfp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(-1);
    } else if (rawfp->error==-2) {
        /* Error case where num_bytes is less than 0 */
        free(rawfp);
        exit(-1);
    }
    status=OldRawRead(rawfp,prm,raw);
  } else {
    if (arg==argc) fp=stdin;
    else fp=fopen(argv[arg],"r");

    if (fp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(-1);
    }
    status=RawFread(fp,prm,raw);
  }

  command[0]=0;
  n=0;
  for (c=0;c<argc;c++) {
    n+=strlen(argv[c])+1;
    if (n>127) break;
    if (c !=0) strcat(command," ");
    strcat(command,argv[c]);
  }



  if (vb)
      fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",prm->time.yr,prm->time.mo,
	     prm->time.dy,prm->time.hr,prm->time.mt,prm->time.sc,prm->bmnum);



  FitACFex(prm,raw,fit);

  if (old) {
    char vstr[256];
    fitfp=fopen(argv[arg+1],"w");
    if (fitfp==NULL) {
      fprintf(stderr,"Could not create fit file.\n");
      exit(-1);
    }
    if (argc-arg>2) {
      inxfp=fopen(argv[arg+2],"w");
      if (inxfp==NULL) {
        fprintf(stderr,"Could not create inx file.\n");
        exit(-1);
      }
    }
    sprintf(vstr,"%d.%d",fit->revision.major,fit->revision.minor);
    OldFitHeaderFwrite(fitfp,"make_fit","fitacf",vstr);
    if (inxfp !=NULL) OldFitInxHeaderFwrite(inxfp,prm);
  }



  do {


    ctime = time((time_t) 0);
    RadarParmSetOriginCommand(prm,command);
    strcpy(tmstr,asctime(gmtime(&ctime)));
    tmstr[24]=0;
    RadarParmSetOriginTime(prm,tmstr);

    if (old) {
       dnum=OldFitFwrite(fitfp,prm,fit,NULL);
       if (inxfp !=NULL) OldFitInxFwrite(inxfp,drec,dnum,prm);
       drec+=dnum;
       irec++;
    } else status=FitFwrite(stdout,prm,fit);

    if (old) status=OldRawRead(rawfp,prm,raw);
    else status=RawFread(fp,prm,raw);

     if (vb)
      fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",prm->time.yr,prm->time.mo,
	     prm->time.dy,prm->time.hr,prm->time.mt,prm->time.sc,prm->bmnum);

    FitACFex(prm,raw,fit);


  } while (status==0);

  if (old) OldRawClose(rawfp);
  return 0;
}













