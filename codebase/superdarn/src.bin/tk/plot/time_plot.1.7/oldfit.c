/* oldfit.c
   ========
   Author: R.J.Barnes
*/


/*
   See license.txt
*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <zlib.h>
#include "rtime.h"
#include "rtypes.h"
#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"

#include "oldfitread.h"
#include "tplot.h"



double oldfit_find(struct OldFitFp *fitfp,struct RadarParm *prm,
                struct FitData *fit,double sdate,
                double stime) {
  int status;
  int yr,mo,dy,hr,mt;
  double sc;
  double atime;
  if (OldFitRead(fitfp,prm,fit)==-1) return -1;
  atime=TimeYMDHMSToEpoch(prm->time.yr,prm->time.mo,prm->time.dy,
                          prm->time.hr,prm->time.mt,
                          prm->time.sc+prm->time.us/1.0e6);
       	                 

  if ((stime==-1) && (sdate==-1)) return atime;
  if (stime==-1) stime= ( (int) atime % (24*3600));
  if (sdate==-1) stime+=atime - ( (int) atime % (24*3600));
  else stime+=sdate;
  TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sc);
  if (atime>=stime) return stime;
  status=OldFitSeek(fitfp,yr,mo,dy,hr,mt,sc,NULL);
  if (status==-1) return -1;
  if (OldFitRead(fitfp,prm,fit)==-1) return -1;
  return stime;
}

double oldfit_scan(double ctime,struct OldFitFp *fitfp,int rflg,
                struct RadarParm *prm,struct FitData *fit,
                int bmnum,int chnum,int cpid,int sflg,int scan) {


  double atime;
  int flg=1;
  if (rflg) {
    if (OldFitRead(fitfp,prm,fit)==-1) return -1;
  }
  while (1) {
   
    atime=TimeYMDHMSToEpoch(prm->time.yr,prm->time.mo,prm->time.dy,
                          prm->time.hr,prm->time.mt,
                          prm->time.sc+prm->time.us/1.0e6);
  
    if (atime<ctime) flg=0;
    if ((bmnum !=-1) && (prm->bmnum !=bmnum)) flg=0;
    if ((sflg) && (prm->scan !=scan)) flg=0;
    if (chnum !=-1) {
       if ((chnum==2) && (prm->channel!=2)) flg=0;
       if ((chnum==1) && (prm->channel==2)) flg=0;
    }
    if ((cpid !=-1) && (prm->cp !=cpid)) flg=0;
    if (flg==1) break;
    if (OldFitRead(fitfp,prm,fit)==-1) return -1;
    flg=1;
  }
  
  return atime;
}

