/* make_cfiinx.c
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
#include "dmap.h"
#include "rconvert.h"
#include "rtime.h"
#include "option.h"
#include "cfitdata.h"
#include "cfitread.h"

#include "errstr.h"
#include "hlpstr.h"




struct OptionData opt;

int main (int argc,char *argv[]) {



  int arg;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char vb=0;

  int yr,mo,dy,hr,mt;
  double sc;

  struct CFitfp *cfitfp;
  struct CFitdata *cfit=NULL;


  int ptr=0;

  cfit=CFitMake();

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






















