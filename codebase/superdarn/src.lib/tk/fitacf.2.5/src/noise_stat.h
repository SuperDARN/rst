/* noise_stat.h
   ============
   Author: R.J.Barnes & K.Baker & P.Ponomarenko
*/


/*
   See license.txt
 */



double lag_power(struct complex *a);

double noise_stat(double mnpwr,struct FitPrm *ptr,
                  struct FitACFBadSample *badsmp,
		  struct complex *acf);
