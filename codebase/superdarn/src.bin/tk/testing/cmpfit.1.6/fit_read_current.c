/* fit_read_current.c
   ==================
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
#include "lmt.h"
#include "raw.h"
#include "fit.h"

#include "fit_read.h"
#include "fit_str.h"



int fit_read_current(struct fitfp *ptr,struct fitdata *fit_data) {
  int r1_pat[]={4,2,1,2,2,17,4,2,2,14,4,4,2,4,
                2,PULSE_PAT_LEN,2,2*LAG_TAB_LEN,1,ORIG_COMBF_SIZE,4,3,
		2,2*ORIG_MAX_RANGE,1,ORIG_MAX_RANGE,0,0};

  int r2_pat[]={4,3,1,25,1,25,2,475,0,0};

  int hlength;
  int rng,i,icnt=0,inum=0;
  union fit_out r;
   
  /* read in a record block */

  memset(fit_data,0,sizeof(struct fitdata));
  if (ptr->fitfp !=-1) {
    if (read(ptr->fitfp,&r,sizeof(union fit_out)) !=sizeof(union fit_out)) 
      return -1;
  } else {
    if ((ptr->fptr+sizeof(union fit_out))>=ptr->fstat.st_size) return -1;
    memcpy((unsigned char *) &r,ptr->fbuf+ptr->fptr,sizeof(union fit_out));
    ptr->fptr+=sizeof(union fit_out);
  } 
  ConvertBlock( (unsigned char *) &r,r1_pat);
  if (r.r1.rrn !=0) return -1;
  
  /* ptr->ctime=r.r1.r_time; */
  ptr->blen=1;
  hlength=sizeof(struct radar_parms)+sizeof(int16)*
                (PULSE_PAT_LEN+2*LAG_TAB_LEN)+ORIG_COMBF_SIZE;

  memcpy(&fit_data->prms,&(r.r1.plist[0]),hlength);

  if (fit_data->prms.YEAR==0) return -1;

  fit_data->noise.skynoise=r.r1.r_noise_lev;
  fit_data->noise.lag0=r.r1.r_noise_lag0;
  fit_data->noise.vel=r.r1.r_noise_vel; 

  /* substitute the actual time past epoch for the old year sec time */
  ptr->ctime=TimeYMDHMSToEpoch(fit_data->prms.YEAR,
                        fit_data->prms.MONTH,
			fit_data->prms.DAY,
			fit_data->prms.HOUR,
			fit_data->prms.MINUT,
			fit_data->prms.SEC);


  /* calculate how many records to store the data*/
  for (i=0;i<fit_data->prms.NRANG;i++) {
     fit_data->rng[i].p_0=r.r1.r_pwr0[i]/100.0;
     if (r.r1.r_slist[i] !=0) {
        icnt++;
        fit_data->rng[r.r1.r_slist[i]-1].nump=r.r1.r_numlags[i];
     }
  }
  if (icnt==0) return 0;
  while (inum<icnt) {
    if (ptr->fitfp !=-1) {
      if (read(ptr->fitfp,&r,sizeof(union fit_out)) 
          != sizeof(union fit_out)) return -1;
    } else {
      if ((ptr->fptr+sizeof(union fit_out))>=ptr->fstat.st_size) return -1;
      memcpy((unsigned char *) &r,ptr->fbuf+ptr->fptr,sizeof(union fit_out));
      ptr->fptr+=sizeof(union fit_out);
    } 
    ConvertBlock( (unsigned char *) &r,r2_pat);
    ptr->blen++;
    if ((r.r2.rrn==0) || (r.r2.r_xflag!=0)) return -1;

    for (i=0;(i<25) && (inum<icnt);i++) {
      rng=r.r2.range[i];


      fit_data->rng[rng-1].qflg=r.r2.r_qflag[i];
      fit_data->rng[rng-1].gsct=r.r2.r_gscat[i];
      fit_data->rng[rng-1].p_l=((double) r.r2.r_pwr_l[i])/100.0;
      fit_data->rng[rng-1].p_s=((double) r.r2.r_pwr_s[i])/100.0;
      fit_data->rng[rng-1].p_l_err=((double) r.r2.r_pwr_l_err[i])/100.0;
      fit_data->rng[rng-1].p_s_err=((double) r.r2.r_pwr_s_err[i])/100.0;
      fit_data->rng[rng-1].w_l=((double) r.r2.r_w_l[i])/10.0;
      fit_data->rng[rng-1].w_s=((double) r.r2.r_w_s[i])/10.0;
      fit_data->rng[rng-1].w_l_err=((double) r.r2.r_w_l_err[i])/10.0;
      fit_data->rng[rng-1].w_s_err=((double) r.r2.r_w_s_err[i])/10.0;

      fit_data->rng[rng-1].v=((double) r.r2.r_vel[i])/10.0;
      fit_data->rng[rng-1].v_err=((double) r.r2.r_vel_err[i])/10.0;
      fit_data->rng[rng-1].sdev_l=((double) r.r2.r_sdev_l[i])/1000.0;
      fit_data->rng[rng-1].sdev_s=((double) r.r2.r_sdev_s[i])/1000.0;
      fit_data->rng[rng-1].sdev_phi=((double) r.r2.r_sdev_s[i])/100.0;
      inum++;
    }
  }
  inum=0;
  if (fit_data->prms.XCF ==0) {
    if ((fit_data->prms.BMNUM<0) || (fit_data->prms.BMNUM>15)) 
      return fit_read_current(ptr,fit_data);
    return 0;
  }
  while (inum<icnt) {
    if (ptr->fitfp !=-1) {
      if (read(ptr->fitfp,&r,sizeof(union fit_out)) 
          != sizeof(union fit_out)) return -1;
    } else {
      if ((ptr->fptr+sizeof(union fit_out))>=ptr->fstat.st_size) return -1;
      memcpy((unsigned char *) &r,ptr->fbuf+ptr->fptr,sizeof(union fit_out));
      ptr->fptr+=sizeof(union fit_out);
    } 
    ConvertBlock( (unsigned char *) &r,r2_pat);
    ptr->blen++;
    if ((r.r2.rrn==0) || (r.r2.r_xflag ==0)) return -1;

    for (i=0;(i<25) && (inum<icnt);i++) {
      rng=r.r2.range[i];
      fit_data->xrng[rng-1].qflg=r.r2.r_qflag[i];
      fit_data->xrng[rng-1].gsct=r.r2.r_gscat[i];
      fit_data->xrng[rng-1].p_l=((double) r.r2.r_pwr_l[i])/100.0;
      fit_data->xrng[rng-1].p_s=((double) r.r2.r_pwr_s[i])/100.0;
      fit_data->xrng[rng-1].p_l_err=((double) r.r2.r_pwr_l_err[i])/100.0;
      fit_data->xrng[rng-1].p_s_err=((double) r.r2.r_pwr_s_err[i])/100.0;

      fit_data->xrng[rng-1].w_l=((double) r.r2.r_w_l[i])/10.0;
      fit_data->xrng[rng-1].w_s=((double) r.r2.r_w_s[i])/10.0;
      fit_data->xrng[rng-1].w_l_err=((double) r.r2.r_w_l_err[i])/10.0;
      fit_data->xrng[rng-1].w_s_err=((double) r.r2.r_w_s_err[i])/10.0;

      fit_data->xrng[rng-1].v=((double) r.r2.r_vel[i])/10.0;
      fit_data->xrng[rng-1].v_err=((double) r.r2.r_vel_err[i])/10.0;
      fit_data->xrng[rng-1].sdev_l=((double) r.r2.r_sdev_l[i])/1000.0;
      fit_data->xrng[rng-1].sdev_s=((double) r.r2.r_sdev_s[i])/1000.0;
      fit_data->xrng[rng-1].sdev_phi=((double) r.r2.r_sdev_s[i])/100.0;

      fit_data->xrng[rng-1].phi0=((double) r.r2.r_phi0[i])/100.0;
      fit_data->xrng[rng-1].phi0_err=((double) r.r2.r_phi0_err[i])/100.0;
      fit_data->elev[rng-1].normal=((double) r.r2.r_elev[i])/100.0;
      fit_data->elev[rng-1].low=((double) r.r2.r_elev_low[i])/100.0;
      fit_data->elev[rng-1].high=((double) r.r2.r_elev_high[i])/100.0;
      inum++;
    }
  }
  

  if ((fit_data->prms.BMNUM<0) || (fit_data->prms.BMNUM>15)) 
    return fit_read_current(ptr,fit_data);

  return 0;
}

