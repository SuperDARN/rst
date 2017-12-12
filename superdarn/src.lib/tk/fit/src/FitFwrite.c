/* FitFwrite.c
   ===========
   Author: R.J.Barnes */


#include <stdio.h>
#include <stdlib.h>

#include "rtypes.h"
#include "dmap.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "fitread.h"
#include "fitwrite.h"

int main(int argc,char *argv[]) {
 
  FILE *fp;

  struct RadarParm prm;
  struct FitData fit;

  fp=fopen(argv[1],"r");

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }


  while(FitFread(fp,&prm,&fit) !=-1) {
    fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
             prm.time.yr,prm.time.mo,prm.time.dy,
             prm.time.hr,prm.time.mt,prm.time.sc);
    prm.cp=1000;
    FitFwrite(stdout,&prm,&fit);
  }
 
  fclose(fp);


  return 0;
}
