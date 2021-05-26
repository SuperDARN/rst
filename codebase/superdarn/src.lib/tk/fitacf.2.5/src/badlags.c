/* badlags.c
   =========
   Author: R.J.Barnes & K. Baker

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

#include "badsmp.h"
#include "fitblk.h"

#include "rang_badlags.h"


void FitACFBadlags(struct FitPrm *ptr,struct FitACFBadSample *bptr) {
  int i, k, sample;
  long ts, t1=0, t2=0;

  i = -1;
  ts = (long) ptr->lagfr;
  sample = 0;
  k = 0;

  t2 = 0L;

  /* The loops below assume that smsep is not zero...that is not always the case */
  if ( ptr->smsep <= 0 )
  {
      /* First lets do a check to see if txpl is valid so that we can use that in place of smsep */
      if ( ptr->txpl <= 0 )
      {
          fprintf( stderr, "FitACFBadlags: ERROR, both smsep and txpl are invalid...\n");
          return;
      }
      /* If txpl is a valid value, lets set it as smsep and throw off a warning */
      fprintf( stderr, "FitACFBadlags: WARNING using txpl instead of smsep...\n");
      ptr->smsep = ptr->txpl;
  }

  while (i < (ptr->mppul - 1)) {
	/* first, skip over any pulses that occur before the first sample */

	while ((ts > t2) && (i < (ptr->mppul - 1))) {
      i++;
	  t1 = (long) (ptr->pulse[i]) * (long) (ptr->mpinc)
				- ptr->txpl/2;
	  t2 = t1 + 3*ptr->txpl/2 + 100; /* adjust for rx-on delay */
	}

	/*	we now have a pulse that occurs after the current sample.  Start
		incrementing the sample number until we find a sample that lies
		within the pulse */

	while (ts < t1) {
      sample++;
	  ts = ts + ptr->smsep;
	}

	/*	ok, we now have a sample which occurs after the pulse starts.
		check to see if it occurs before the pulse ends, and if so, mark
		it as a bad sample */

	while ((ts >= t1) && (ts <= t2)) {
	  bptr->badsmp[k] = sample;
	  k++;
	  sample++;
	  ts = ts + ptr->smsep;
	}
  }
  bptr->nbad = k;	/* total number of bad samples */

  /* Now set up a table for checking range interference */
  r_overlap(ptr);
return;
}

/*	This routine uses the table set up by badlags to locate which lags
	are bad for a specified range */

void FitACFCkRng(int range,int *badlag,struct FitACFBadSample *bptr,
	       struct FitPrm *ptr) {
  int sam1, sam2, i, j;
  for (i=0; i<ptr->mplgs; i++) {
	badlag[i] = 0;
	sam1 = ptr->lag[0][i]*(ptr->mpinc/ptr->smsep)
			+ range - 1;
	sam2 = ptr->lag[1][i]*(ptr->mpinc/ptr->smsep)
			+ range - 1;

	for (j=0; j<bptr->nbad; j++) {
      if ((sam1 == bptr->badsmp[j]) || (sam2 == bptr->badsmp[j]))
        badlag[i] = 1;
	  if (sam2 < bptr->badsmp[j]) break;
    }
  }

  /* This section of code is only of use to fitacf for reprocessing old
	data that used the 16 lag, 7 pulse code.  */

  if ((ptr->mplgs == 17) && (ptr->old !=0) )
	badlag[13] = 1;

  /* finally, check for range interference */
  lag_overlap(range, badlag, ptr);
  return;
}
