/* linearfit.c
   ===========
   Author: Angeline G. Burrell - NRL - 2021
   This is a U.S. government work and not under copyright protection in the U.S.

   This file is part of the Radar Software Toolkit (RST).

   Disclaimer: RST is licensed under GPL v3.0. Please visit 
               <https://www.gnu.org/licenses/> to see the full license

   Modifications:

*/

#include <stdio.h>
#include <math.h>

#include "linearfit.h"

/**
 * @brief perform a linear regression
 *
 * @params[in] x   - independent variable array
 *             y   - dependent variable array
 *             sig - error of the dependent variable array
 *             num - number of variables in x, y, and sig
 *             mwt - boolean, if true sig is used to calculate weights
 *
 * @params[out] intercept     - pointer to the fitted intercept
 *              sig_intercept - pointer to the error of the intercept
 *              slope         - pointer to the fitted slope
 *              sig_slope     - pointer to the error of the slope
 *              chi2          - pointer to the Chi-squared value
 *              q             - pointer to the gamma function output
 *
 * @references Numerical Recipes in C, 2nd Edition
 **/

int linear_regression(float x[], float y[], float sig[], int num, int mwt,
		      float *intercept, float *sig_intercept, float *slope,
		      float *sig_slope, float *chi2, float *q)
{
  int n, status = 1;

  float wt, t, sxoss, sx, sy, st2, ss, sigdat;

  sx = sy = st2 = 0.0;
  *slope  = 0.0;

  if(mwt)
    {
      for(ss = 0.0, n = 0; n < num; n++)
	{
	  if(sig[n] == 0.0) sig[n] = MINSIG;

	  wt  = 1.0 / (sig[n] * sig[n]);
	  ss += wt;
	  sx += x[n] * wt;
	  sy += y[n] * wt;
	}
    }
  else
    {
      for(ss = num, n = 0; n < num; n++)
	{
	  sx += x[n];
	  sy += y[n];
	}
    }

  sxoss = sx / ss;

  for(n = 0; n < num; n++)
    {
      t       = (mwt) ? (x[n] - sxoss) / sig[n] : x[n] - sxoss;
      st2    += t * t;
      *slope += t * y[n];
    }

  *slope        /= st2;
  *intercept     = (sy - sx * (*slope)) / ss;
  *sig_intercept = sqrt((1.0 + sx * sx / (ss * st2)) / ss);
  *sig_slope     = sqrt(1.0 / st2);

  /* Calculate Chi2 */

  *chi2 = 0.0;
  *q    = 1.0;

  if(mwt == 0)
    {
      for(n = 0; n < num; n++)
	*chi2 += SQR(y[n] - (*intercept) - (*slope) * x[n]);

      sigdat          = sqrt((*chi2) / (float)(num - 2));
      *sig_slope     *= sigdat;
      *sig_intercept *= sigdat;
      status = 0;
    }
  else
    {
      for(n = 0; n < num; n++)
	*chi2 += SQR((y[n] - (*intercept) - (*slope) * x[n]) / sig[n]);

      if(num > 2) *q = (float)gammaq(0.5 * (double)(num - 2),
				     0.5 * (double)(*chi2));
    }

  return(status);
}
