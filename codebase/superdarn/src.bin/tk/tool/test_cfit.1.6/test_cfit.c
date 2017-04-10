/* test_cfit.c
   ===========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

/*Demonstration program that opens and reads fit files*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <zlib.h>
#include "rtypes.h"
#include "rtime.h"
#include "option.h"
#include "cfitdata.h"
#include "version.h"

#include "cfitread.h"

#include "errstr.h"
#include "hlpstr.h"

struct CFitdata *cfit;
struct OptionData opt;

int main (int argc,char *argv[]) {

  int arg;
  int yr,mo,dy,hr,mt;
  double sc;
  unsigned char help=0;
  unsigned char option=0;

  int i;
  struct CFitfp *cfitfp=NULL;
  int c;

  cfit=CFitMake();
  
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

 
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


  for (c=arg;c<argc;c++) {
    cfitfp=CFitOpen(argv[c]); 
    fprintf(stderr,"Opening file %s\n",argv[c]);
    if (cfitfp==NULL) {
      fprintf(stderr,"file %s not found\n",argv[c]);
      continue;
    }
    while (CFitRead(cfitfp,cfit) !=-1) {
      TimeEpochToYMDHMS(cfit->time,&yr,&mo,&dy,&hr,&mt,&sc);
      fprintf(stdout,
        "%d-%d-%d %d:%d:%d beam=%d bmazm=%f channel=%d cpid=%d scan=%d\n",
         yr,mo,dy,hr,mt,(int) sc,cfit->bmnum,cfit->bmazm,
                        cfit->channel,cfit->cp,cfit->scan);

      fprintf(stdout,"v:\n");
      for (i=0;i<cfit->num;i++) {
        fprintf(stdout,"%.2d\t%.4g",cfit->rng[i],cfit->data[i].v);
        if (((i+1) % 4)==0) fprintf(stdout,"\n");
        else fprintf(stdout,"\t");
      }
      fprintf(stdout,"\n");
    } 
    CFitClose(cfitfp);
  }

  return 0;
} 






















