/* badlags.c
   =========
   Author: R.J.Barnes & K. Baker
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



#include "badsmp.h"
#include "fitblk.h"
#include "badlags.h"
#include "rang_badlags.h"

/**
Identifies bad lags caused by transmitter pulse overlap
*/
void FitACFBadlags(struct FitPrm *fitted_prms,struct FitACFBadSample *samples) {
  int i, k, sample;
  long ts, t1=0, t2=0;

  i = -1;
  ts = (long) fitted_prms->lagfr;
  sample = 0;
  k = 0;

  t2 = 0L;

  while (i < (fitted_prms->mppul - 1)) {
	/* first, skip over any pulses that occur before the first sample
	   defines transmitter pulse blanket window*/

	while ((ts > t2) && (i < (fitted_prms->mppul - 1))) {
      i++;
	  t1 = (long) (fitted_prms->pulse[i]) * (long) (fitted_prms->mpinc)
				- fitted_prms->txpl/2;
	  t2 = t1 + 3*fitted_prms->txpl/2 + 100; /* adjust for rx-on delay */
	}	

	/*	we now have a pulse that occurs after the current sample.  Start
		incrementing the sample number until we find a sample that lies
		within the pulse */

	while (ts < t1) {
      sample++;
	  ts = ts + fitted_prms->smsep;
	}
	
	/*	ok, we now have a sample which occurs after the pulse starts.
		check to see if it occurs before the pulse ends, and if so, mark
		it as a bad sample */

	while ((ts >= t1) && (ts <= t2)) {
	  samples->badsmp[k] = sample;
	  k++;
	  sample++;
	  ts = ts + fitted_prms->smsep;
	}
  }
  samples->nbad = k;	/* total number of bad samples */

  /* Now set up a table for checking range interference */
  r_overlap(fitted_prms);
return;
}

/*	This routine uses the table set up by badlags to locate which lags
	are bad for a specified range */

void FitACFCkRng(int range,int *lag,struct FitACFBadSample *samples,
	       struct FitPrm *fitted_prms) {

  int sam1, sam2, i, j;
  int lag_sample_base1, lag_sample_base2, range_offset;

  for (i=0; i<fitted_prms->mplgs; i++) {
	lag[i] = GOOD;

	range_offset = range - 1;
	lag_sample_base1 = fitted_prms->lag[0][i] * (fitted_prms->mpinc/fitted_prms->smsep);
	sam1 = lag_sample_base1 + range_offset;

	lag_sample_base2 = fitted_prms->lag[1][i] * (fitted_prms->mpinc/fitted_prms->smsep);
	sam2 = lag_sample_base2 + range_offset;
			

	for (j=0; j<samples->nbad; j++) {
      if ((sam1 == samples->badsmp[j]) || (sam2 == samples->badsmp[j])) 
        lag[i] = BAD;
	  if (sam2 < samples->badsmp[j]) break;
    }
  }
	
  /* This section of code is only of use to fitacf for reprocessing old
	data that used the 16 lag, 7 pulse code.  */
	
  if ((fitted_prms->mplgs == 17) && (fitted_prms->old !=0) )
	lag[13] = BAD;
	 
  /* finally, check for range interference */
  lag_overlap(range, lag, fitted_prms);
  return;
}
