/* downloadscan.c
   ==============
   Author: R.J.Barnes
*/

/*
  Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
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
    if (status == -1)
    {
        fprintf(stderr, "Error: perror occured in FitCnxRead\n");
    }
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
     if (status == -1)
     {
        fprintf(stderr, "Error: perror occured in FitCnxRead\n");
     }

      c++;
   }
   ptr->st_time=TimeYMDHMSToEpoch(prm->time.yr,prm->time.mo,prm->time.dy,
                                 prm->time.hr,prm->time.mt,prm->time.sc+
                                 prm->time.us/1.0e6);

   fprintf(stderr,"\n");
   if (c>0) return 0;
   return -1;
}
 










