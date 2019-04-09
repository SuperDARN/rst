/* FitDecode.c
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

int main(int argc,char *argv[]) {
 
  FILE *fp;

  struct RadarParm prm;
  struct FitData fit;
  struct DataMap *dmap=NULL;
  int s;

  fp=fopen(argv[1],"r");

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  while((dmap=DataMapFread(fp)) !=NULL) {

    s=FitDecode(dmap,&prm,&fit);
    if (s !=0) break;

    fprintf(stdout,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
             prm.time.yr,prm.time.mo,prm.time.dy,
             prm.time.hr,prm.time.mt,prm.time.sc);
    DataMapFree(dmap);
  }
 
  fclose(fp);


  return 0;
}
