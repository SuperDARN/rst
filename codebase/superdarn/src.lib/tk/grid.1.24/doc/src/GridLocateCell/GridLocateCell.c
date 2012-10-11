/* GridLocateCell.c
   ================
   Author: R.J.Barnes */


#include <stdio.h>
#include <stdlib.h>

#include "rtypes.h"
#include "rtime.h"
#include "dmap.h"
#include "limit.h"
#include "griddata.h"
#include "gridread.h"

struct GridData grd;

int main(int argc,char *argv[]) {
 
  FILE *fp;

  int yr,mo,dy,hr,mt;
  double sc;

  int index=32031,ival;

  fp=fopen(argv[1],"r");

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  while(GridFread(fp,&grd) !=-1) {

    ival=GridLocateCell(grd.vcnum,grd.data,index);
    TimeEpochToYMDHMS(grd.st_time,&yr,&mo,&dy,&hr,&mt,&sc);
    fprintf(stdout,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d pnts=%d\n",
            yr,mo,dy,hr,mt,(int) sc,ival);

  }
 
  fclose(fp);


  return 0;
}
