/* SmrSeek.c
   =========
   Author: R.J.Barnes */


#include <stdio.h>
#include <stdlib.h>

#include "rtypes.h"
#include "rtime.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "smrread.h"
#include "smrseek.h"

int main(int argc,char *argv[]) {
 
  FILE *fp;
  int bmnum=8;
  int s;

  struct RadarParm prm;
  struct FitData fit;
  int yr=2004,mo=11,dy=10,hr=10,mt=0;
  double sc=0,atme;


  fp=fopen(argv[1],"r");

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }



  s=SmrSeek(fp,yr,mo,dy,hr,mt,(int) sc,&atme);
  if (s==-1) {
    fprintf(stderr,"file does not contain that interval.\n");
    exit(-1);
  }

  fprintf(stdout,"Requested:%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
            yr,mo,dy,hr,mt,(int) sc);

  TimeEpochToYMDHMS(atme,&yr,&mo,&dy,&hr,&mt,&sc);
  fprintf(stdout,"Found:%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
            yr,mo,dy,hr,mt,(int) sc);

  while(SmrFread(fp,&prm,&fit,bmnum) !=-1) {
    fprintf(stdout,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
             prm.time.yr,prm.time.mo,prm.time.dy,
             prm.time.hr,prm.time.mt,prm.time.sc);


  }
 
  fclose(fp);


  return 0;
}
