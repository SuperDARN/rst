/* fit_noise.c
   ===========
   Author: R.J.Barnes & K.Baker
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



#include <math.h>
#include <complex.h>
#include "rmath.h"
#include "badsmp.h"
#include "fitblk.h"
#include "fit_acf.h"

void fit_noise(double complex *ncf,int *badlag,struct FitACFBadSample *badsmp,
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
        ncf[j] = A*exp(-ptr->w_l*t)*cos(ptr->v*t) +
                 A*exp(-ptr->w_l*t)*sin(ptr->v*t) * I;
      }
    } else {
      if (ptr->p_s > skynoise) ptr->p_s = skynoise;
      A = exp(ptr->p_s);
      for (j=0; j < prm->mplgs; ++j) {
        t = (prm->lag[1][j] - prm->lag[0][j])*tau;
        ncf[j] = A*exp(-(ptr->w_s*t)*(ptr->w_s*t))*cos(ptr->v*t) +
                 A*exp(-(ptr->w_s*t)*(ptr->w_s*t))*sin(ptr->v*t) * I;
      }
    }
  } else
    for (j=0; j < prm->mplgs; ++j) {
      ncf[j] = 0.0 + 0.0 * I;
    }
  return;
}
