/* fit_noise.h
   ===========
   Author: R.J.Barnes & K.Baker
*/

/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.2-beta-4-g32f7302.txt for more information.
 
 
 
*/



void fit_noise(struct complex *nacf,int *badlag,
               struct FitACFBadSample *badsmp,
	       double skynoise,struct FitPrm *prm,
	       struct FitRange *ptr);
