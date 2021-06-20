/* smrscan.c
   ========= */

/*

Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

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
#include <sys/stat.h>
#include <zlib.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"
#include "scandata.h"
#include "smrread.h"



int SmrRadarScan(FILE *fp,int *state,
                 struct RadarScan *ptr,struct RadarParm *prm,
                 struct FitData *fit,int fbeam,
                 int tlen,
                 int lock,int chn) {

  int fstatus=0;
  int flg=0;
  int n=0;
  int r;
  struct RadarBeam *tmp;
  struct RadarBeam *bm;
  if (ptr==NULL) return -1;
  if (fit==NULL) return -1;
  if (fp==NULL) return -1;
  if (state==NULL) return -1;
  if (*state!=1) {
    if (chn==0) fstatus=SmrFread(fp,prm,fit,fbeam);
    else {
      do {
        fstatus=SmrFread(fp,prm,fit,fbeam); /* read first fit record */
        if (fstatus==-1) break;
      } while (prm->channel !=chn);
    }
    if (fstatus==-1) return -1;
  }
  if (*state !=2) {
    ptr->stid=prm->stid;
    ptr->version.major=1;
    ptr->version.minor=0;
    ptr->st_time=TimeYMDHMSToEpoch(prm->time.yr,
			           prm->time.mo,
			           prm->time.dy,
			           prm->time.hr,
			           prm->time.mt,
			           prm->time.sc+prm->time.us/1.0e6);
    if ((tlen !=0) && (lock !=0)) ptr->st_time=tlen*(int) (ptr->st_time/tlen);
    RadarScanReset(ptr);  
  }
  
  *state=1;
  do {

    n=ptr->num;
    if (ptr->bm==NULL) ptr->bm=malloc(sizeof(struct RadarBeam));
    else {
      tmp=realloc(ptr->bm,sizeof(struct RadarBeam)*(n+1));
      if (tmp==NULL) {
        flg=-1;        
        break;
      }
      ptr->bm=tmp;
    }
    
    bm=&ptr->bm[n];    
 
    bm->time=TimeYMDHMSToEpoch(prm->time.yr,
			       prm->time.mo,
			       prm->time.dy,
			       prm->time.hr,
			       prm->time.mt,
			       prm->time.sc+prm->time.us/1.0e6);
  
    bm->bm=prm->bmnum;
    bm->scan=prm->scan;
    bm->cpid=prm->cp;
    bm->intt.sc=prm->intt.sc;
    bm->intt.us=prm->intt.us;
    bm->frang=prm->frang;
    bm->rsep=prm->rsep;
    bm->rxrise=prm->rxrise;
    bm->freq=prm->tfreq;
    bm->noise=prm->noise.search;
    bm->atten=prm->atten;
    bm->channel=prm->channel;
    bm->nrang=prm->nrang;

    bm->sct=malloc(sizeof(char)*bm->nrang);
    bm->rng=malloc(sizeof(struct RadarCell)*bm->nrang);

    for (r=0;r<bm->nrang;r++) bm->sct[r]=0;
    for (r=0;r<bm->nrang;r++) {
      bm->sct[r]=(fit->rng[r].qflg==1);
      bm->rng[r].gsct=fit->rng[r].gsct;
      bm->rng[r].v=fit->rng[r].v;
      bm->rng[r].p_l=fit->rng[r].p_l;
      bm->rng[r].w_l=fit->rng[r].w_l;
      bm->rng[r].v_e=fit->rng[r].v_err;    
    }
    
    ptr->num++;
    ptr->ed_time=TimeYMDHMSToEpoch(prm->time.yr,
			           prm->time.mo,
			           prm->time.dy,
			           prm->time.hr,
			           prm->time.mt,
			           prm->time.sc+prm->time.us/1.0e6);
    if (ptr->num>100) {
      flg=-1;
      break;
    }
    if (chn==0) fstatus=SmrFread(fp,prm,fit,fbeam);
    else {
      do { 
        fstatus=SmrFread(fp,prm,fit,fbeam);
        if (fstatus==-1) break;
      } while (prm->channel !=chn);
    }
    if (fstatus==-1) flg=2;
    else {
      if (tlen==0) {
        if (abs(prm->scan)==1) flg=1;
      } else if (ptr->ed_time-ptr->st_time>=tlen) flg=1;
    }
  } while (flg==0);
  if (flg>0) flg--;
  if (flg==1) *state=2;
  return flg;
}



