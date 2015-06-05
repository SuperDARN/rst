/* power_fits.h
*/

#ifndef _POWER_FITS_H
#define _POWER_FITS_H

#include "fit_mem_helpers.h"

void set_sigma_error_huge(struct FitRange *fit_range);
void set_lambda_error_huge(struct FitRange *fit_range);
/*void do_sigma_fit(struct FitPrm *fitted_prms, struct FitRange *fit_range, int *badlag, 
                    int *bad_pwr, double *w, double *tau, double *tau2, double *pwr, double sum_np, 
                    double sum_w, double t0, double sum_wk, double sum_wk2, double c_log_err,
                    double sum_p, double t2, double sum_pk, double sum_wk4, double t4, 
                    double sum_pk2);*/
void do_sigma_fit(struct FitPrm *fitted_prms,struct FitRange *fit_range,
                  int *badlag, LS_DATA *ls_data);                    
/*void do_lambda_fit(struct FitPrm *fitted_prms, struct FitRange *fit_range, int *badlag, 
                    int *bad_pwr, double *w, double * tau, double *pwr, double sum_np, 
                    double sum_w, double t0, double sum_wk, double sum_wk2, double c_log_err,
                    double sum_p, double t2, double sum_pk);*/
void do_lambda_fit(struct FitPrm *fitted_prms, struct FitRange *fit_range,
				   int *badlag,LS_DATA *ls_data);
#endif
