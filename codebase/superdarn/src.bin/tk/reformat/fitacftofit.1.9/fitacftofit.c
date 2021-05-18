/* fitacftofit.c
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
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "option.h"
#include "rtime.h"
#include "rprm.h"
#include "fitdata.h"
#include "fitread.h"
#include "oldfitwrite.h"

#include "hlpstr.h"



struct OptionData opt;

#define ORIG_MAX_RANGE 75

int rst_opterr(char *txt) {
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: fitacftofit --help\n");
  return(-1);
}

int main(int argc,char *argv[]) {

  unsigned char help=0;
  unsigned char option=0;
  unsigned char version=0;

  unsigned char vb=0;
  int arg=0;
  int s;
  struct RadarParm *rprm;
  struct FitData *fitacf;
  int dnum;
  FILE *fp; 
  char vstr[256];

  int rtab[ORIG_MAX_RANGE];
  float snr[ORIG_MAX_RANGE];
  int inx,l,step;
  float maxval;

  int cnt=0;

  rprm=RadarParmMake();
  fitacf=FitMake();

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

  if (arg !=argc) { 
    fp=fopen(argv[arg],"r");
    if (fp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(-1);
    }  
  } else fp=stdin; 

  s=FitFread(fp,rprm,fitacf);
  if (s==-1) {
    fprintf(stderr,"Error reading file.\n");
    exit(-1);
  }

  sprintf(vstr,"%d.%.3d",fitacf->revision.major,fitacf->revision.minor);

  if (OldFitHeaderWrite(fileno(stdout),"fitacftofit","fitwrite",vstr) ==-1) {
    fprintf(stderr,"Could not write header.\n");
    exit(-1);
  }

  do {


    for (l=0;l<rprm->nrang;l++) {
      if (rprm->noise.search>0)
        snr[l]=fitacf->rng[l].p_0/rprm->noise.search;
      else snr[l]=0;
    }
  

    if (rprm->nrang>ORIG_MAX_RANGE) {
      step=rprm->nrang/ORIG_MAX_RANGE;
      for (l=0;l<ORIG_MAX_RANGE;l++) {
        maxval=0.0;
        inx=l*step;
        for (s=0;s<step;s++) {
	  /*
                    if (fitacf.rng[l*step+s].qflg==0) continue;
                    if (fitacf.rng[l*step+s].gsct !=0) continue;
	  */
	  if (snr[l*step+s]>maxval) {
	    maxval=snr[l*step+s];
	    inx=l*step+s;
	  }
        }
        rtab[l]=inx;
      }
      rprm->rsep=rprm->rsep*step;
      dnum=OldFitFwrite(stdout,rprm,
			fitacf,rtab);
    } else dnum=OldFitFwrite(stdout,rprm,
			     fitacf,NULL);

    if (dnum==-1) {
      cnt=-1;
      break;
    } 

    if (vb) fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",rprm->time.yr,
                    rprm->time.mo,rprm->time.dy,rprm->time.hr,rprm->time.mt,
                    rprm->time.sc);

    cnt++;
  } while ((s=FitFread(fp,rprm,fitacf)) !=-1);

  if (cnt==-1) exit(EXIT_FAILURE);
  else exit(EXIT_SUCCESS);

  return 0;
}



