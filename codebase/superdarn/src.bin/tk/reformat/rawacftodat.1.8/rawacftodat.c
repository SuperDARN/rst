/* rawacftodat.c
   ============= 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <zlib.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "dmap.h"
#include "rprm.h"
#include "rawdata.h"
#include "rawread.h"
#include "oldrawwrite.h"
#include "hlpstr.h"



#define ORIG_MAX_RANGE 75

struct OptionData opt;

int main(int argc,char *argv[]) {

  unsigned char help=0;
  unsigned char option=0;

  unsigned char vb=0;
  int arg=0;

  int s;
  int thr=-1;

  struct RadarParm *rprm;
  struct RawData *rawacf;
  FILE *fp;

  int rtab[ORIG_MAX_RANGE];
  float snr[ORIG_MAX_RANGE];
  int inx,l,step;
  float maxval;
  int cnt=0;

  int recnum=0;

  char vstring[256];
 
  rprm=RadarParmMake();
  rawacf=RawMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"t",'i',&thr);
 
  arg=OptionProcess(1,argc,argv,&opt,NULL); 
 
  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

 
  if (arg !=argc) { 
    fp=fopen(argv[arg],"r");
    if (fp==NULL) {
      fprintf(stderr,"Error opening file.\n");
      exit(-1);
    } 
  } else fp=stdin;

  
  s=RawFread(fp,rprm,rawacf);
  if (s==-1) {
    fprintf(stderr,"Error reading file.\n");
    exit(-1);
  }
  if (thr !=-1) rawacf->thr=thr;

  sprintf(vstring,"%d.%.3d",rawacf->revision.major,rawacf->revision.minor);
  if (OldRawHeaderFwrite(stdout,"rawwrite",vstring,rawacf->thr,            
			"example file") !=0) {
    fprintf(stderr,"Error writing header.\n");
    exit(-1);
  }

  do {
    recnum++;

    if (thr !=-1) rawacf->thr=thr;

    for (l=0;l<rprm->nrang;l++) {
      if (rprm->noise.search>0)
        snr[l]=rawacf->pwr0[l]/rprm->noise.search;
      else snr[l]=0;
    }

    if (rprm->nrang>ORIG_MAX_RANGE) {
      step=rprm->nrang/ORIG_MAX_RANGE;
      for (l=0;l<ORIG_MAX_RANGE;l++) {
        maxval=0.0;
        inx=l*step;
        for (s=0;s<step;s++) {
          if (snr[l*step+s]>maxval) {
            maxval=snr[l*step+s];
            inx=l*step+s;
          }
        }
        rtab[l]=inx;
      }
      rprm->rsep=rprm->rsep*step;
      s=OldRawFwrite(stdout,"rawwrite",rprm,rawacf,recnum,rtab);  
    } else  s=OldRawFwrite(stdout,"rawwrite",rprm,rawacf,recnum,NULL);
    if (s !=0) {
      cnt=-1;
      break;
    }

     if (vb) fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",rprm->time.yr,
                    rprm->time.mo,rprm->time.dy,rprm->time.hr,rprm->time.mt,
                    rprm->time.sc);

     cnt++;
  } while ((s=RawFread(fp,rprm,rawacf)) !=-1);

  if (cnt==-1) exit(EXIT_FAILURE);
  exit(EXIT_SUCCESS);
  return 0;
}



