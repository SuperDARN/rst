/* SmrFwrite.c
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
#include "smrwrite.h"

int main(int argc,char *argv[]) {
 
  FILE *fp;
  int bmnum=8;
  int minpwr=3;

  struct RadarParm prm;
  struct FitData fit;


  fp=fopen(argv[1],"r");

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  SmrHeaderFwrite(stdout,"1.00","Demonstration code","SmrFwrite");
 

  while(FitFread(fp,&prm,&fit) !=-1) {
    if (prm.bmnum !=bmnum) continue;
    fprintf(stdout,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
             prm.time.yr,prm.time.mo,prm.time.dy,
             prm.time.hr,prm.time.mt,prm.time.sc);
 
    SmrFwrite(stdout,&prm,&fit,minpwr);

  }
 
  fclose(fp);


  return 0;
}
