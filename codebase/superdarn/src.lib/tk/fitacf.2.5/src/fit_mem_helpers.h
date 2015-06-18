/* fit_mem_helpers.h
*/

#ifndef _FIT_MEM_HELPERS_H
#define _FIT_MEM_HELPERS_H

#include "fitblk.h"
/**
This struct holds the sums used for least square fitting
*/
typedef struct sums{
    int num_points; /*!<number of good lags*/
    double w;          /*!<sum of squared ACF powers*/
    double wk;         /*!<sum of squared ACF power multiplied by lag*/
    double wk2;        /*!<sum of squared ACF power multiplied by squared lag*/
    double *wk2_arr;   /*!<Array of squared ACF power multiplied by squared lag*/
    double wk4;        /*!<sum of squared ACF powers multiplied by lag^4*/
    double p;          /*!<sum of squared ACF powers multiplied by log power*/
    double pk;         /*!<sum of squared ACF powers multiplied by log power and lag*/
    double pk2;        /*!<sum of squared ACF powers multiplied by log power and squared lag*/
    double phi;        /*!<sum of ACf phase multiplied by squared power*/
    double kphi;       /*!<sum of ACF phase multiplied by lag and squared power*/
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
    double *ln_pwr;  		/*!<natural log of ACF power values */
    double *wt;  			/*!<sqaured ACF power values multiplied by tau*/
    double *wt2;  			/*!<squared ACF power values multiplied by tau squared*/
    double *wp;  			/*!<squared ACF power values multiplied by log power values*/
    double omega_loc;   	/*!<Phase slope*/
    double omega_err_loc;   /*!<*/
    double phi_loc;  		/*!<ACF Phase*/
    double omega_base;  	/*!<Base phase slope*/
    double omega_high;  	/*!<High end offset estimate of slope*/
    double omega_low;  		/*!<Low end offset estimate of slope*/
    double phase_sdev;  	/*!<Standard deviation in slope*/
    double phi_err;  		/*!<ACF phase error*/
    double omega_err;  		/*!<Phase slope error*/

    int *pwr_level;			/*!<Power levels marked as good or bad for each lag*/;
    int acf_stat;
}LS_DATA;

void free_ls_data(LS_DATA* ls_data);
void free_arrays(double **sum_wk2_arr, double **phi_res, double **tau, 
                    double **tau2, double **phi_k, double **w, double **pwr, 
                    double **wt, double **wt2, double **wp, int **bad_pwr);
void zero_fitrange(struct FitRange *ptr);
int allocate_ls_arrays(struct FitPrm *prm, double **sum_wk2_arr, double **phi_res, double **tau, 
                    double **tau2, double **phi_k, double **w, double **pwr, 
                    double **wt, double **wt2, double **wp, int **bad_pwr);
LS_DATA* new_least_squares_data(struct FitPrm *fitted_prms, double noise_level,
                                struct complex *acf, int acf_stat);
#endif
