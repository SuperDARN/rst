/* CFitRead.c
   ===========
   Author: R.J.Barnes */


#include <stdio.h>
#include <stdlib.h>

#include "rtypes.h"
#include "rtime.h"
#include "limit.h"
#include "cfitdata.h"
#include "cfitread.h"



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
    
    TimeEpochToYMDHMS(cfit.time,&yr,&mo,&dy,&hr,&mt,&sc);
    fprintf(stdout,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d bmnum=%d pnts=%d\n",
            yr,mo,dy,hr,mt,(int) sc,cfit.bmnum,cfit.num);


   


  }
  CFitClose(fp);

  return 0;
}
