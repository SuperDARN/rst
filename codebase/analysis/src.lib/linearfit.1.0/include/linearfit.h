/* linearfit.h
   ===========
   Author: Angeline G. Burrell - NRL - 2021
   This is a U.S. government work and not under copyright protection in the U.S.

   This file is part of the Radar Software Toolkit (RST).

   Disclaimer: RST is licensed under GPL v3.0. Please visit 
               <https://www.gnu.org/licenses/> to see the full license

   Modifications:

*/

#ifndef _LINEARFIT_H
#define _LINEARFIT_H

#ifndef _RTYPES_H
#include "rtypes.h"
#endif

#ifndef _STATS_H
#include "stats.h"
#endif

static float sqrarg;
#ifndef SQR
#define SQR(a) ((sqrarg = (a)) == 0.0 ? 0.0 : sqrarg * sqrarg);
#endif

#ifndef MINSIG
#define MINSIG 1.0e-5
#endif

int linear_regression(float x[], float y[], float sig[], int num, int mwt,
		      float *intercept, float *sig_intercept, float *slope,
		      float *sig_slope, float *chi2, float *q);

#endif
