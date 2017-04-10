/* downloadscan.c
   ==============
   Author: R.J.Barnes
*/

/*
   See license.txt
*/



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <zlib.h>
#include "rtypes.h"
#include "rtime.h"
#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"
#include "connex.h"
#include "fitcnx.h"
#include "scandata.h"
#include "fitscan.h"



int downloadscan(int sock,struct RadarParm *prm,struct FitData *fit,
				struct RadarScan *ptr) {


   int c=0;
   int flag,status=0;

   ptr->st_time=-1;
   ptr->ed_time=-1;
  
    
   /* clear out the arrays */

   RadarScanReset(ptr);

  while (prm->scan !=1) {  
    status=FitCnxRead(1,&sock,prm,fit,&flag,NULL);
    fprintf(stderr,"+%d (%d)+",prm->bmnum,prm->scan); 
    fflush(stderr);
  } 
  fprintf(stderr,"\n");

  ptr->stid=prm->stid;
  ptr->st_time=TimeYMDHMSToEpoch(prm->time.yr,prm->time.mo,prm->time.dy,
                                 prm->time.hr,prm->time.mt,prm->time.sc+
                                 prm->time.us/1.0e6);


  prm->scan=0;

   while (prm->scan !=1)  {
     fprintf(stderr,"%.2d.",prm->bmnum);
     fflush(stderr);
     if (prm->scan==0) FitToRadarScan(ptr,prm,fit);
      status=FitCnxRead(1,&sock,prm,fit,&flag,NULL);
     c++;
   }
   ptr->st_time=TimeYMDHMSToEpoch(prm->time.yr,prm->time.mo,prm->time.dy,
                                 prm->time.hr,prm->time.mt,prm->time.sc+
                                 prm->time.us/1.0e6);

   fprintf(stderr,"\n");
   if (c>0) return 0;
   return -1;
}
 










