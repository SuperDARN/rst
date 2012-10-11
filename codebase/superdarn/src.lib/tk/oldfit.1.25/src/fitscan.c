/* fitscan.c
   =========
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <zlib.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"
#include "scandata.h"

#include "oldfitread.h"




int OldFitReadRadarScan(struct OldFitFp *fp,int *state,
                     struct RadarScan *ptr,
                     struct RadarParm *prm,struct FitData *fit,
                     int tlen,
                     int lock,int chn) {

  int fstatus=0;
  int flg=0;
  int r;
  struct RadarBeam *bm;
  if (ptr==NULL) return -1;
  if (fit==NULL) return -1;
  if (state==NULL) return -1;
  if (*state!=1) {
    if (chn==0) fstatus=OldFitRead(fp,prm,fit);
    else {
      do {
        fstatus=OldFitRead(fp,prm,fit); /* read first fit record */
        if (fstatus==-1) break;

        /* The logic below will handle mono/stereo mixed days.
         * If the channel is set to zero then the file is mono
         * and we should treat the data as channel A.
         */

           
        } while ( ((chn==2) && (prm->channel !=2)) || 
                ((chn==1) && (prm->channel ==2)));

    }
    if (fstatus==-1) return -1;
  }
  if (*state !=2) {
    ptr->stid=prm->stid;
    ptr->version.major=prm->revision.major;
    ptr->version.minor=prm->revision.minor;
    ptr->st_time=TimeYMDHMSToEpoch(prm->time.yr,prm->time.mo,
			      prm->time.dy,
			      prm->time.hr,prm->time.mt,
			      prm->time.sc+prm->time.us/1.0e6); 
 
    if ((tlen !=0) && (lock !=0)) ptr->st_time=tlen*(int) (ptr->st_time/tlen);
    RadarScanReset(ptr);  
  }
  
  *state=1;
  do {

    bm=RadarScanAddBeam(ptr,prm->nrang);
    if (bm==NULL) {
      flg=-1;
      break;
    }
    bm->time=TimeYMDHMSToEpoch(prm->time.yr,prm->time.mo,
			      prm->time.dy,
			      prm->time.hr,prm->time.mt,
			      prm->time.sc+prm->time.us/1.0e6); 
 
    bm->scan=prm->scan;
    bm->bm=prm->bmnum;
    bm->cpid=prm->cp;
    bm->intt.sc=prm->intt.sc;
    bm->intt.us=prm->intt.us;
    bm->nave=prm->nave;
    bm->frang=prm->frang;
    bm->rsep=prm->rsep;
    bm->rxrise=prm->rxrise;
    bm->freq=prm->tfreq;
    bm->noise=prm->noise.search;
    bm->atten=prm->atten;
    bm->channel=prm->channel;
    bm->nrang=prm->nrang;

    for (r=0;r<bm->nrang;r++) bm->sct[r]=0;
    for (r=0;r<bm->nrang;r++) {
      bm->sct[r]=(fit->rng[r].qflg==1);
      bm->rng[r].gsct=fit->rng[r].gsct;
      bm->rng[r].p_0=fit->rng[r].p_0;
      bm->rng[r].p_0_e=0;
      bm->rng[r].v=fit->rng[r].v;
      bm->rng[r].p_l=fit->rng[r].p_l;
      bm->rng[r].w_l=fit->rng[r].w_l;
      bm->rng[r].v_e=fit->rng[r].v_err;    
    }
   
    ptr->ed_time=TimeYMDHMSToEpoch(prm->time.yr,prm->time.mo,
			      prm->time.dy,
			      prm->time.hr,prm->time.mt,
			      prm->time.sc+prm->time.us/1.0e6); 
 
    if (ptr->num>100) {
      flg=-1;
      break;
    }
    if (chn==0) fstatus=OldFitRead(fp,prm,fit);
    else {
      do { 
        fstatus=OldFitRead(fp,prm,fit);
        if (fstatus==-1) break;

        /* The logic below will handle mono/stereo mixed days.
         * If the channel is set to zero then the file is mono
         * and we should treat the data as channel A.
         */

           
        } while ( ((chn==2) && (prm->channel !=2)) || 
                ((chn==1) && (prm->channel ==2)));

    }
    if (fstatus==-1) flg=2;
    else {
      if (tlen==0) {
        if (prm->scan==1) flg=1;
      } else if (ptr->ed_time-ptr->st_time>=tlen) flg=1;
    }
  } while (flg==0);
  if (flg>0) flg--;
  if (flg==1) *state=2;
  return flg;
}

