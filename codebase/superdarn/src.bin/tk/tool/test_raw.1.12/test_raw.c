/* test_raw.c
   ==========
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
#include <sys/stat.h>
#include <zlib.h>
#include "rtypes.h"
#include "option.h"
#include "dmap.h"
#include "rprm.h"
#include "rawdata.h"
#include "version.h"

#include "rawread.h"
#include "oldrawread.h"

#include "errstr.h"
#include "hlpstr.h"



struct RadarParm *prm;
struct RawData *raw;
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

  int c=1,i;
  int arg=0;
  struct OldRawFp *rawfp=NULL;
  FILE *fp;
  unsigned char help=0;
  unsigned char option=0;


  prm=RadarParmMake();
  raw=RawMake();
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new); 

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



  if ((old) && (arg==argc)) {
    OptionPrintInfo(stdout,errstr);
    exit(-1);
  }

       
  if (old) {
    for (c=arg;c<argc;c++) {
      rawfp=OldRawOpen(argv[c],NULL);
      if (rawfp==NULL) {
        fprintf(stderr,"Could not open file %s.\n",argv[c]);
        continue;
      }
      while (OldRawRead(rawfp,prm,raw) !=-1) {
       fprintf(stdout,
          "%d-%d-%d %d:%d:%d beam=%d  bmazm=%f channel=%d cpid=%d scan=%d\n",
           prm->time.yr,prm->time.mo,prm->time.dy,
           prm->time.hr,prm->time.mt,prm->time.sc,
           prm->bmnum,prm->bmazm,prm->channel,prm->cp,prm->scan);
       fprintf(stdout,"origin.code=%d\norigin.time=%s\norigin.command=%s\n",
                 prm->origin.code,prm->origin.time,prm->origin.command);

       fprintf(stdout,"pwr 0:\n");
       for (i=0;i<prm->nrang;i++) {
         fprintf(stdout,"%g",raw->pwr0[i]);
         if ((i % 8)==0) fprintf(stdout,"\n");
         else fprintf(stdout,"\t");
        
       }
       fprintf(stdout,"\n");
       
    
      }    
      OldRawClose(rawfp);
    }
  } else {
    if (arg==argc) fp=stdin;
    else {
      fp=fopen(argv[arg],"r");
      if (fp==NULL) {
        fprintf(stderr,"Could not open file.\n");
        exit(-1);
      }
    }
    while (RawFread(fp,prm,raw) !=-1) {
       fprintf(stdout,
          "%d-%d-%d %d:%d:%d beam=%d  bmazm=%f channel=%d cpid=%d scan=%d\n",
           prm->time.yr,prm->time.mo,prm->time.dy,
           prm->time.hr,prm->time.mt,prm->time.sc,
           prm->bmnum,prm->bmazm,prm->channel,prm->cp,prm->scan);
       fprintf(stdout,"origin.code=%d\norigin.time=%s\norigin.command=%s\n",
                 prm->origin.code,prm->origin.time,prm->origin.command);

      fprintf(stdout,"pwr 0:\n");
      for (i=0;i<prm->nrang;i++) {
        fprintf(stdout,"%g",raw->pwr0[i]);
        if ((i % 8)==0) fprintf(stdout,"\n");
        else fprintf(stdout,"\t");
      }
      fprintf(stdout,"\n");   
    }
    if (fp !=stdin) fclose(fp);
  }    
  
  return 0;

}






















