/* omega_guess.h
   =============
   Author: R.J.Barnes & K.Baker
*/

/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.2-beta-4-g32f7302.txt for more information.
 
 
 
*/



double omega_guess(struct complex *acf,double *tau,
	           int *badlag,double *phi_res,
                   double *omega_err,int mpinc,int mplgs);
