/* RawSeek.c
   =========
   Author: R.J.Barnes */


#include <stdio.h>
#include <stdlib.h>

#include "rtypes.h"
#include "rtime.h"
#include "dmap.h"
#include "limit.h"
#include "rprm.h"
#include "rawdata.h"
#include "rawread.h"
#include "rawindex.h"
#include "rawseek.h"

int main(int argc,char *argv[]) {
 
  FILE *fp;
  int fd=0;

  struct RadarParm prm;
  struct RawData raw;
  struct RawIndex *inx=NULL;



  int yr=2003,mo=5,dy=28,hr=12,mt=0;
  double sc=0,atme;

  int s;

  if (argc>2) {
     
    fp=fopen(argv[2],"r");

    if (fp==NULL) {
      fprintf(stderr,"Index not found.\n");
      exit(-1);
    }
    fd=fileno(fp);
    inx=RawIndexLoad(fd);
  
    fclose(fp);
    if (inx==NULL) {
        fprintf(stderr,"Error loading index.\n");
        exit(-1);
    }
  }

  fp=fopen(argv[1],"r");

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }
  fd=fileno(fp);


  s=RawSeek(fd,yr,mo,dy,hr,mt,(int) sc,&atme,inx);
  if (s==-1) {
    fprintf(stderr,"file does not contain that interval.\n");
    exit(-1);
  }

  fprintf(stdout,"Requested:%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
            yr,mo,dy,hr,mt,(int) sc);

  TimeEpochToYMDHMS(atme,&yr,&mo,&dy,&hr,&mt,&sc);
  fprintf(stdout,"Found:%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
            yr,mo,dy,hr,mt,(int) sc);
 

  while(RawRead(fd,&prm,&raw) !=-1) {
    fprintf(stdout,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
             prm.time.yr,prm.time.mo,prm.time.dy,
             prm.time.hr,prm.time.mt,prm.time.sc);
  }
 
  fclose(fp);


  return 0;
}
