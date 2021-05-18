/* merge_grid.c
   ==============
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
#include <math.h>
#include <sys/types.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "rfile.h"
#include "griddata.h"
#include "gridread.h"
#include "oldgridread.h"
#include "gridwrite.h"
#include "oldgridwrite.h"
#include "hlpstr.h"



struct GridData *rcd;
struct GridData *mrg;

struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: merge_grid --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {
  
  int arg;

  int old=0;

  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  unsigned char vb=0;

  FILE *fp;   
  char *fname=NULL;
 
  double tme;
  int yr,mo,dy,hr,mt;
  double sc;
 
  rcd=GridMake();
  mrg=GridMake();

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


  if (arg<argc) fname=argv[arg];
  if (fname==NULL) fp=stdin;
  else fp=fopen(fname,"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }
  if (old) {
    while (OldGridFread(fp,rcd) !=-1) {

      GridMerge(rcd,mrg);

      if (vb) {
        tme=rcd->st_time;
        TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
        fprintf(stderr,"%.4d %.2d %.2d %.2d %.2d %.2d merge vectors=%d\n",
		       yr,mo,dy,hr,mt,(int) sc,mrg->vcnum);
      }
      OldGridFwrite(stdout,mrg);
    }
  } else {

    while (GridFread(fp,rcd) !=-1) {

      GridMerge(rcd,mrg);

      if (vb) {
        tme=rcd->st_time;
        TimeEpochToYMDHMS(tme,&yr,&mo,&dy,&hr,&mt,&sc);
        fprintf(stderr,"%.4d %.2d %.2d %.2d %.2d %.2d merge vectors=%d\n",
		       yr,mo,dy,hr,mt,(int) sc,mrg->vcnum);
      }
      GridFwrite(stdout,mrg);
    }
  }
  if (fp !=stdout) fclose(fp);
  

  return 0;
}






















