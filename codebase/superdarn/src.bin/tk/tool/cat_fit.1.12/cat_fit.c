/* cat_fit.c
   =========
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
#include "option.h"
#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"

#include "oldfitread.h"
#include "oldfitwrite.h"

#include "errstr.h"
#include "hlpstr.h"

struct RadarParm *pbuf[2];
struct FitData *fbuf[2];

struct RadarParm *prm=NULL;
struct FitData *fit=NULL;

int fnum=0;

struct OptionData opt;

int main (int argc,char *argv[]) {
  int arg;
  unsigned char help=0;
  unsigned char option=0;

  unsigned char new=0;

  int i;
  
  struct OldFitFp *fp=NULL;
  FILE *fitfp=NULL;
  FILE *inxfp=NULL;
  int irec=1;
  int drec=2;
  
  unsigned char inx=0; 
  int dnum=0;

  pbuf[0]=RadarParmMake();
  pbuf[1]=RadarParmMake();
  fbuf[0]=FitMake();
  fbuf[1]=FitMake();
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);

  OptionAdd(&opt,"new",'x',&new);
  OptionAdd(&opt,"i",'x',&inx);

  arg=OptionProcess(1,argc,argv,&opt,NULL);

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }

  if (new==1) {
    fprintf(stderr,
	    "New format files can be concatenated using the cat command\n");
    exit(-1);
  }

  if (argc-arg<2+inx) {
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }
      	  
  fitfp=fopen(argv[argc-1-inx],"w");
  if (fitfp==NULL) {
    fprintf(stderr,"Could not create fit file.\n");
    exit(-1);
  }
  OldFitHeaderFwrite(fitfp,"cat_fit","fitacf","4.0");

  if (inx !=0) {
    inxfp=fopen(argv[argc-1],"w");
    if (inxfp==NULL) fprintf(stderr,"Could not create index file.\n");
  }
 
  for (i=arg;i<argc-1-inx;i++) {
    fp=OldFitOpen(argv[i],NULL); 
    if (fp==NULL) {
      fprintf(stderr,"file %s not found\n",argv[i]);
      continue;
    }
    while (OldFitRead(fp,pbuf[fnum],fbuf[fnum]) !=-1) {
      prm=pbuf[fnum];
      fit=fbuf[fnum];
      fnum=(fnum+1) % 2;
      if ((inxfp !=NULL) && (irec==1)) OldFitInxHeaderFwrite(inxfp,prm);
      dnum=OldFitFwrite(fitfp,prm,fit,NULL);
      if (inxfp !=NULL) OldFitInxFwrite(inxfp,drec,dnum,prm);
      drec+=dnum;
      irec++;
    } 
    OldFitClose(fp);
  }
  if (inxfp !=NULL) {
    fclose(inxfp);
    inxfp=fopen(argv[argc-1],"r+");
    OldFitInxFclose(inxfp,prm,irec-1);
    fclose(inxfp);
  }
  fclose(fitfp);
  return 0;
} 






















