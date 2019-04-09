/* RadarScanReset.c
   ================
   Author: R.J.Barnes */


#include <stdio.h>
#include <stdlib.h>

#include "rtypes.h"
#include "rtime.h"
#include "dmap.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "fitread.h"
#include "scandata.h"
#include "fitscan.h"

struct RadarScan scn;


int main(int argc,char *argv[]) {
 
  FILE *fp=NULL;

  struct RadarParm prm;
  struct FitData fit;
  
  fp=fopen(argv[1],"r");

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }


  while(FitFread(fp,&prm,&fit) !=-1) {

    FitToRadarScan(&scn,&prm,&fit);
    if (prm.scan !=1) continue;
   
    fprintf(stdout,"%d\n",scn.num);
    RadarScanReset(&scn);

  }

  return 0;
}
