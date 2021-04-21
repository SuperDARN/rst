/* fitpacket.c
   =========== 
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
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <zlib.h>
#include "rtypes.h"

#include "rconvert.h"
#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"
#include "cnxstr.h"

#define read_bit(t,n) ((t[n/8] & (1<<(n%8))) !=0)
#define set_bit(t,n)  t[n/8]=t[n/8] | (1<<(n%8))
#define set_top(t,n) t[9]=t[9] | (n & 0x0f)<<4
#define read_top(t)  ((t[9] >> 4) &0x0f)




unsigned char *fitpacket(struct RadarParm *prm,
              struct FitData *fit,int *size) {

  int pat[]={1,2,2,17,4,2,2,14,4,4,2,4,0,0};
  int dpat[]={8,0,0,0};

  int rtab[ORIG_MAX_RANGE];
  float *snr;

  unsigned char *ptr=NULL;                
  unsigned char *dflg;
  unsigned char *gsct;
  double *store;

  int rng_num,loop,l,inx,s,step;
  float maxval;

  struct radar_parms oldprm;

  oldprm.REV.MAJOR=prm->revision.major;
  oldprm.REV.MINOR=prm->revision.minor;
  oldprm.NPARM=48;
  oldprm.CP=prm->cp;
  oldprm.ST_ID=prm->stid;
  oldprm.YEAR=prm->time.yr;
  oldprm.MONTH=prm->time.mo;
  oldprm.DAY=prm->time.dy;
  oldprm.HOUR=prm->time.hr;
  oldprm.MINUT=prm->time.mt;
  oldprm.SEC=prm->time.sc;
   
  oldprm.TXPOW=prm->txpow;
  oldprm.NAVE=prm->nave;
  oldprm.ATTEN=prm->atten;
  oldprm.LAGFR=prm->lagfr;
  oldprm.SMSEP=prm->smsep;
  oldprm.ERCOD=prm->ercod;
  oldprm.AGC_STAT=prm->stat.agc;
  oldprm.LOPWR_STAT=prm->stat.lopwr;
  oldprm.NOISE=prm->noise.search;
  oldprm.NOISE_MEAN=prm->noise.mean;
      
  oldprm.CHANNEL=prm->channel;
  oldprm.BMNUM=prm->bmnum;
  oldprm.SCAN=prm->scan;
 
  oldprm.RXRISE=prm->rxrise;
  oldprm.INTT=prm->intt.sc;
  oldprm.TXPL=prm->txpl;
    
  oldprm.MPINC=prm->mpinc;
  oldprm.MPPUL=prm->mppul;
  oldprm.MPLGS=prm->mplgs;
  oldprm.NRANG=prm->nrang;
  oldprm.FRANG=prm->frang;
  oldprm.RSEP=prm->rsep;
  oldprm.XCF=prm->xcf; 
  oldprm.TFREQ=prm->tfreq;
  oldprm.MXPWR=prm->mxpwr;
  oldprm.LVMAX=prm->lvmax;   

  snr=malloc(sizeof(float)*prm->nrang);
  if (snr==NULL) return NULL;

  for (l=0;l<prm->nrang;l++) {
    if (prm->noise.search>0)
      snr[l]=fit->rng[l].p_0/prm->noise.search;
      else snr[l]=0;
  }
  
  if (prm->nrang>ORIG_MAX_RANGE) {
    step=prm->nrang/ORIG_MAX_RANGE;
    for (l=0;l<ORIG_MAX_RANGE;l++) {
      maxval=0.0;
      inx=l*step;
      for (s=0;s<step;s++) {
        /*
                    if (fit->rng[l*step+s].qflg==0) continue;
                    if (fit->rng[l*step+s].gsct !=0) continue;
	*/
	if (snr[l*step+s]>maxval) {
	  maxval=snr[l*step+s];
	  inx=l*step+s;
	}
      }
      rtab[l]=inx;
    }
    oldprm.RSEP=prm->rsep*step;
  } else for (l=0;l<ORIG_MAX_RANGE;l++) rtab[l]=l;
  free(snr);

  rng_num=0;
  for (loop=0;loop<ORIG_MAX_RANGE;loop++) 
    if (fit->rng[rtab[loop]].qflg ==1) rng_num++;
 
  ptr=malloc(sizeof(struct radar_parms)+21+3*sizeof(double)*rng_num);
  if (ptr==NULL) return 0;

  ptr[0]='d';
  gsct=ptr+1+sizeof(struct radar_parms);
  dflg=gsct+10;
  store=(double *) (dflg+10);

  memcpy(ptr+1,&oldprm,sizeof(struct radar_parms));
  ConvertBlock(ptr+1,pat);
 
  memset(gsct,0,10);
  memset(dflg,0,10);

  /* work out which ranges to store */
   
  for (loop=0;loop<ORIG_MAX_RANGE;loop++) {
    if (fit->rng[rtab[loop]].gsct !=0) set_bit(gsct,loop);
    if (fit->rng[rtab[loop]].qflg ==1) set_bit(dflg,loop);
  }
 
  /* store the ranges */
      
  if (rng_num>0) {
    int index=0;
    for (loop=0;loop<ORIG_MAX_RANGE;loop++) {
      if (read_bit(dflg,loop) !=0) {
        store[index]=fit->rng[rtab[loop]].p_l;
        store[index+rng_num]=fit->rng[rtab[loop]].v;
        store[index+2*rng_num]=fit->rng[rtab[loop]].w_l;  
	/* store[index+2*rng_num]=fit->rng[rtab[loop]].v_err; */
        index++;
      } 
    }
  }

  /* convert the stored values */
  dpat[1]=3*rng_num;
  ConvertBlock( (unsigned char *) store,dpat);
  
  *size=(sizeof(struct radar_parms)+21+3*sizeof(double)*rng_num);
  return ptr;
}


