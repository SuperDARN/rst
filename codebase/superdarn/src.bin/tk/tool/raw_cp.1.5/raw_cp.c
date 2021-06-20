/* raw_cp.c
   ========
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
#include "option.h"
#include "rprm.h"
#include "rawdata.h"

#include "rawread.h"
#include "oldrawread.h"

#include "hlpstr.h"
#include "errstr.h"



int cpid[256];
int cpcnt=0;

struct RadarParm *prm;
struct RawData *raw;
struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: raw_cp --help\n");
  return(-1);
}

int main (int argc,char *argv[]) {

  int old=0;

  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  struct OldRawFp *rawfp=NULL;
  FILE *fp=NULL;
  int c,i;

  prm=RadarParmMake();
  raw=RawMake();

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
      fprintf(stderr,"Opening file %s\n",argv[c]);
      rawfp=OldRawOpen(argv[c],NULL);
      if (rawfp==NULL) {
        fprintf(stderr,"File %s not found.\n",argv[c]);
        continue;
      } else if (rawfp->error==-2) {
        /* Error code where num_bytes < 0 */
        free(rawfp);
        continue;
      }
      while (OldRawRead(rawfp,prm,raw) !=-1) {
        for (i=0;i<cpcnt;i++) if (cpid[i]==prm->cp) break;
        if (i>=cpcnt) {
          cpid[cpcnt]=prm->cp;
          cpcnt++;
        }
      }
      OldRawClose(rawfp);
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
    while (RawFread(fp,prm,raw) !=-1) {
      for (i=0;i<cpcnt;i++) if (cpid[i]==prm->cp) break;
        if (i>=cpcnt) {
          cpid[cpcnt]=prm->cp;
          cpcnt++;
        }
      }
    if (fp !=stdin) fclose(fp);
  }
  fprintf(stdout,"%d\n",cpcnt);
  for (i=0;i<cpcnt;i++) fprintf(stdout,"%.2d:%d\n",i,cpid[i]);
  exit(cpcnt);

  return cpcnt;
}


















