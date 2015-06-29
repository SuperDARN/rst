/* noise_acf.c
   ===========
   Author: R.J.Barnes & K.Baker
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



#include <math.h>
#include "rmath.h"
#include "badsmp.h"
#include "fitblk.h"
#include "badlags.h"
#include "noise_stat.h"


#define PLIM 1.6

double noise_acf(double mnpwr,struct FitPrm *ptr,
	             double *pwr, struct FitACFBadSample *badsmp,
				 struct complex *raw,
				 struct complex *n_acf) {
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
	n_acf[i].x = 0;
	n_acf[i].y= 0;
	np[i] = 0;
  }
  plim = PLIM * mnpwr;

  for (i=0; i< ptr->nrang; i++) {
    if ((pwr[i] < plim) && ((fabs(raw[i*ptr->mplgs].x) + 
			fabs(raw[i*ptr->mplgs].y)) > 0) &&
			(fabs(raw[i*ptr->mplgs].x) < plim) &&
			(fabs(raw[i*ptr->mplgs].y) < plim)) {
	  FitACFCkRng((i+1), bad,badsmp, ptr);

	  for (j=0; j< ptr->mplgs; j++) {
	    if ((fabs(raw[i*ptr->mplgs+j].x) < plim) &&
			(fabs(raw[i*ptr->mplgs+j].y) < plim) &&
			(bad[j] == 0)) {
		  n_acf[j].x = n_acf[j].x + raw[i*ptr->mplgs+j].x;
		  n_acf[j].y = n_acf[j].y + raw[i*ptr->mplgs+j].y;
		  ++(np[j]);
		}
	  }
	}
  }

  if (np[0] <= 2) {
	for (i=0; i < ptr->mplgs; ++i) {
	  n_acf[i].x = 0;
	  n_acf[i].y = 0;
	}
    return 0.0;
  }

  for (i=0; i< ptr->mplgs; i++) {
	if (np[i] > 2) {
	  n_acf[i].x = n_acf[i].x/np[i];
	  n_acf[i].y = n_acf[i].y/np[i];
	} else {
	  n_acf[i].x = 0;
	  n_acf[i].y= 0;
	}
  }

  /*  Now determine the average power in the non-zero lags of the noise acf */

  for (i=1, P=0; i < ptr->mplgs; ++i) {
	P = P + lag_power(&n_acf[i]);
  }
  P = P/(ptr->mplgs - 1);
  return P;
}
