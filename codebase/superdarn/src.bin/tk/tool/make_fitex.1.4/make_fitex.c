/* make_fitex.c
   ===========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <zlib.h>

#include "rtypes.h"
#include "option.h"

#include "dmap.h"
#include "rprm.h"
#include "rawdata.h"
#include "fitblk.h"
#include "fitdata.h"
#include "radar.h"

#include "fitacfex.h"
#include "rawread.h"
#include "fitwrite.h"

#include "oldrawread.h"
#include "oldfitwrite.h"

#include "errstr.h"
#include "hlpstr.h"

struct RadarParm prm;
struct RawData raw;
struct FitData fit;

struct OptionData opt;

int main(int argc,char *argv[]) {

  /* File format transistion
   * ------------------------
   * 
   * When we switch to the new file format remove any reference
   * to "new". Change the command line option "new" to "old" and
   * remove "old=!new".
   */


  unsigned char old=0;
  unsigned char new=0;


  int status;
  int arg;

  unsigned char help=0;
  unsigned char option=0;

  unsigned char vb=0;

  FILE *fp=NULL;
  struct OldRawFp *rawfp=NULL;
  FILE *fitfp=NULL;
  FILE *inxfp=NULL;  
  int irec=1;
  int drec=2;
  int dnum=0;

  time_t ctime;
  int c,n;
  char command[128];
 

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"vb",'x',&vb);

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




  if ((old) && (argc-arg<2)) {
    OptionPrintInfo(stdout,hlpstr);
    exit(-1);
  }

  if (old) {
     rawfp=OldRawOpen(argv[arg],NULL);
     if (rawfp==NULL) {
       fprintf(stderr,"File not found.\n");
       exit(-1);
     }
     status=OldRawRead(rawfp,&prm,&raw);  
  } else { 
    if (arg==argc) fp=stdin;
    else fp=fopen(argv[arg],"r");

    if (fp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(-1);
    }
    status=RawFread(fp,&prm,&raw);
  }

  command[0]=0;
  n=0;
  for (c=0;c<argc;c++) {
    n+=strlen(argv[c])+1;
    if (n>127) break;
    if (c !=0) strcat(command," ");
    strcat(command,argv[c]);
  }



  if (vb) 
      fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",prm.time.yr,prm.time.mo,
	     prm.time.dy,prm.time.hr,prm.time.mt,prm.time.sc,prm.bmnum);



  FitACFex(&prm,&raw,&fit);
    
  if (old) {
    char vstr[256];
    fitfp=fopen(argv[arg+1],"w");
    if (fitfp==NULL) {
      fprintf(stderr,"Could not create fit file.\n");
      exit(-1);
    } 
    if (argc-arg>2) {
      inxfp=fopen(argv[arg+2],"w");
      if (inxfp==NULL) {
        fprintf(stderr,"Could not create inx file.\n");
        exit(-1);
      }
    }
    sprintf(vstr,"%d.%d",fit.revision.major,fit.revision.minor);
    OldFitHeaderFwrite(fitfp,"make_fit","fitacf",vstr);
    if (inxfp !=NULL) OldFitInxHeaderFwrite(inxfp,&prm);
  }


  
  do {


    ctime = time((time_t) 0);
    strcpy(prm.origin.command,command);
    strcpy(prm.origin.time,asctime(gmtime(&ctime)));
    prm.origin.time[24]=0;
  
    if (old) {
       dnum=OldFitFwrite(fitfp,&prm,&fit,NULL);
       if (inxfp !=NULL) OldFitInxFwrite(inxfp,drec,dnum,&prm);
       drec+=dnum;
       irec++;
    } else status=FitFwrite(stdout,&prm,&fit);
    
    if (old) status=OldRawRead(rawfp,&prm,&raw);
    else status=RawFread(fp,&prm,&raw);

     if (vb) 
      fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",prm.time.yr,prm.time.mo,
	     prm.time.dy,prm.time.hr,prm.time.mt,prm.time.sc,prm.bmnum);

    FitACFex(&prm,&raw,&fit);
    
  
  } while (status==0);
  
  if (old) OldRawClose(rawfp);
  return 0;
}













