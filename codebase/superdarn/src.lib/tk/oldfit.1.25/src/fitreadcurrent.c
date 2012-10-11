/* fitreadcurrent.c
   ================
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

/* This routine reads in and decodes a record from a fit file */

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
#include "rtime.h"
#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"

#include "oldfitread.h"
#include "fitstr.h"



int OldFitReadCurrent(struct OldFitFp *ptr, 
                      struct RadarParm *prm,struct FitData *fit) {

  int r1_pat[]={4,2,1,2,2,17,4,2,2,14,4,4,2,4,
                2,ORIG_PULSE_PAT_LEN,2,2*ORIG_LAG_TAB_LEN,1,
                ORIG_COMBF_SIZE,4,3,
		2,2*ORIG_MAX_RANGE,1,ORIG_MAX_RANGE,0,0};

  int r2_pat[]={4,3,1,25,1,25,2,475,0,0};



  int c;
  int hlength;
  int rng,i,icnt=0,inum=0;
  union fit_out r;
  struct fitdata oldfit;
  struct fitdata *fptr;

  int16 lag[2*ORIG_LAG_TAB_LEN];

     
  /* read in a record block */ 

  fptr=&oldfit;
  memset(fptr,0,sizeof(struct fitdata));

  if (read(ptr->fitfp,&r,sizeof(union fit_out)) !=sizeof(union fit_out)) 
    return -1;

  ConvertBlock( (unsigned char *) &r,r1_pat);
  if (r.r1.rrn !=0) return -1;
  
  /* ptr->ctime=r.r1.r_time; */
  ptr->blen=1;
  hlength=sizeof(struct radar_parms)+sizeof(int16)*
                (ORIG_PULSE_PAT_LEN+2*ORIG_LAG_TAB_LEN)+ORIG_COMBF_SIZE;

  memcpy(&fptr->prms,&(r.r1.plist[0]),hlength);

  if (fptr->prms.YEAR==0) return -1;

  fptr->noise.skynoise=r.r1.r_noise_lev;
  fptr->noise.lag0=r.r1.r_noise_lag0;
  fptr->noise.vel=r.r1.r_noise_vel/10.0; 

  /* substitute the actual time past epoch for the old year sec time */
  ptr->ctime=TimeYMDHMSToEpoch(fptr->prms.YEAR,
                        fptr->prms.MONTH,
			fptr->prms.DAY,
			fptr->prms.HOUR,
			fptr->prms.MINUT,
			fptr->prms.SEC);


  /* calculate how many records to store the data*/
  for (i=0;i<fptr->prms.NRANG;i++) {
     fptr->rng[i].p_0=((double) r.r1.r_pwr0[i]/100.0);
     if (r.r1.r_slist[i] !=0) {
        icnt++;
        fptr->rng[r.r1.r_slist[i]-1].nump=r.r1.r_numlags[i];
     }
  }
  if (icnt>0) {
    while (inum<icnt) {
      if (read(ptr->fitfp,&r,sizeof(union fit_out)) 
            != sizeof(union fit_out)) return -1;
      ConvertBlock( (unsigned char *) &r,r2_pat);
      ptr->blen++;
      if ((r.r2.rrn==0) || (r.r2.r_xflag!=0)) return -1;

      for (i=0;(i<25) && (inum<icnt);i++) {
        rng=r.r2.range[i];


        fptr->rng[rng-1].qflg=r.r2.r_qflag[i];
        fptr->rng[rng-1].gsct=r.r2.r_gscat[i];
        fptr->rng[rng-1].p_l=((double) r.r2.r_pwr_l[i])/100.0;
        fptr->rng[rng-1].p_s=((double) r.r2.r_pwr_s[i])/100.0;
        fptr->rng[rng-1].p_l_err=((double) r.r2.r_pwr_l_err[i])/100.0;
        fptr->rng[rng-1].p_s_err=((double) r.r2.r_pwr_s_err[i])/100.0;
        fptr->rng[rng-1].w_l=((double) r.r2.r_w_l[i])/10.0;
        fptr->rng[rng-1].w_s=((double) r.r2.r_w_s[i])/10.0;
        fptr->rng[rng-1].w_l_err=((double) r.r2.r_w_l_err[i])/10.0;
        fptr->rng[rng-1].w_s_err=((double) r.r2.r_w_s_err[i])/10.0;

        fptr->rng[rng-1].v=((double) r.r2.r_vel[i])/10.0;
        fptr->rng[rng-1].v_err=((double) r.r2.r_vel_err[i])/10.0;
        fptr->rng[rng-1].sdev_l=((double) r.r2.r_sdev_l[i])/1000.0;
        fptr->rng[rng-1].sdev_s=((double) r.r2.r_sdev_s[i])/1000.0;
        fptr->rng[rng-1].sdev_phi=((double) r.r2.r_sdev_s[i])/100.0;
        inum++;
      }
    }
    inum=0;
    if (fptr->prms.XCF !=0) {
      while (inum<icnt) {
        if (read(ptr->fitfp,&r,sizeof(union fit_out)) 
              != sizeof(union fit_out)) return -1;
       
        ConvertBlock( (unsigned char *) &r,r2_pat);
        ptr->blen++;
        if ((r.r2.rrn==0) || (r.r2.r_xflag ==0)) return -1;

        for (i=0;(i<25) && (inum<icnt);i++) {
          rng=r.r2.range[i];
          fptr->xrng[rng-1].qflg=r.r2.r_qflag[i];
          fptr->xrng[rng-1].gsct=r.r2.r_gscat[i];
          fptr->xrng[rng-1].p_l=((double) r.r2.r_pwr_l[i])/100.0;
          fptr->xrng[rng-1].p_s=((double) r.r2.r_pwr_s[i])/100.0;
          fptr->xrng[rng-1].p_l_err=((double) r.r2.r_pwr_l_err[i])/100.0;
          fptr->xrng[rng-1].p_s_err=((double) r.r2.r_pwr_s_err[i])/100.0;

          fptr->xrng[rng-1].w_l=((double) r.r2.r_w_l[i])/10.0;
          fptr->xrng[rng-1].w_s=((double) r.r2.r_w_s[i])/10.0;
          fptr->xrng[rng-1].w_l_err=((double) r.r2.r_w_l_err[i])/10.0;
          fptr->xrng[rng-1].w_s_err=((double) r.r2.r_w_s_err[i])/10.0;

          fptr->xrng[rng-1].v=((double) r.r2.r_vel[i])/10.0;
          fptr->xrng[rng-1].v_err=((double) r.r2.r_vel_err[i])/10.0;
          fptr->xrng[rng-1].sdev_l=((double) r.r2.r_sdev_l[i])/1000.0;
          fptr->xrng[rng-1].sdev_s=((double) r.r2.r_sdev_s[i])/1000.0;
          fptr->xrng[rng-1].sdev_phi=((double) r.r2.r_sdev_phi[i])/100.0;

          fptr->xrng[rng-1].phi0=((double) r.r2.r_phi0[i])/100.0;
          fptr->xrng[rng-1].phi0_err=((double) r.r2.r_phi0_err[i])/100.0;
          fptr->elev[rng-1].normal=((double) r.r2.r_elev[i])/100.0;
          fptr->elev[rng-1].low=((double) r.r2.r_elev_low[i])/100.0;
          fptr->elev[rng-1].high=((double) r.r2.r_elev_high[i])/100.0;
          inum++;
        }
      }
    }
  }
  if ((fptr->prms.BMNUM<0) || (fptr->prms.BMNUM>15)) 
    return -1;


  prm->revision.major=oldfit.prms.REV.MAJOR;
  prm->revision.minor=oldfit.prms.REV.MINOR;
  prm->origin.code=-1;
  prm->cp=oldfit.prms.CP;
  prm->stid=oldfit.prms.ST_ID;
  prm->time.yr=oldfit.prms.YEAR;
  prm->time.mo=oldfit.prms.MONTH;
  prm->time.dy=oldfit.prms.DAY;
  prm->time.hr=oldfit.prms.HOUR;
  prm->time.mt=oldfit.prms.MINUT;
  prm->time.sc=oldfit.prms.SEC;
  prm->time.us=0;

  prm->txpow=oldfit.prms.TXPOW;
  prm->nave=oldfit.prms.NAVE;
  prm->atten=oldfit.prms.ATTEN;
  prm->lagfr=oldfit.prms.LAGFR;
  prm->smsep=oldfit.prms.SMSEP;
  prm->ercod=oldfit.prms.ERCOD;
  prm->stat.agc=oldfit.prms.AGC_STAT;
  prm->stat.lopwr=oldfit.prms.LOPWR_STAT;
  prm->noise.search=oldfit.prms.NOISE;
  prm->noise.mean=oldfit.prms.NOISE_MEAN;
      
  prm->channel=oldfit.prms.CHANNEL;
  prm->bmnum=oldfit.prms.BMNUM;
  prm->bmazm=-999.0;
  prm->scan=oldfit.prms.SCAN;
  
  prm->offset=oldfit.prms.usr_resL1; 
  prm->rxrise=oldfit.prms.RXRISE;
  prm->intt.sc=oldfit.prms.INTT;
  prm->intt.us=0;
  prm->txpl=oldfit.prms.TXPL;
    
  prm->mpinc=oldfit.prms.MPINC;
  prm->mppul=oldfit.prms.MPPUL;
  prm->mplgs=oldfit.prms.MPLGS;
  prm->nrang=oldfit.prms.NRANG;
  prm->frang=oldfit.prms.FRANG;
  prm->rsep=oldfit.prms.RSEP;
  prm->xcf=oldfit.prms.XCF; 
  prm->tfreq=oldfit.prms.TFREQ;
  prm->mxpwr=oldfit.prms.MXPWR;
  prm->lvmax=oldfit.prms.LVMAX;   

  for (c=0;c<=prm->mplgs;c++) {
    lag[c*2]=oldfit.lag[0][c];
    lag[c*2+1]=oldfit.lag[1][c];
  }

  RadarParmSetPulse(prm,prm->mppul,oldfit.pulse);
  RadarParmSetLag(prm,prm->mplgs,lag);
  RadarParmSetCombf(prm,oldfit.combf);

  fit->revision.major=ptr->major_rev;
  fit->revision.minor=ptr->minor_rev;

  fit->noise.vel=oldfit.noise.vel;
  fit->noise.lag0=oldfit.noise.lag0;
  fit->noise.skynoise=oldfit.noise.skynoise;

  FitSetRng(fit,prm->nrang);

  for (c=0;c<prm->nrang;c++) {
    fit->rng[c].v=oldfit.rng[c].v;
    fit->rng[c].v_err=oldfit.rng[c].v_err;
    fit->rng[c].p_0=oldfit.rng[c].p_0;
    fit->rng[c].p_l=oldfit.rng[c].p_l;
    fit->rng[c].p_l_err=oldfit.rng[c].p_l_err;
    fit->rng[c].p_s=oldfit.rng[c].p_s;
    fit->rng[c].p_s_err=oldfit.rng[c].p_s_err;
    fit->rng[c].w_l=oldfit.rng[c].w_l;
    fit->rng[c].w_l_err=oldfit.rng[c].w_l_err;
    fit->rng[c].w_s=oldfit.rng[c].w_s;
    fit->rng[c].w_s_err=oldfit.rng[c].w_s_err;
    fit->rng[c].phi0=oldfit.rng[c].phi0;
    fit->rng[c].phi0_err=oldfit.rng[c].phi0_err;
    fit->rng[c].sdev_l=oldfit.rng[c].sdev_l;
    fit->rng[c].sdev_s=oldfit.rng[c].sdev_s;
    fit->rng[c].sdev_phi=oldfit.rng[c].sdev_phi;
    fit->rng[c].qflg=oldfit.rng[c].qflg;
    fit->rng[c].gsct=oldfit.rng[c].gsct;
    fit->rng[c].nump=oldfit.rng[c].nump; 
  }
    
  if (prm->xcf !=0) {
    FitSetXrng(fit,prm->nrang);
    FitSetElv(fit,prm->nrang);
    for (c=0;c<prm->nrang;c++) {
      fit->xrng[c].v=oldfit.xrng[c].v;
      fit->xrng[c].v_err=oldfit.xrng[c].v_err;
      fit->xrng[c].p_0=oldfit.xrng[c].p_0;
      fit->xrng[c].p_l=oldfit.xrng[c].p_l;
      fit->xrng[c].p_l_err=oldfit.xrng[c].p_l_err;
      fit->xrng[c].p_s=oldfit.xrng[c].p_s;
      fit->xrng[c].p_s_err=oldfit.xrng[c].p_s_err;
      fit->xrng[c].w_l=oldfit.xrng[c].w_l;
      fit->xrng[c].w_l_err=oldfit.xrng[c].w_l_err;
      fit->xrng[c].w_s=oldfit.xrng[c].w_s;
      fit->xrng[c].w_s_err=oldfit.xrng[c].w_s_err;
      fit->xrng[c].phi0=oldfit.xrng[c].phi0;
      fit->xrng[c].phi0_err=oldfit.xrng[c].phi0_err;
      fit->xrng[c].sdev_l=oldfit.xrng[c].sdev_l;
      fit->xrng[c].sdev_s=oldfit.xrng[c].sdev_s;
      fit->xrng[c].sdev_phi=oldfit.xrng[c].sdev_phi;
      fit->xrng[c].qflg=oldfit.xrng[c].qflg;
      fit->xrng[c].gsct=oldfit.xrng[c].gsct;
      fit->xrng[c].nump=oldfit.xrng[c].nump; 
      fit->elv[c].low=oldfit.elev[c].low;
      fit->elv[c].normal=oldfit.elev[c].normal;
      fit->elv[c].high=oldfit.elev[c].high;
    }
  } else {
    FitSetXrng(fit,0);
    FitSetElv(fit,0);
  }
 
  return 0;
}




 
