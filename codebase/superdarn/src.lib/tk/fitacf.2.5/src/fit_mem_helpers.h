/* fit_mem_helpers.h
*/

#ifndef _FIT_MEM_HELPERS_H
#define _FIT_MEM_HELPERS_H



void free_arrays(double **sum_wk2_arr, double **phi_res, double **tau, 
                    double **tau2, double **phi_k, double **w, double **pwr, 
                    double **wt, double **wt2, double **wp, int **bad_pwr);
void zero_fitrange(struct FitRange *ptr);
int allocate_ls_arrays(struct FitPrm *prm, double **sum_wk2_arr, double **phi_res, double **tau, 
                    double **tau2, double **phi_k, double **w, double **pwr, 
                    double **wt, double **wt2, double **wp, int **bad_pwr);
#endif
