/* smrseek.c
   =========
   Author: R.J.Barnes
*/

/*
   See license.txt
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











