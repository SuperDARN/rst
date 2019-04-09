/* fitseek.c
   ==========
   Author: R.J.Barnes
*/

/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
*/


/* seeks a record from a fitfile */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <zlib.h>
#include "rtypes.h"
#include "rtime.h"
#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"

#include "oldfitread.h"
#include "fitstr.h"
#include "fitreadinx.h"



int OldFitSeek(struct OldFitFp *fp,
	      int yr,int mo,int dy,int hr,int mt,int sc,double *atme) {
 
  int status=0;
  int bjmp=0;
 
  double tval;
  struct RadarParm *prm=NULL;
  struct FitData *fit=NULL;

  if (fp==NULL) return -1;

  tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
  if ((fp->stime !=-1) && (tval<fp->stime)) {
    lseek(fp->fitfp,fp->fit_recl,SEEK_SET);
    fp->ctime=fp->stime;
    if (atme !=NULL) *atme=fp->ctime;
    return 2;
  }

  if ((fp->etime !=-1) && (tval>fp->etime)) {

    int32 inx_rec[4];

    OldFitReadInx(fp,inx_rec,fp->inx_erec);

    lseek(fp->fitfp,(inx_rec[1]-1)*fp->fit_recl,SEEK_SET);
    fp->ctime=fp->etime;
    if (atme !=NULL) *atme=fp->ctime;
    return 3;
  }
 
  if (fp->inxfp !=-1) {
 
    /* search using the index record */

    int32 inx_rec[4];
    int prec=-1;
    int recno=0;
    int srec,erec;
    double stime,etime,sec,itme=0;
    int syrsec;
    stime=fp->stime;
    etime=fp->etime;
    srec=fp->inx_srec;
    erec=fp->inx_erec; 
   
    /* get the actual start time of the file */
    TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&hr,&mt,&sec);

    syrsec=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,sec);
  
    do { 
      prec=recno;
      recno=srec+(int) ((tval-stime)*(erec-srec)/(etime-stime)); 
      if (OldFitReadInx(fp,inx_rec,recno) == -1) status=-1;
      if (status==0) {
        if (inx_rec[0]<syrsec) itme=TimeYMDHMSToEpoch(yr+1,1,1,0,0,0); 
        else itme=TimeYMDHMSToEpoch(yr,1,1,0,0,0); 
        itme+=inx_rec[0];
	if (itme==tval) break;
        else if (itme<tval) {
          srec=recno;
          stime=itme;
        } else {
          erec=recno;
          etime=itme;
        }
      }
    } while ((prec != recno) && (status==0));
    
    if (status==0) {

      if (itme !=tval) {
        recno=srec;
        if (recno>erec) recno=erec; 

      }  
      OldFitReadInx(fp,inx_rec,recno);

      /* align the read fit routine */
      lseek(fp->fitfp,(inx_rec[1]-1)*fp->fit_recl,SEEK_SET);
      if (atme !=NULL) *atme=itme;
      if (itme==tval) return 0;
      else return 1;
    }
  } 

  /* try and do the skip by reading fit records */

  prm=RadarParmMake();
  fit=FitMake();


  if (tval<fp->ctime) {
    /* rewind the file */
    lseek(fp->fitfp,fp->fit_recl,SEEK_SET);
    status=OldFitRead(fp,prm,fit);
  }
  if (tval>=fp->ctime) {
    do {
      bjmp=fp->blen;
      status=OldFitRead(fp,prm,fit);
    } while ((tval>=fp->ctime) && (status==0));
   
    if (status !=0) {
      RadarParmFree(prm);
      FitFree(fit);
      return -1;
    }
    lseek(fp->fitfp,-fp->fit_recl*(fp->blen+bjmp),SEEK_CUR);
  }
  /* now determine the actual time of the record */

  status=OldFitRead(fp,prm,fit);

  /* rewind the file after we've read it so that fit_read gets this record */

  lseek(fp->fitfp,-fp->fit_recl*fp->blen,SEEK_CUR);

  RadarParmFree(prm);
  FitFree(fit);

  if (atme !=NULL) *atme=fp->ctime;
  return 4;
  
}




