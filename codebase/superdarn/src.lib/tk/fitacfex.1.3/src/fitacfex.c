/* fitacfex.c
   ========== 
   Algorithm: R.A.Greenwald, K.Oskavik
   Implementation: R.J.Barnes, R.A.Greenwald
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
#include <time.h>
#include <math.h>
#include <zlib.h>
#include "rtypes.h"
#include "rmath.h"
#include "nrfit.h"
#include "dmap.h"
#include "rprm.h"
#include "rawdata.h"
#include "fitdata.h"
#include "fitblk.h"

void FitACFex(struct RadarParm *prm,struct RawData *raw,
              struct FitData *fit) {


   float minpwr  = 3.0;
   float sderr   = 3.0;
   int   minlag  = 6;
   int   nslopes = 120;
   float a,b,siga,sigb,chi2,q;
   float *model_phi,*model_vels,*model_errors;
   float model_slope,model_vel_pos;
   float model_mean,model_sd,model_min;
   float *data_phi_pos,*data_phi_neg,data_phi;
   float *lagpwr=NULL,*logpwr=NULL,*good_lags=NULL;
   float lag0pwr,re,im,pwr,phi;
   float fitted_width=0.0,fitted_power=0.0;
   float delta_pos,delta_neg,error_neg=0,error_pos=0;
   int   *lag_avail=NULL,availcnt=0,goodcnt=0;
   int   mininx=0, mplgs, /*lastlag, */ lag,i,j,p,R,L;

/* No longer used! Find the highest lag, and allocate memory */

/*   lastlag = 0;
   for (j=0;j<prm->mplgs-1;j++) {
      if (abs(prm->lag[j][0]-prm->lag[j][1])>lastlag) {
         lastlag = abs(prm->lag[j][0]-prm->lag[j][1]);
      }
      }*/

   mplgs        = prm->mplgs;
   model_phi    = malloc(sizeof(float)*(nslopes+1)*mplgs);
   model_vels   = malloc(sizeof(float)*(2*nslopes+1));
   model_errors = malloc(sizeof(float)*(2*nslopes+1));
   lagpwr       = malloc(sizeof(float)*mplgs);
   logpwr       = malloc(sizeof(float)*mplgs);
   data_phi_pos = malloc(sizeof(float)*mplgs);
   data_phi_neg = malloc(sizeof(float)*mplgs);
   lag_avail    = malloc(sizeof(int)*mplgs);
   good_lags    = malloc(sizeof(float)*mplgs);

/* Generate models that will be used in the velocity determination */

   for (i=0;i<=nslopes;i++) {
      model_slope = 180.0*i/nslopes;
      for (j=0;j<mplgs;j++) {
         phi = j*model_slope;
         p = phi/360;
         model_phi[i*mplgs+j] = phi - p*360;
      }
      model_vel_pos = 2.9979E8/2.0*(1-1000.0*prm->tfreq/
         (1000.0*prm->tfreq+model_slope/360.0/(prm->mpinc*1.0e-6)));
      model_vels[nslopes-i] = -model_vel_pos;
      model_vels[nslopes+i] =  model_vel_pos;
   }

   FitSetRng(fit,prm->nrang);
   if (prm->xcf) {
     FitSetXrng(fit,prm->nrang);
     FitSetElv(fit,prm->nrang);
   }

/* Loop every range gate and calculate parameters */

   for (R=0;R<prm->nrang;R++) {
      fit->rng[R].v        = HUGE_VAL;
      fit->rng[R].v_err    = HUGE_VAL;
      fit->rng[R].p_0      = 0.0;
      fit->rng[R].w_l      = 0.0;
      fit->rng[R].w_l_err  = 0.0;
      fit->rng[R].p_l      = 0.0;
      fit->rng[R].p_l_err  = 0.0;
      fit->rng[R].w_s      = 0.0;
      fit->rng[R].w_s_err  = 0.0;
      fit->rng[R].p_s      = 0.0;
      fit->rng[R].p_s_err  = 0.0;
      fit->rng[R].sdev_l   = 0.0;
      fit->rng[R].sdev_s   = 0.0;
      fit->rng[R].sdev_phi = 0.0;
      fit->rng[R].qflg     = 0;
      fit->rng[R].gsct     = 0;
      fit->rng[R].nump     = 0;
      availcnt = 0;
      lag0pwr  = 10.0*log10((raw->acfd[0][R*prm->mplgs] +
                             prm->noise.search)/prm->noise.search);
      for (j=0;j<=2*nslopes;j++) model_errors[j] = 1.0e30;
      for (L=0;L<prm->mplgs-1;L++) {
         lag = L;
         re  = raw->acfd[0][R*prm->mplgs+L];
         im  = raw->acfd[1][R*prm->mplgs+L];
         lagpwr[lag] = sqrt(re*re + im*im);
         if (lagpwr[lag]>raw->acfd[0][R*prm->mplgs]/sqrt(1.0*prm->nave)) {
            lag_avail[availcnt] = lag;
            availcnt++;
         }
         else lagpwr[lag] = 0.0;   
      }
      if ((lag0pwr>=minpwr) && (availcnt>=minlag)) {
       
/* Determine Lambda Power and Spectral Width from least square fit */

         goodcnt = 0;
         for (i=0;i<availcnt;i++) {
           lag = lag_avail[i];
           logpwr[goodcnt]    = log(lagpwr[lag]);
           good_lags[goodcnt] = lag;
           goodcnt++;
         }
         nrfit(good_lags,logpwr,goodcnt,NULL,0,&a,&b,&siga,&sigb,&chi2,&q);
         fitted_width = -2.9979e8*b/(prm->mpinc*1.e-6)/
                                    (2*PI*1000.0*prm->tfreq);
         if (fitted_width<0.00) fitted_width = 0.0;
         fitted_power = log(exp(a) + prm->noise.search);
    
/* Determine Doppler velocity by comparing the phase with models */

         pwr = 0.0;
         for (i=0;i<goodcnt;i++) {
            lag = good_lags[i];
            for (j=0;j<prm->mplgs;j++) {
               if (abs(prm->lag[j][0]-prm->lag[j][1])==lag) {
               L = j;
               }
            } 
            data_phi = atan2(raw->acfd[0][R*prm->mplgs+L],
                             raw->acfd[1][R*prm->mplgs+L])*180.0/PI; 
            data_phi_pos[i] = data_phi;
            data_phi_neg[i] = 360 - data_phi;
            if (data_phi<0) {
               data_phi_pos[i] += 360;
               data_phi_neg[i]  = -data_phi;
            }
            pwr += lagpwr[lag];
         }
         for(i=0;i<=nslopes;i++) {
            error_neg = 0;
            error_pos = 0;
            for (j=0;j<goodcnt;j++) {
               lag = good_lags[j];
               delta_pos = fabs(data_phi_pos[j] - 
                                model_phi[i*mplgs+lag]); 
               delta_neg = fabs(data_phi_neg[j] - 
                                model_phi[i*mplgs+lag]);
               if (delta_pos>180.0) delta_pos = 360 - delta_pos;
               if (delta_neg>180.0) delta_neg = 360 - delta_neg;
               error_neg += delta_neg*delta_neg*lagpwr[lag]/pwr;
               error_pos += delta_pos*delta_pos*lagpwr[lag]/pwr;
            }
            error_neg = sqrt(error_neg);
            error_pos = sqrt(error_pos);
            model_errors[nslopes-i] = error_neg;      
            model_errors[nslopes+i] = error_pos;
         }
         model_mean = 0.0;
         model_sd   = 0.0;
         model_min  = 1.0e30;
         mininx     = 0;
         for (i=0;i<=nslopes*2;i++) {
            model_mean += model_errors[i];
            if (model_errors[i]<model_min) {
               model_min = model_errors[i];
               mininx = i;
            }
         }
         model_mean = model_mean/(nslopes*2+1);

/* Only keep values giving a fit better than 'sterr' Standard Deviations */

         for (i=0;i<=nslopes*2;i++) 
            model_sd += (model_errors[i] - model_mean)*
                        (model_errors[i] - model_mean);
         model_sd = sqrt(model_sd/(nslopes*2));
         if ((model_min<(model_mean - sderr*model_sd)) &&
            (10*log10((exp(a) + prm->noise.search)/
             prm->noise.search)> minpwr)) {
            fit->rng[R].v     = model_vels[mininx];
            fit->rng[R].v_err = model_vels[1] - model_vels[0];
            fit->rng[R].qflg  = 1;
            fit->rng[R].p_0   = raw->pwr0[R];
            fit->rng[R].p_l   = 10.0*(fitted_power/2.3026 - 
                                log10(prm->noise.search));
            fit->rng[R].w_l   = fitted_width;
            fit->rng[R].nump  = goodcnt;    
            if ((fabs(fit->rng[R].v)<30) && (fit->rng[R].w_l<30)) 
                  fit->rng[R].gsct = 1;
         }
      }
      if (prm->xcf) {
        fit->xrng[R].phi0 = 0.0;
        fit->elv[R].normal = 0.0;
        fit->elv[R].high = 0.0;
        fit->elv[R].low = 0.0;
      }
   }
   free(model_phi);
   free(model_vels);
   free(model_errors);
   free(lagpwr);
   free(logpwr);
   free(data_phi_pos);
   free(data_phi_neg);
   free(lag_avail);
   free(good_lags);

   return;
}
