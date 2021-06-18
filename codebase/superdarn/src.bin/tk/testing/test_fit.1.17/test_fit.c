/* test_fit.c
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

/*Demonstration program that opens and reads fit files*/

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
#include "option.h"
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
  fprintf(stderr,"Please try: test_fit --help\n");
  return(-1);
}

int main (int argc,char *argv[]) {

  int old=0;

  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  int i;
  struct OldFitFp *fitfp=NULL;
  FILE *fp=NULL;
  int c;

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
    for (c=arg;c<argc;c++) {
      fitfp=OldFitOpen(argv[c],NULL); 
      fprintf(stderr,"Opening file %s\n",argv[c]);
      if (fitfp==NULL) {
        fprintf(stderr,"file %s not found\n",argv[c]);
        continue;
      }
      while (OldFitRead(fitfp,prm,fit) !=-1) {
        fprintf(stdout,
	  "%d-%d-%d %d:%d:%d beam=%d bmazm=%f channel=%d cpid=%d scan=%d\n",
               prm->time.yr,prm->time.mo,prm->time.dy,
               prm->time.hr,prm->time.mt,prm->time.sc,
               prm->bmnum,prm->bmazm,prm->channel,prm->cp,prm->scan);

        fprintf(stdout,"origin.code=%d\norigin.time=%s\norigin.command=%s\n",
                 prm->origin.code,prm->origin.time,prm->origin.command);
        fprintf(stdout,"v:\n");
        for (i=0;i<prm->nrang;i++) {
          fprintf(stdout,"%.4g",fit->rng[i].v);
          if ((i % 8)==0) fprintf(stdout,"\n");
          else fprintf(stdout,"\t");
        }

        fprintf(stdout,"v_e:\n");
        for (i=0;i<prm->nrang;i++) {
          fprintf(stdout,"%.4g",fit->rng[i].v_err);
          if ((i % 8)==0) fprintf(stdout,"\n");
          else fprintf(stdout,"\t");
        }
        fprintf(stdout,"\n");
        fprintf(stdout,"p_l:\n");
        for (i=0;i<prm->nrang;i++) {
          fprintf(stdout,"%.4g",fit->rng[i].p_l);
          if ((i % 8)==0) fprintf(stdout,"\n");
          else fprintf(stdout,"\t");
        }
        fprintf(stdout,"\n");
        fprintf(stdout,"w_l:\n");
        for (i=0;i<prm->nrang;i++) {
          fprintf(stdout,"%.4g",fit->rng[i].w_l);
          if ((i % 8)==0) fprintf(stdout,"\n");
          else fprintf(stdout,"\t");
        }
        fprintf(stdout,"\n");
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
    while (FitFread(fp,prm,fit) !=-1) {
        fprintf(stdout,"%d %d\n",fit->revision.major,fit->revision.minor);
        fprintf(stdout,
	   "%d-%d-%d %d:%d:%d beam=%d  bmazm=%f channel=%d cpid=%d scan=%d\n",
               prm->time.yr,prm->time.mo,prm->time.dy,
               prm->time.hr,prm->time.mt,prm->time.sc,
               prm->bmnum,prm->bmazm,prm->channel,prm->cp,prm->scan);
        fprintf(stdout,"origin.code=%d\norigin.time=%s\norigin.command=%s\n",
                 prm->origin.code,prm->origin.time,prm->origin.command);
        fprintf(stdout,"combf=%s\n",prm->combf);
        fprintf(stdout,"v:\n");
        for (i=0;i<prm->nrang;i++) {
          fprintf(stdout,"%.4g",fit->rng[i].v);
          if ((i % 8)==0) fprintf(stdout,"\n");
          else fprintf(stdout,"\t");
        }
        fprintf(stdout,"\n");
        fprintf(stdout,"v_e:\n");
        for (i=0;i<prm->nrang;i++) {
          fprintf(stdout,"%.4g",fit->rng[i].v_err);
          if ((i % 8)==0) fprintf(stdout,"\n");
          else fprintf(stdout,"\t");
        }
        fprintf(stdout,"\n");
        fprintf(stdout,"p_l:\n");
        for (i=0;i<prm->nrang;i++) {
          fprintf(stdout,"%.4g",fit->rng[i].p_l);
          if ((i % 8)==0) fprintf(stdout,"\n");
          else fprintf(stdout,"\t");
        }
        fprintf(stdout,"\n");
        fprintf(stdout,"w_l:\n");
        for (i=0;i<prm->nrang;i++) {
          fprintf(stdout,"%.4g",fit->rng[i].w_l);
          if ((i % 8)==0) fprintf(stdout,"\n");
          else fprintf(stdout,"\t");
        }
        fprintf(stdout,"\n");

    }
    if (fp !=stdin) fclose(fp);
  }


  return 0;
} 






















