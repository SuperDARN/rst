/* fit_acf.h
   =========
   Author: R.J.Barnes & K.Baker
*/

/*
   See license.txt
*/




int fit_acf (struct complex *acf,int range,int *badlag,
	     struct FitACFBadSample *badsmp,int lag_lim,
	     struct FitPrm *prm,
	     double noise_lev_in,char xflag,double xomega,
             struct FitRange *ptr);
