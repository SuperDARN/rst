/* rawstat.c
   =========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

/*Generates statistics from raw and rawacf files.*/

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
#include "dmap.h"
#include "option.h"
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


  int arg;
  unsigned char help=0;
  unsigned char option=0;

  
  struct OldRawFp *rawfp=NULL;
  FILE *fp=NULL;
  

  int yr,mo,dy,hr,mt;
  double sc,st_time,ed_time;
  int bmcnt;
  int stid;

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

    rawfp=OldRawOpen(argv[arg],NULL); 
    if (rawfp==NULL) {
      fprintf(stderr,"file %s not found\n",argv[arg]);
      exit(2);
    }
    bmcnt=0;
    st_time=-1;
    ed_time=-1;
    stid=0;
    while (OldRawRead(rawfp,prm,raw) !=-1) {
      ed_time=TimeYMDHMSToEpoch(prm->time.yr,prm->time.mo,prm->time.dy,
             prm->time.hr,prm->time.mt,prm->time.sc);


      if (bmcnt==0) {
        st_time=ed_time;
        stid=prm->stid;
      }
      bmcnt++;
    }
    if (bmcnt>0) {

     fprintf(stdout,"%s:",argv[arg]);
      TimeEpochToYMDHMS(st_time,&yr,&mo,&dy,&hr,&mt,&sc);
      fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d",yr,mo,dy,hr,mt,(int) sc);
      TimeEpochToYMDHMS(ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
      fprintf(stdout,":");
      fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d ",yr,mo,dy,hr,mt,
                        (int) sc);
      fprintf(stdout,"[%.2d] (%d)\n",stid,bmcnt);
    } else exit(1);
    OldRawClose(rawfp);
  } else {
    fp=fopen(argv[arg],"r");
    if (fp==NULL) {
      fprintf(stderr,"Could not open file.\n");
      exit(1);
    }
    bmcnt=0;
    st_time=-1;
    ed_time=-1;
    stid=0;
    while (RawFread(fp,prm,raw) !=-1) {
      ed_time=TimeYMDHMSToEpoch(prm->time.yr,prm->time.mo,prm->time.dy,
               prm->time.hr,prm->time.mt,prm->time.sc);

      if (bmcnt==0) {
        st_time=ed_time;
        stid=prm->stid;
      }
      bmcnt++;
    }

    if (bmcnt>0) {
      fprintf(stdout,"%s:",argv[arg]);
      TimeEpochToYMDHMS(st_time,&yr,&mo,&dy,&hr,&mt,&sc);
      fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d",yr,mo,dy,hr,mt,(int) sc);
      TimeEpochToYMDHMS(ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
      fprintf(stdout,":");
      fprintf(stdout,"%.4d %.2d %.2d %.2d %.2d %.2d ",yr,mo,dy,hr,mt,
                        (int) sc);
      fprintf(stdout,"[%.2d] (%d)\n",stid,bmcnt);
    } else exit(1);
    fclose(fp);
  }
  exit(0);
  return 0;
} 






















