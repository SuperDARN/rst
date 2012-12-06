/* fit_noise.c
   ===========
   Author: R.J.Barnes & K.Baker
*/

/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.2-beta-4-g32f7302.txt for more information.
 
 
 
*/



#include <math.h>
#include "rmath.h"
#include "badsmp.h"
#include "fitblk.h"
#include "fit_acf.h"

void fit_noise(struct complex *ncf,int *badlag,struct FitACFBadSample *badsmp,
			   double skynoise,struct FitPrm *prm,
			   struct FitRange *ptr) {
 
  int lag_lim;
  int istat, j;
  double A, tau, t;
  

  lag_lim = prm->mplgs/2;	/* require at least 1/2 of lags be OK*/

  istat = fit_acf(ncf, 1, badlag, badsmp, lag_lim, prm, 0.0, 0,0.0,ptr);

  tau = prm->mpinc * 1.0e-6;

  if (istat ==1 && (ptr->v == 0.0)) ptr->v = 1.0;
  if (istat == 1 && (fabs(ptr->v_err/ptr->v) < 0.05)) {
    if (ptr->w_l_err <= ptr->w_s_err) {
      if (ptr->p_s > skynoise) ptr->p_s = skynoise;
      A = exp(ptr->p_s);
      for (j=0; j < prm->mplgs; ++j) {
        t = (prm->lag[1][j] - prm->lag[0][j])*tau;
        ncf[j].x = A*exp(-ptr->w_l*t)*cos(ptr->v*t);
        ncf[j].y = A*exp(-ptr->w_l*t)*sin(ptr->v*t);
      }
    } else {
      if (ptr->p_s > skynoise) ptr->p_s = skynoise;
      A = exp(ptr->p_s);
      for (j=0; j < prm->mplgs; ++j) {
        t = (prm->lag[1][j] - prm->lag[0][j])*tau;
        ncf[j].x = A*exp(-(ptr->w_s*t)*(ptr->w_s*t))*cos(ptr->v*t);
        ncf[j].y = A*exp(-(ptr->w_s*t)*(ptr->w_s*t))*sin(ptr->v*t);
      }
    }
  } else
    for (j=0; j < prm->mplgs; ++j) {
      ncf[j].x = 0;
      ncf[j].y = 0;
    }
  return;
}
