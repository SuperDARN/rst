/* make_smr.c
   ==========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "option.h"
#include "rtime.h"
#include "rprm.h"
#include "fitdata.h"
#include "smrwrite.h"
#include "fitread.h"

#include "oldfitread.h"

#include "errstr.h"
#include "hlpstr.h"

struct RadarParm *prm;
struct FitData *fit;
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
  int i;
  int c=1;
  char *bstr=NULL;
  struct OldFitFp *fitfp=NULL;
  FILE *fp;
  double min_pwr=3.0;
  char bflg[16];
  int bnum=0,b;
  unsigned char vb=0;
  unsigned char help=0;
  unsigned char option=0;

  prm=RadarParmMake();
  fit=FitMake();

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new); 
  OptionAdd(&opt,"vb",'x',&vb);
  OptionAdd(&opt,"t",'d',&min_pwr);
  OptionAdd(&opt,"b",'t',&bstr); 

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



  if (min_pwr<0) min_pwr=0;
  if (min_pwr>30) min_pwr=30;

  for (i=0;i<16;i++) bflg[i]=0;
  bnum=0;

  if (bstr !=NULL) {
    char *tmp;
    tmp=strtok(bstr,",");
    do {
       b=atoi(tmp);
       if (bflg[b]==0) {
         bflg[b]=1;
         bnum++;
       }
     } while ((tmp=strtok(NULL,",")) !=NULL);
  }

  if ((old) && (arg==argc)) {
     OptionPrintInfo(stdout,errstr);
     exit(-1);
  }
    
  if (bnum==0) {
     bflg[8]=1;
     bnum=1;
  }
  if (old) {
    for (c=arg;c<argc;c++) {
      fitfp=OldFitOpen(argv[c],NULL); 
      if (vb) fprintf(stderr,"Opening file %s\n",argv[c]);
      if (fitfp==NULL) {
        if (vb) fprintf(stderr,"file %s not found\n",argv[c]);
        continue;
      }
 
      SmrHeaderFwrite(stdout,"1.00","make_smr","make_smr");
    

      while (OldFitRead(fitfp,prm,fit) !=-1) {

        if (bflg[prm->bmnum]==0) continue;
 
        if (vb) fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",
		      prm->time.dy,prm->time.mo,prm->time.dy,prm->time.hr,
                      prm->time.mt,prm->time.sc,prm->bmnum);
        SmrFwrite(stdout,prm,fit,min_pwr);            
      } 
      OldFitClose(fitfp);
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
    SmrHeaderFwrite(stdout,"1.00","make_smr","make_smr");
    while (FitFread(fp,prm,fit) !=-1) {
      if (bflg[prm->bmnum]==0) continue;
 
        if (vb) fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",
		      prm->time.dy,prm->time.mo,prm->time.dy,prm->time.hr,
                      prm->time.mt,prm->time.sc,prm->bmnum);
        SmrFwrite(stdout,prm,fit,min_pwr);            
      }
    if (fp !=stdin) fclose(fp);
  } 
  return 0;
} 






















