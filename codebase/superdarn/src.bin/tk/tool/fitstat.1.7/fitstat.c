/* fitstat.c
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

/*Generates statistics from fit files*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <zlib.h>
#include "rtypes.h"
#include "rtime.h"
#include "dmap.h"
#include "option.h"
#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"
#include "version.h"

#include "fitread.h"
#include "oldfitread.h"

#include "errstr.h"
#include "hlpstr.h"






struct RadarParm *prm;
struct FitData *fit;
struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: fitstat --help\n");
  return(-1);
}

int main (int argc,char *argv[]) {

  int old=0;

  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  struct OldFitFp *fitfp=NULL;
  FILE *fp=NULL;


  int yr,mo,dy,hr,mt;
  double sc,st_time,ed_time;
  int bmcnt;
  int stid;

  prm=RadarParmMake();
  fit=FitMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

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


  if ((old) && (arg==argc)) {
    OptionPrintInfo(stdout,errstr);
    exit(-1);
  }


  if (old) {

    fitfp=OldFitOpen(argv[arg],NULL);
    if (fitfp==NULL) {
      fprintf(stderr,"file %s not found\n",argv[arg]);
      exit(1);
    }
    bmcnt=0;
    st_time=-1;
    ed_time=-1;
    stid=0;
    while (OldFitRead(fitfp,prm,fit) !=-1) {
      ed_time=TimeYMDHMSToEpoch(prm->time.yr,prm->time.mo,prm->time.dy,
             prm->time.hr,prm->time.mt,prm->time.sc);


      if (bmcnt==0) {
        st_time=ed_time;
        stid=prm->stid;
      }
      bmcnt++;
    }
    if (bmcnt>0) {

      fprintf(stdout,"%s:",argv[arg]);
      TimeEpochToYMDHMS(st_time,&yr,&mo,&dy,&hr,&mt,&sc);
      fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d",yr,mo,dy,hr,mt,(int) sc);
      TimeEpochToYMDHMS(ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
      fprintf(stdout,":");
      fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d ",yr,mo,dy,hr,mt,
                        (int) sc);
      fprintf(stdout,"[%.2d] (%d)\n",stid,bmcnt);
    } else {
        fprintf(stderr,"Error, no beams read. bmcnt is: %d\n",bmcnt);
        exit(2);
    }
    OldFitClose(fitfp);
  } else {
    fp=fopen(argv[arg],"r");
    if (fp==NULL) {
      fprintf(stderr,"Could not open file.\n");
      exit(1);
    }
    bmcnt=0;
    st_time=-1;
    ed_time=-1;
    stid=0;
    while (FitFread(fp,prm,fit) !=-1) {
      ed_time=TimeYMDHMSToEpoch(prm->time.yr,prm->time.mo,prm->time.dy,
               prm->time.hr,prm->time.mt,prm->time.sc);
      if (bmcnt==0) {
        st_time=ed_time;
        stid=prm->stid;
      }
      bmcnt++;
    }
    if (bmcnt>0) {
      fprintf(stdout,"%s:",argv[arg]);
      TimeEpochToYMDHMS(st_time,&yr,&mo,&dy,&hr,&mt,&sc);
      fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d",yr,mo,dy,hr,mt,(int) sc);
      TimeEpochToYMDHMS(ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
      fprintf(stdout,":");
      fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d ",yr,mo,dy,hr,mt,
                        (int) sc);
      fprintf(stdout,"[%.2d] (%d)\n",stid,bmcnt);
    } else {
        fprintf(stderr,"Error, no beams read. bmcnt is: %d\n",bmcnt);
        exit(1);
    }
    fclose(fp);
  }
  exit(0);
  return 0;
}

