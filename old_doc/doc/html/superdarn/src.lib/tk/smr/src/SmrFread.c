/* SmrFread.c
   ==========
   Author: R.J.Barnes */


#include <stdio.h>
#include <stdlib.h>

#include "rtypes.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "smrread.h"

int main(int argc,char *argv[]) {
 
  FILE *fp;
  int bmnum=8;

  struct RadarParm prm;
  struct FitData fit;


  fp=fopen(argv[1],"r");

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  while(SmrFread(fp,&prm,&fit,bmnum) !=-1) {
    fprintf(stdout,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
             prm.time.yr,prm.time.mo,prm.time.dy,
             prm.time.hr,prm.time.mt,prm.time.sc);


  }
 
  fclose(fp);


  return 0;
}
