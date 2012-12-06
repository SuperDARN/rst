/* fit_acf.h
   =========
   Author: R.J.Barnes & K.Baker
*/

/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.2-beta-4-g32f7302.txt for more information.
 
 
 
*/




int fit_acf (struct complex *acf,int range,int *badlag,
	     struct FitACFBadSample *badsmp,int lag_lim,
	     struct FitPrm *prm,
	     double noise_lev_in,char xflag,double xomega,
             struct FitRange *ptr,int print);
