/* cat_raw.c
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
#include <sys/stat.h>
#include <zlib.h>
#include "rtypes.h"
#include "option.h"
#include "dmap.h"
#include "rprm.h"
#include "rawdata.h"
#include "version.h"

#include "oldrawread.h"
#include "oldrawwrite.h"

#include "errstr.h"
#include "hlpstr.h"

struct RadarParm *prm;
struct RawData *raw;

struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: cat_raw --help\n");
  return(-1);
}

int main (int argc,char *argv[]) {
  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;
  int old=0;
  float thr=-1;
  int hflg=1;
  int i;
  int recnum=0;
  struct OldRawFp *infp=NULL;
  char vstring[256];
  FILE *outfp=NULL;

  prm=RadarParmMake();
  raw=RawMake();

  sprintf(vstring,"%s.%s",MAJOR_VERSION,MINOR_VERSION);

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);
  OptionAdd(&opt,"t",'f',&thr);

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

  if (old==0) {
    fprintf(stderr,
    "New format files can be concatenated with the cat command.\n");
    exit(-1);
  }

  if (thr !=-1) {
    fprintf(stderr,"Error: the -t threshold option has been deprecated.\n");
    exit(-1);
  }

  if (argc-arg<2) {
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }

  outfp=fopen(argv[argc-1],"w");
  if (outfp==NULL) {
    fprintf(stderr,"Could not open output file.\n");
    exit(-1);
  }

  for (i=arg;i<argc-1;i++) {
    infp=OldRawOpen(argv[i],NULL);
    if (infp==NULL) {
      fprintf(stderr,"Could not open file %s.\n",argv[i]);
      continue;
    } else if(infp->error==-2) {
        /* Error case where num_bytes is less than 0 */
        free(infp);
        exit(-1);
    }
    while (OldRawRead(infp,prm,raw) !=-1) {
      if (thr !=-1) raw->thr=thr;
      if (hflg==1) {
	if (OldRawHeaderFwrite(outfp,"rawwrite",vstring,raw->thr,
			       "concatenated with cat_raw") !=0) {
	  fprintf(stderr,"Could not write header.\n");
	  exit(-1);
	}
        hflg=0;
      }


      recnum++;
      OldRawFwrite(outfp,"rawwrite",prm,raw,recnum,NULL);
    }
    OldRawClose(infp);
  }
  fclose(outfp);

  return 0;
}






















