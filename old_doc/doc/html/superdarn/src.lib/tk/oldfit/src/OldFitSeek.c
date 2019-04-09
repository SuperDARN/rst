/* OldFitSeek.c
   ============
   Author: R.J.Barnes */


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "rtypes.h"
#include "rtime.h"
#include "dmap.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "oldfitread.h"

int main(int argc,char *argv[]) {
 
  struct RadarParm prm;
  struct FitData fit;

  struct OldFitFp *fp;

   int yr=2004,mo=11,dy=10,hr=12,mt=0;
  double sc=0,atme;

  int s;


  if (argc==2) fp=OldFitOpen(argv[1],NULL);
  else fp=OldFitOpen(argv[1],argv[2]);

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }


  s=OldFitSeek(fp,yr,mo,dy,hr,mt,(int) sc,&atme);
  if (s==-1) {
    fprintf(stderr,"file does not contain that interval.\n");
    exit(-1);
  }

  fprintf(stdout,"Requested:%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
            yr,mo,dy,hr,mt,(int) sc);

  TimeEpochToYMDHMS(atme,&yr,&mo,&dy,&hr,&mt,&sc);
  fprintf(stdout,"Found:%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
            yr,mo,dy,hr,mt,(int) sc);

  while(OldFitRead(fp,&prm,&fit) !=-1) {
    fprintf(stdout,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
             prm.time.yr,prm.time.mo,prm.time.dy,
             prm.time.hr,prm.time.mt,prm.time.sc);
  }
 
  OldFitClose(fp);

  return 0;
}
