/* CFitToRadarScan.c
   =================
   Author: R.J.Barnes */


#include <stdio.h>
#include <stdlib.h>

#include "rtypes.h"
#include "rtime.h"
#include "limit.h"
#include "cfitdata.h"
#include "cfitread.h"
#include "scandata.h"
#include "cfitscan.h"

struct RadarScan scn;


int main(int argc,char *argv[]) {
 
  struct CFitfp *fp=NULL;
  struct CFitdata cfit;
 
  int yr,mo,dy,hr,mt;
  double sc;

  fp=CFitOpen(argv[1]);

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  while(CFitRead(fp,&cfit) !=-1) {
    CFitToRadarScan(&scn,&cfit);
    if (cfit.scan !=1) continue;
    TimeEpochToYMDHMS(scn.st_time,&yr,&mo,&dy,&hr,&mt,&sc);

    fprintf(stdout,"%d\n",scn.num);
    RadarScanReset(&scn);

  }

  CFitClose(fp);

  return 0;
}
