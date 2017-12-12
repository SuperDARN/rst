/* OldFitInxClose.c
   ================
   Author: R.J.Barnes */


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "rtypes.h"
#include "dmap.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "oldfitread.h"
#include "oldfitwrite.h"

struct RadarParm pbuf[2];
struct FitData fbuf[2];

struct FitData *fptr;
struct RadarParm *pptr;

int fnum=0;

int main(int argc,char *argv[]) {
 
  int irec=1;
  int drec=2,dnum;

  struct OldFitFp *fitfp;
  FILE *inxfp=0;

  int inxfd;


  fitfp=OldFitOpen(argv[1],NULL);

  if (fitfp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }


  inxfp=fopen(argv[2],"w");
  if (inxfp==NULL) {
    fprintf(stderr,"could not create index file.\n");
    exit(-1);  
  }
  inxfd=fileno(inxfp);
 
  while(OldFitRead(fitfp,&pbuf[fnum],&fbuf[fnum]) !=-1) {
  


    fptr=&fbuf[fnum];
    pptr=&pbuf[fnum];
    fnum=(fnum+1) % 2;
    if (irec==1) OldFitInxHeaderWrite(inxfd,pptr);
    dnum=OldFitFwrite(NULL,pptr,fptr,NULL);
    OldFitInxWrite(inxfd,drec,dnum,pptr);
    fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
            pptr->time.yr,
            pptr->time.mo,pptr->time.dy,pptr->time.hr,pptr->time.mt,
            pptr->time.sc);


    drec+=dnum;
    irec++;
  }
 
  OldFitClose(fitfp);
  fclose(inxfp);
  inxfp=fopen(argv[2],"r+");
  inxfd=fileno(inxfp);
  OldFitInxClose(inxfd,pptr,irec-1);
  fclose(inxfp);

  return 0;
}
