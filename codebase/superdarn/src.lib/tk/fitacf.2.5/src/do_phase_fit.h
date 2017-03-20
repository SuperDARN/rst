/* do_phase_fit.h
   ==============
   Author: K.Baker
*/

/*
   See license.txt
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

