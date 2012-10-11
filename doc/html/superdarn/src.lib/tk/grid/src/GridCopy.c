/* GridCopy.c
   ==========
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
struct GridData ogrd;

int main(int argc,char *argv[]) {
 
  FILE *fp;

  int num=0;

  int yr,mo,dy,hr,mt;
  double sc;

  fp=fopen(argv[1],"r");

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  while(GridFread(fp,&grd) !=-1) {

    if (num>0) {
      TimeEpochToYMDHMS(grd.st_time,&yr,&mo,&dy,&hr,&mt,&sc);
      fprintf(stdout,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d",
            yr,mo,dy,hr,mt,(int) sc);
      fprintf(stdout,"Difference in points:%d\n",grd.vcnum-ogrd.vcnum);

   }   
   GridCopy(&ogrd,&grd);
   num++;  

 
  }
 
  fclose(fp);


  return 0;
}
