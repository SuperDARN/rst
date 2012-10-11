/* FitToCSD.c
   ==========
   Author: R.J.Barnes */


#include <stdio.h>
#include <stdlib.h>

#include "rtypes.h"
#include "dmap.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "fitread.h"
#include "csddata.h"
#include "fitcsd.h"
#include "csdwrite.h"

int main(int argc,char *argv[]) {
 
  FILE *fp;

  struct RadarParm prm;
  struct FitData fit;
  struct CSDdata csd;
  double minpwr=3.0;
  int store=0x0f;
  double pmax=30.0;
  double vmax=1500.0;
  double wmax=500.0;

  int s=0;

  fp=fopen(argv[1],"r");

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }


  while(FitFread(fp,&prm,&fit) !=-1) {
    fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
             prm.time.yr,prm.time.mo,prm.time.dy,
             prm.time.hr,prm.time.mt,prm.time.sc);
    
    FitToCSD(store,minpwr,pmax,vmax,wmax,&prm,&fit,&csd);
    if (s==0)  CSDHeaderFwrite(stdout,&csd);
    CSDFwrite(stdout,&csd);
    s++;

  }
 
  fclose(fp);


  return 0;
}
