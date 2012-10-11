/* OldFitOpen.c
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
#include "scandata.h"
#include "oldfitscan.h"


struct RadarScan scn;

int main(int argc,char *argv[]) {
 
 

  struct RadarParm prm;
  struct FitData fit;

  struct OldFitFp *fp;

  int state=0;

  int yr,mo,dy,hr,mt;
  double sc;

  if (argc==2)
    fp=OldFitOpen(argv[1],NULL);
  else
    fp=OldFitOpen(argv[1],argv[2]);


  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }


  while(OldFitReadRadarScan(fp,&state,&scn,&prm,&fit,0,0,0) !=-1) {
    TimeEpochToYMDHMS(scn.st_time,&yr,&mo,&dy,&hr,&mt,&sc);

    fprintf(stdout,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
            yr,mo,dy,hr,mt,(int) sc);

  }

 
 
  OldFitClose(fp);

  return 0;
}
