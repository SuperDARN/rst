/* combine_grid.c
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

#include "errstr.h"
#include "hlpstr.h"
#include "make_grid.h"



char *fname[64];
int fnum=4;

FILE *in_fp[64];

struct GridData *in_rcd[64];
int read_flg[64];
int dflg[64];

struct GridData *rcd;

struct OptionData opt;

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: combine_grid --help\n");
  return(-1);
}

int read_set(int flg) {
  int c=0;
  int i;
  int status=0;
  
  for (i=0;i<fnum;i++) {
    if ((in_fp[i] !=NULL) && (read_flg[i]==1)) {
      if (flg)  status=OldGridFread(in_fp[i],in_rcd[i]);
      else status=GridFread(in_fp[i],in_rcd[i]);
      if (status !=-1) {
        read_flg[i]=0;
        dflg[i]=1;
      } else {
        dflg[i]=0;
        fclose(in_fp[i]);
        in_fp[i]=NULL;
      }
    }
    if (in_fp[i] !=NULL) c++;
  }
  return c;
}

int get_length() { 
  int smin=3600*24;
  int scan;
  int i;
  for (i=0;i<fnum;i++) {
    if (dflg[i] !=0) {
       scan=in_rcd[i]->ed_time-in_rcd[i]->st_time;
       if (scan<smin) smin=scan;
    }
  }
  return smin;
}
  

int main(int argc,char *argv[]) {

  int old=0;

  int arg=0;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;
  unsigned char vb=0;
  unsigned char replace=0;
 
  int record=0;
  int period=0;
  int opfp=0;

  int pmin=365*24*3600;
  int fmax=0;

  int i,c=0;
 
  rcd=GridMake(); 
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"-version",'x',&version);

  OptionAdd(&opt,"old",'x',&old);

  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"r",'x',&replace);

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


  if (argc-arg<2) {
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }
  fnum=argc-arg;
 
  for (i=0;i<fnum;i++) {
    in_rcd[i]=GridMake();
    dflg[i]=0;
    in_fp[i]=fopen(argv[arg+i],"r");
    if (in_fp[i]==NULL) fprintf(stderr,"File %s Not Found.\n",fname[i]);
    else {
      read_flg[i]=1;
      c++;
    }
  }
  if (c==0) {
    fprintf(stderr,"No files to be processed.\n");
    exit(-1);
  }
  fmax=c;
  if (vb) {
    fprintf(stderr,"Processing %d files\n",fmax);
  }

  while ((opfp=read_set(old)) !=0)  {

    if (record==0) {
      rcd->st_time=1e31;
      c=0;
      for (i=0;i<fnum;i++) {
        if (dflg[i] !=0) {
          if (in_rcd[i]->st_time<rcd->st_time) rcd->st_time=in_rcd[i]->st_time;
          c++;
        }
      }
    } 
    period=get_length(); 
    if (period<pmin) pmin=period;
    rcd->ed_time=rcd->st_time+period;

    /* load up the results here */

    make_grid(rcd,in_rcd,fnum,replace,dflg);

    if (vb) {
      int syr,smo,sdy,shr,smt,ssc,eyr,emo,edy,ehr,emt,esc;
      double sec;
      TimeEpochToYMDHMS(rcd->st_time,&syr,&smo,&sdy,&shr,&smt,&sec);
      ssc=sec;
      TimeEpochToYMDHMS(rcd->ed_time,&eyr,&emo,&edy,&ehr,&emt,&sec);
      esc=sec;

      fprintf(stderr,"%d-%d-%d %d:%d:%d %d:%d:%d pnts=%d files=%d\n",
              syr,smo,sdy,shr,smt,ssc,ehr,emt,esc,rcd->vcnum,rcd->stnum);
    }
    if (old) OldGridFwrite(stdout,rcd);
    else GridFwrite(stdout,rcd);
   
    /* get next record */
    rcd->st_time=rcd->ed_time;
    
    /* discard old records */
    for (i=0;i<fnum;i++) if (in_fp[i] !=NULL) 
      if (in_rcd[i]->ed_time <= rcd->st_time) read_flg[i]=1;
    record++;
  }

  return 0;
}

