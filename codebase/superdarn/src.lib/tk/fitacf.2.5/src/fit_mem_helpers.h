/* fit_mem_helpers.h
*/

#ifndef _FIT_MEM_HELPERS_H
#define _FIT_MEM_HELPERS_H


/**
This struct holds the sums used for least square fitting
*/
typedef struct sums{
    double num_points; /*!<number of good lags*/
    double w;          /*!<sum of squared ACF powers*/
    double wk;         /*!<sum of squared ACF power multiplied by lag*/
    double wk2;        /*!<sum of squared ACF power multiplied by squared lag*/
    double *wk2_arr;   /*!<*/
    double wk4;        /*!<sum of squared ACF powers multiplied by lag^4*/
    double p;          /*!<sum of squared ACF powers multiplied by log power*/
    double pk;         /*!<sum of squared ACF powers multiplied by log power and lag*/
    double pk2;        /*!<sum of squared ACF powers multiplied by log power and squared lag*/
    double phi;        /*!<*/
    double kphi;       /*!<*/
}SUMS;


/**
This struct holds all the data necessary for least square fitting*/
typedef struct least_squares_data{
    SUMS *sums;

    double t0;  	 		/*!<basic lag length (mpinc) in microseconds*/
    double t2;  	 		/*!<basic lag length squared*/
    double t4;  	 		/*!<basic lag length to the fourth*/
    double *phi_res; 		/*!<*/
    double *tau;  			/*!<ACF lags expressed as quantity of basic lags*/
    double *tau2;  			/*!<ACF lags expressed as quantity of basic lags squared*/
    double *phi_k;  		/*!<*/
    double *w;  			/*!<ACF power values*/
    double *ln_pwr;  			/*!<natural log of ACF power values */
    double *wt;  			/*!<sqaured ACF power values multiplied by tau*/
    double *wt2;  			/*!<squared ACF power values multiplied by tau squared*/
    double *wp;  			/*!<sqaured ACF power values multiplied by log power values*/
    double omega_loc;   	/*!<*/
    double omega_err_loc;   /*!<*/
    double phi_loc;  		/*!<*/
    double omega_base;  	/*!<*/
    double omega_high;  	/*!<*/
    double omega_low;  		/*!<*/
    double phase_sdev;  	/*!<*/
    double phi_err;  		/*!<*/
    double omega_err;  		/*!<*/

    int *pwr_level			/*!<Power levels marked as good or bad for each lag*/;
}LS_DATA;

void free_ls_data(LS_DATA* ls_data);
void free_arrays(double **sum_wk2_arr, double **phi_res, double **tau, 
                    double **tau2, double **phi_k, double **w, double **pwr, 
                    double **wt, double **wt2, double **wp, int **bad_pwr);
void zero_fitrange(struct FitRange *ptr);
int allocate_ls_arrays(struct FitPrm *prm, double **sum_wk2_arr, double **phi_res, double **tau, 
                    double **tau2, double **phi_k, double **w, double **pwr, 
                    double **wt, double **wt2, double **wp, int **bad_pwr);
LS_DATA* new_least_squares_data(struct FitPrm *fitted_prms);
#endif
