/* SmrRadarScan.c
   ==========
   Author: R.J.Barnes */


#include <stdio.h>
#include <stdlib.h>

#include "rtypes.h"
#include "rtime.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "scandata.h"
#include "smrscan.h"

struct RadarScan scn;

int main(int argc,char *argv[]) {
 
  FILE *fp;
  int bmnum=8;

  struct RadarParm prm;
  struct FitData fit;
  int state=0;

  int yr,mo,dy,hr,mt;
  double sc;

  fp=fopen(argv[1],"r");

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  while(SmrRadarScan(fp,&state,&scn,&prm,&fit,bmnum,0,0,0) !=-1) {
    TimeEpochToYMDHMS(scn.st_time,&yr,&mo,&dy,&hr,&mt,&sc);

    fprintf(stdout,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
            yr,mo,dy,hr,mt,(int) sc);

  }
 
  fclose(fp);


  return 0;
}
