/* make_iqinx.c
   ============
   Author: R.J.Barnes
*/

/*
   See license.txt
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
#include "option.h"
#include "rtime.h"
#include "dmap.h"
#include "rprm.h"
#include "iq.h"

#include "iqread.h"
#include "iqwrite.h"

#include "errstr.h"
#include "hlpstr.h"




struct OptionData opt;

int main (int argc,char *argv[]) {



  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char vb=0;

  FILE *fp;
  struct RadarParm *prm;
  struct IQ *iq;
  unsigned int *badtr=NULL;
  int16 *samples=NULL;  

  int sze=0;
  double tval;

  prm=RadarParmMake();
  iq=IQMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);
  OptionAdd(&opt,"vb",'x',&vb);

  arg=OptionProcess(1,argc,argv,&opt,NULL);

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  if (arg==argc) fp=stdin;
  else fp=fopen(argv[arg],"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }
    
  while (IQFread(fp,prm,iq,&badtr,&samples) !=-1) {
    if (vb) 
      fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",prm->time.yr,prm->time.mo,
	     prm->time.dy,prm->time.hr,prm->time.mt,prm->time.sc,prm->bmnum);

    tval=TimeYMDHMSToEpoch(prm->time.yr,
		           prm->time.mo,
                           prm->time.dy,
                           prm->time.hr,
		           prm->time.mt,
                           prm->time.sc+prm->time.us/1.0e6);
    ConvertFwriteDouble(stdout,tval);
    ConvertFwriteInt(stdout,sze);
    sze+=IQWrite(-1,prm,iq,badtr,samples);
  }
  if (fp !=stdin) fclose(fp);
  return 0;
} 






















