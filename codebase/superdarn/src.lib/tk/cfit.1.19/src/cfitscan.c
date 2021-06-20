/* cfitscan.c
   ==========
   Author: R.J.Barnes
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


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <zlib.h>
#include "rtypes.h"
#include "cfitdata.h"
#include "scandata.h"
#include "cfitread.h"





int CFitReadRadarScan(struct CFitfp *fp,int *state,
                 struct RadarScan *ptr,struct CFitdata *cfit,int tlen,
                 int lock,int chn) {

  int fstatus=0;
  int flg=0;
  int r,rng;
  struct RadarBeam *bm;
  if (ptr==NULL) return -1;
  if (cfit==NULL) return -1;
  if (fp==NULL) return -1;
  if (state==NULL) return -1;
  if (*state!=1) {
    if (chn==0) fstatus=CFitRead(fp,cfit);
    else {
      do {
        fstatus=CFitRead(fp,cfit); /* read first fit record */
        if (fstatus==-1) break;


        /* The logic below will handle mono/stereo mixed days.
         * If the channel is set to zero then the file is mono
         * and we should treat the data as channel A.
         */


      } while ( ((chn==2) && (cfit->channel !=2)) || 
                ((chn==1) && (cfit->channel ==2)));

    }
    if (fstatus==-1) return -1;
  }
  if (*state !=2) {
    ptr->stid=cfit->stid;
    ptr->version.major=cfit->version.major;
    ptr->version.minor=cfit->version.minor;
    ptr->st_time=cfit->time;
    if ((tlen !=0) && (lock !=0)) ptr->st_time=tlen*(int) (ptr->st_time/tlen);
    RadarScanReset(ptr);  
  }
  
  *state=1;
  do {


    bm=RadarScanAddBeam(ptr,cfit->nrang);
    if (bm==NULL) {
      flg=-1;        
      break;
    }
   
    bm->time=cfit->time;
    bm->scan=cfit->scan;
    bm->bm=cfit->bmnum;
    bm->bmazm=cfit->bmazm;
    bm->cpid=cfit->cp;
    bm->intt.sc=cfit->intt.sc;
    bm->intt.us=cfit->intt.us;
    bm->nave=cfit->nave;
    bm->frang=cfit->frang;
    bm->rsep=cfit->rsep;
    bm->rxrise=cfit->rxrise;
    bm->freq=cfit->tfreq;
    bm->noise=cfit->noise;
    bm->atten=cfit->atten;
    bm->channel=cfit->channel;
    bm->nrang=cfit->nrang;

    for (r=0;r<cfit->nrang;r++) bm->sct[r]=0;
    for (r=0;r<cfit->num;r++) {
      rng=cfit->rng[r];
      bm->sct[rng]=1;
      bm->rng[rng].gsct=cfit->data[r].gsct;
      bm->rng[rng].p_0=cfit->data[r].p_0;
      bm->rng[rng].p_0_e=cfit->data[r].p_0_e;
      bm->rng[rng].v=cfit->data[r].v;
      bm->rng[rng].p_l=cfit->data[r].p_l;
      bm->rng[rng].w_l=cfit->data[r].w_l;
      bm->rng[rng].v_e=cfit->data[r].v_e;   
      bm->rng[rng].p_l_e=cfit->data[r].p_l_e;   
      bm->rng[rng].w_l_e=cfit->data[r].w_l_e;   
    }
    
    ptr->ed_time=cfit->time;
    if (ptr->num>1000) {
      flg=-1;
      break;
    }
    if (chn==0) fstatus=CFitRead(fp,cfit);
    else {
      do { 
        fstatus=CFitRead(fp,cfit);
        if (fstatus==-1) break;

        /* The logic below will handle mono/stereo mixed days.
         * If the channel is set to zero then the file is mono
         * and we should treat the data as channel A.
         */


      } while ( ((chn==2) && (cfit->channel !=2)) || 
                ((chn==1) && (cfit->channel ==2)));

    }
    if (fstatus==-1) flg=2;
    else {
      if (tlen==0) {
        if (cfit->scan==1) flg=1;
      } else if (ptr->ed_time-ptr->st_time>=tlen) flg=1;
    }
  } while (flg==0);
  if (flg>0) flg--;
  if (flg==1) *state=2;
  return flg;
}


int CFitToRadarScan(struct RadarScan *ptr,struct CFitdata *cfit) {

  int n=0;
  int r,rng;
  struct RadarBeam *tmp;
  struct RadarBeam *bm;
  if (ptr==NULL) return -1;
  if (cfit==NULL) return -1;
  
  ptr->stid=cfit->stid;
  
  n=ptr->num;
  if (ptr->bm==NULL) ptr->bm=malloc(sizeof(struct RadarBeam));
  else {
    tmp=realloc(ptr->bm,sizeof(struct RadarBeam)*(n+1));
    if (tmp==NULL) {       
      return -1;
    }
    ptr->bm=tmp;
  }
    
  bm=&ptr->bm[n];    
 
  bm->time=cfit->time;
  
  bm->bm=cfit->bmnum;
  bm->cpid=cfit->cp;
  bm->intt.sc=cfit->intt.sc;
  bm->intt.us=cfit->intt.us;
  bm->nave=cfit->nave;
  bm->frang=cfit->frang;
  bm->rsep=cfit->rsep;
  bm->rxrise=cfit->rxrise;
  bm->freq=cfit->tfreq;
  bm->noise=cfit->noise;
  bm->atten=cfit->atten;
  bm->channel=cfit->channel;
  bm->nrang=cfit->nrang;

  bm->sct=malloc(sizeof(char)*bm->nrang);
  bm->rng=malloc(sizeof(struct RadarCell)*bm->nrang);

  for (r=0;r<cfit->nrang;r++) bm->sct[r]=0;
  for (r=0;r<cfit->num;r++) {
    rng=cfit->rng[r];
    bm->sct[rng]=1;
    bm->rng[rng].gsct=cfit->data[r].gsct;
    bm->rng[rng].v=cfit->data[r].v;
    bm->rng[rng].p_l=cfit->data[r].p_l;
    bm->rng[rng].w_l=cfit->data[r].w_l;
    bm->rng[rng].v_e=cfit->data[r].v_e;   
    bm->rng[rng].p_l_e=cfit->data[r].p_l_e;   
    bm->rng[rng].w_l_e=cfit->data[r].w_l_e;   
  }  
  return 0;
}



