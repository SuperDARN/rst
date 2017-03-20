/* make_rawinx.c
   =============
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
#include "rconvert.h"
#include "option.h"
#include "rtime.h"
#include "dmap.h"
#include "rprm.h"
#include "rawdata.h"

#include "rawread.h"
#include "rawwrite.h"

#include "errstr.h"
#include "hlpstr.h"




struct OptionData opt;

int main (int argc,char *argv[]) {


  /* File format transistion
   * ------------------------
   * 
   * When we switch to the new file format remove any reference
   * to "new". Change the command line option "new" to "old" and
   * remove "old=!new".
   */


  int old=0;
  int new=0;

  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char vb=0;



 
  FILE *fp;
  struct RadarParm *prm;
  struct RawData *raw;
  
  int sze=0;
  double tval;

  prm=RadarParmMake();
  raw=RawMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new);
  OptionAdd(&opt,"vb",'x',&vb);

  arg=OptionProcess(1,argc,argv,&opt,NULL);


  old=!new;

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }


  if (old) {
    fprintf(stderr,
    "Index files for old format raw files cannot be created.\n");
    exit(-1);
  }

  if (arg==argc) fp=stdin;
  else fp=fopen(argv[arg],"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }
    
  while (RawFread(fp,prm,raw) !=-1) {
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
    sze+=RawWrite(-1,prm,raw);
  }
  if (fp !=stdin) fclose(fp);
  return 0;
} 






















