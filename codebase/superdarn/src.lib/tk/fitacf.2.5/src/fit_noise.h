/* fit_noise.h
   ===========
   Author: R.J.Barnes & K.Baker
*/

/*
   See license.txt
*/



void fit_noise(struct complex *nacf,int *badlag,
               struct FitACFBadSample *badsmp,
	       double skynoise,struct FitPrm *prm,
	       struct FitRange *ptr);
