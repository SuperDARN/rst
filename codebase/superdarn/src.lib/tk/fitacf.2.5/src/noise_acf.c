/* noise_acf.c
   ===========
   Author: R.J.Barnes & K.Baker
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
#include <complex.h>
#include "rmath.h"
#include "badsmp.h"
#include "fitblk.h"
#include "badlags.h"
#include "noise_stat.h"


#define PLIM 1.6

double noise_acf(double mnpwr,struct FitPrm *ptr, double *pwr, 
        struct FitACFBadSample *badsmp, double complex *raw, 
        double complex *n_acf) {
  int i, j;

  int *np=NULL;
  int *bad=NULL; 
  double plim, P;

  np=malloc(sizeof(int)*ptr->mplgs);
  if (np==NULL) return -1;
  memset(np,0,sizeof(int)*ptr->mplgs);

  bad=malloc(sizeof(int)*ptr->bad);
  if (bad==NULL) {
    free(np);
    return -1;
  }
  memset(bad,0,sizeof(int)*ptr->mplgs);

  for (i=0; i< ptr->mplgs; i++) {
	n_acf[i] = 0.0 + 0.0 * I;
	np[i] = 0;
  }
  plim = PLIM * mnpwr;

  for (i=0; i< ptr->nrang; i++) {
    if ((pwr[i] < plim) && ((fabs(creal(raw[i*ptr->mplgs])) + 
			fabs(cimag(raw[i*ptr->mplgs]))) > 0) &&
			(fabs(creal(raw[i*ptr->mplgs])) < plim) &&
			(fabs(cimag(raw[i*ptr->mplgs])) < plim)) {
	  FitACFCkRng((i+1), bad,badsmp, ptr);

	  for (j=0; j< ptr->mplgs; j++) {
	    if ((fabs(creal(raw[i*ptr->mplgs+j])) < plim) &&
			(fabs(cimag(raw[i*ptr->mplgs+j])) < plim) &&
			(bad[j] == 0)) {
		  n_acf[j] = n_acf[j] + raw[i*ptr->mplgs+j];
		  ++(np[j]);
		}
	  }
	}
  }

  if (np[0] <= 2) {
	for (i=0; i < ptr->mplgs; ++i) {
	  n_acf[i] = 0.0 + 0.0 * I;
	}
    free(np);
    free(bad);
    return 0.0;
  }

  for (i=0; i< ptr->mplgs; i++) {
	if (np[i] > 2) {
	  n_acf[i] = n_acf[i])/np[i];
	} else {
	  n_acf[i] = 0.0 + 0.0 * I;
	}
  }

  /*  Now determine the average power in the non-zero lags of the noise acf */

  for (i=1, P=0; i < ptr->mplgs; ++i) {
	P = P + lag_power(&n_acf[i]);
  }
  P = P/(ptr->mplgs - 1);
  free(np);
  free(bad);
  return P;
}
