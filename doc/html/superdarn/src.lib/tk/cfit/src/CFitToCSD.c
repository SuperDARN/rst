/* CFitToCSD.c
   ===========
   Author: R.J.Barnes */


#include <stdio.h>
#include <stdlib.h>

#include "rtypes.h"
#include "rtime.h"
#include "limit.h"
#include "cfitdata.h"
#include "cfitread.h"
#include "csddata.h"
#include "cfitcsd.h"
#include "csdwrite.h"

int main(int argc,char *argv[]) {
 
  struct CFitfp *fp=NULL;
  struct CFitdata cfit;
  struct CSDdata csd;

  int store=0x0f;
  double minpwr=3.0;
  
  double pmax=30.0,vmax=1500.0,wmax=500.0;

  int yr,mo,dy,hr,mt;
  double sc;
  int s=0;

  fp=CFitOpen(argv[1]);

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  while(CFitRead(fp,&cfit) !=-1) {

    TimeEpochToYMDHMS(cfit.time,&yr,&mo,&dy,&hr,&mt,&sc);
    fprintf(stderr,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d bmnum=%d scan=%d\n",
            yr,mo,dy,hr,mt,(int) sc,cfit.bmnum,cfit.scan);
        
    CFitToCSD(store,minpwr,pmax,vmax,wmax,&cfit,&csd);
    
    if (s==0) CSDHeaderFwrite(stdout,&csd);
    CSDFwrite(stdout,&csd);
    s++;

  }
  CFitClose(fp);

  return 0;
}
