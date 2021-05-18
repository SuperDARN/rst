/* smrseek.c
   =========
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
#include <sys/types.h>
#include <zlib.h>
#include "rtypes.h"
#include "rtime.h"
#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"

#include "smrread.h"



int SmrSeek(FILE *fp,
	     int yr,int mo,int dy,int hr,int mt,int sc,double *atme) {
  struct RadarParm prm;
  struct FitData fit;
  int status=0,sjmp=0;
  int ctime=0;
  int tval;
  tval=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,sc);
  fseek(fp,0,SEEK_SET);
 
  do {
    sjmp=status;
    status=SmrFread(fp,&prm,&fit,0);
    if (status !=-1) ctime=TimeYMDHMSToYrsec(prm.time.yr,
				    prm.time.mo,
				    prm.time.dy,
				    prm.time.hr,
				    prm.time.mt,
				    prm.time.sc+prm.time.us/1.0e6);
  } while ((tval>=ctime) && (status !=-1));
  if (status==-1) return -1;
  fseek(fp,-(status+sjmp),SEEK_CUR);
  if (atme !=NULL) {
    status=SmrFread(fp,&prm,&fit,0);
    *atme=TimeYMDHMSToEpoch(prm.time.yr,
			    prm.time.mo,
			    prm.time.dy,
			    prm.time.hr,
			    prm.time.mt,
			    prm.time.sc+prm.time.us/1.0e6);
  } 
  fseek(fp,-status,SEEK_CUR);
  return 0;
}











