/* do_phase_fit.h
   ==============
   Author: K.Baker
*/

/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.2-beta-4-g32f7302.txt for more information.
 
 
 
*/



int do_phase_fit(double omega_guess, 
		  char xflag, 
		  int mplgs,
		  struct complex *acf,
		  double *tau,
		  double *w,
		  double *sum_wk2_arr,
		  double *phi_res,
		  int *badlag,
		  double t0,
		  double sum_w,
		  double sum_wk,
		  double sum_wk2,
		  
		  double *omega,
		  double *phi0,
		  double *sdev,
		  double *phi0_err,
		  double *omega_err);

