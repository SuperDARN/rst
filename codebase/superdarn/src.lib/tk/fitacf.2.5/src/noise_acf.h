/* noise_acf.h
   ===========
   Author: R.J.Barnes & K.Baker
*/


/*
   See license.txt
*/



double noise_acf(double mnpwr,struct FitPrm *ptr,
	         double *pwr, struct FitACFBadSample *badsmp,
		 struct complex *raw,
	         struct complex *n_acf);
