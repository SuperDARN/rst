/* OldCnvMapFread.c
   ================
   Author: R.J.Barnes */


#include <stdio.h>
#include <stdlib.h>

#include "rtypes.h"
#include "rtime.h"
#include "rfile.h"
#include "limit.h"
#include "griddata.h"
#include "cnvmap.h"
#include "oldcnvmapread.h"

  struct CnvMapData map;
  struct GridData grd;


int main(int argc,char *argv[]) {
 
  FILE *fp;

  int yr,mo,dy,hr,mt;
  double sc;

  fp=fopen(argv[1],"r");

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  while(OldCnvMapFread(fp,&map,&grd) !=-1) {

    TimeEpochToYMDHMS(map.st_time,&yr,&mo,&dy,&hr,&mt,&sc);
    fprintf(stdout,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d pnts=%d\n",
            yr,mo,dy,hr,mt,(int) sc,grd.vcnum);


    
  }
 
  fclose(fp);


  return 0;
}
