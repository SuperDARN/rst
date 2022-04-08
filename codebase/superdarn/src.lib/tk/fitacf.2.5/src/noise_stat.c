/* noise_stat.c
   ============
   Author: R.J.Barnes & K.Baker & P.Ponomarenko
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <complex.h>
#include "rmath.h"
#include "fitblk.h"

#include "badsmp.h"
#include "badlags.h"

#define PLIM (1.6)
#define ROOT_3 1.7


double lag_power(double complex a) {
  return sqrt(creal(a)*creal(a) + cimag(a)*cimag(a));
}

double noise_stat(double mnpwr,struct FitPrm *ptr,
                  struct FitACFBadSample *badsmp,
		  double complex *acf) {
  double plim;
  int i, j, np0, npt;
  int *bdlag;
  double var, sigma, P, P2;
  double temp, fluct, low_lim, high_lim;

  plim = PLIM * mnpwr;

  P = 0.0;
  P2 = 0.0;
  var = 0.0;
  np0 = 0;
  npt = 0;
  
  bdlag=malloc(sizeof(int)*ptr->mplgs);
  if (bdlag==NULL) return -1;
  memset(bdlag,0,sizeof(int)*ptr->mplgs);

  for (i=0; i < ptr->nrang; ++i) { 
  	if ((creal(acf[i*ptr->mplgs]) > plim) || (creal(acf[i*ptr->mplgs]) <= 0.0)) 
    	{
		continue;
	}
	FitACFCkRng((i+1), bdlag,badsmp, ptr);
	++np0;
	fluct = ((double) creal(acf[i*ptr->mplgs]))/sqrt(ptr->nave);
	low_lim = creal(acf[i*ptr->mplgs]) - 2.0*fluct;
	if (low_lim < 0) 
		low_lim = low_lim + fluct;
	high_lim = creal(acf[i*ptr->mplgs]) + fluct;

	for (j=1; j < ptr->mplgs; ++j) {
      	if (bdlag[j])
		continue;
	temp = lag_power(acf[i*ptr->mplgs+j]);
	if (temp < low_lim || temp > high_lim)
		continue;
	++npt;
	P = P + temp;
	P2 = P2 + temp*temp;
    }
  }

  free(bdlag);

  if (npt < 2) {
  /*  *signal = 0; */
	return plim/sqrt((double) ptr->nave);
  }

  P = P/npt;
  var = (P2 - P*P*npt)/((double) (npt-1));
  sigma = (var > 0.0) ? sqrt(var) : 0.0;

  /*if ((P >= sigma * ROOT_3) && (sigma > 0.0)) *signal = P;
  else *signal = 0.0; */
  return (P > sigma) ? P : sigma;
}
