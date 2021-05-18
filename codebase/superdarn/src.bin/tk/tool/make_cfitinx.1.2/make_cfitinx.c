/* make_cfitinx.c
   =============
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
#include "rtime.h"
#include "option.h"
#include "cfitdata.h"
#include "cfitread.h"

#include "errstr.h"
#include "hlpstr.h"




struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: make_cfitinx --help\n");
  return(-1);
}

int main (int argc,char *argv[]) {



  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;
  unsigned char vb=0;

  int yr,mo,dy,hr,mt;
  double sc;

  struct CFitfp *cfitfp;
  struct CFitdata *cfit=NULL;


  int ptr=0;

  cfit=CFitMake();

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

  if (arg==argc) {
    OptionPrintInfo(stdout,errstr);
    exit(-1);
  }

  cfitfp=CFitOpen(argv[arg]);
  if (cfitfp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }
  while (CFitRead(cfitfp,cfit) !=-1) {
 
    if (vb) {
      TimeEpochToYMDHMS(cfit->time,&yr,&mo,&dy,&hr,&mt,&sc);
      fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",yr,mo,
	      dy,hr,mt,(int) sc,cfit->bmnum);
    }

    ptr=gztell(cfitfp->fp);
    ConvertFwriteDouble(stdout,cfit->time);
    ConvertFwriteInt(stdout,ptr);

    
  }
  CFitClose(cfitfp);

 
  return 0;
} 






















