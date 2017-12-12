/* RadarScanResetBeam.c
   ====================
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

  int bmnum=2;
  int bmtab[2]={5,8};

  struct RadarParm prm;
  struct FitData fit;
  int n;
  
  fp=fopen(argv[1],"r");

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }


  while(FitFread(fp,&prm,&fit) !=-1) {

    FitToRadarScan(&scn,&prm,&fit);
    if (prm.scan !=1) continue;
   
    fprintf(stdout,"Input:\n");
    for (n=0;n<scn.num;n++) fprintf(stdout,"%d\n",scn.bm[n].bm);

    RadarScanResetBeam(&scn,bmnum,bmtab);
 
    fprintf(stdout,"Output:\n");
    for (n=0;n<scn.num;n++) fprintf(stdout,"%d\n",scn.bm[n].bm);

   
    RadarScanReset(&scn);

  }

  return 0;
}
