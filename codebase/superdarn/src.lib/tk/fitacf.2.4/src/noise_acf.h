/* noise_acf.h
   ===========
   Author: R.J.Barnes & K.Baker
*/


/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.2-beta-4-g32f7302.txt for more information.
 
 
 
*/



double noise_acf(double mnpwr,struct FitPrm *ptr,
	         double *pwr, struct FitACFBadSample *badsmp,
		 struct complex *raw,
	         struct complex *n_acf);
