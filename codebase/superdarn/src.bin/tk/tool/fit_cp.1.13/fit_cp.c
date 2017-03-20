/* fit_cp.c
   ========
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
#include "option.h"
#include "rprm.h"
#include "fitdata.h"

#include "fitread.h"
#include "oldfitread.h"

#include "hlpstr.h"
#include "errstr.h"

int cpid[256];
int cpcnt=0;

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
  unsigned char help=0;
  unsigned char option=0;

  struct OldFitFp *fitfp=NULL;
  FILE *fp=NULL;
  int c,i;

  prm=RadarParmMake();
  fit=FitMake();
  
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new); 

  arg=OptionProcess(1,argc,argv,&opt,NULL);

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  old=!new;


  if ((old) && (arg==argc)) {
    OptionPrintInfo(stdout,errstr);
    exit(-1);
  }


  if (old) {
    for (c=arg;c<argc;c++) {
      fitfp=OldFitOpen(argv[c],NULL); 
      fprintf(stderr,"Opening file %s\n",argv[c]);
      if (fitfp==NULL) {
        fprintf(stderr,"file %s not found\n",argv[c]);
        continue;
      }
      while (OldFitRead(fitfp,prm,fit) !=-1) {
        for (i=0;i<cpcnt;i++) if (cpid[i]==prm->cp) break;
        if (i>=cpcnt) {
          cpid[cpcnt]=prm->cp;
          cpcnt++;
        } 
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
    while (FitFread(fp,prm,fit) !=-1) {
      for (i=0;i<cpcnt;i++) if (cpid[i]==prm->cp) break;
        if (i>=cpcnt) {
          cpid[cpcnt]=prm->cp;
          cpcnt++;
        } 
      }
    if (fp !=stdin) fclose(fp);
  } 
  fprintf(stdout,"%d\n",cpcnt);
  for (i=0;i<cpcnt;i++) fprintf(stdout,"%.2d:%d\n",i,cpid[i]);
  exit(cpcnt);

  return cpcnt;
} 






















