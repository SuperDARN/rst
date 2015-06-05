/* power_fits.c
     =========
     refactored from fit_acf.c by Pasha and AJ
     Author: R.J.Barnes & K.Baker & P.Ponomarenko
*/

/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
    
*/

#include <math.h>

#include "fitblk.h"
#include "fit_mem_helpers.h"
#include "badlags.h"


/*
Preprocessor macro expansion for a determinant
*/
#define determ(aa,bb,cc,dd) (aa*dd - bb*cc)

/**
If a sigma fit cannot be performed with enough accuracy then
the error levels are set to HUGE_VAL
*/
void set_sigma_error_huge(struct FitRange *fit_range){
    fit_range->p_s_err = HUGE_VAL;
    fit_range->w_s_err = HUGE_VAL;
    fit_range->sdev_s = HUGE_VAL;
}


/**
If a lambda fit cannot be performed with enough accuracy then
the error levels are set to HUGE_VAL
*/
void set_lambda_error_huge(struct FitRange *fit_range){
    fit_range->p_l_err = HUGE_VAL;
    fit_range->w_l_err = HUGE_VAL;
    fit_range->sdev_l = HUGE_VAL;
}

/**
This function performs the best fit for power and spectral width for 
a given range using the sigma fit technique
*/
void sigma_power_and_width_fit_for_range(LS_DATA *ls_data, struct FitRange *fit_range){
    double d, w, c_log;
    double d_aa, d_bb, d_cc, d_dd;
    double c_aa, c_bb, c_cc, c_dd;
    double w_aa, w_bb, w_cc, w_dd;

    d_aa = ls_data->sums->w;
    d_bb = -1 * ls_data->t2 * ls_data->sums->wk2;
    d_cc = ls_data->t2 * ls_data->sums->wk2;
    d_dd = -1 * ls_data->t4 * ls_data->sums->wk4;

    d = determ(d_aa,d_bb,d_cc,d_dd);

    c_aa = ls_data->sums->p;
    c_bb = -1 * ls_data->t2 * ls_data->sums->wk2;
    c_cc = ls_data->t2 * ls_data->sums->pk2;
    c_dd = -1 * ls_data->t4 * ls_data->sums->wk4;

    c_log = determ(c_aa,c_bb,c_cc,c_dd)/d;

    w_aa = ls_data->sums->w;
    w_bb = ls_data->sums->p;
    w_cc = ls_data->t2 * ls_data->sums->wk2;
    w_dd = ls_data->t2 * ls_data->sums->pk2;

    w = determ(w_aa,w_bb,w_cc,w_dd);

    fit_range->p_s = c_log;

    fit_range->w_s = w/d;
}

/**
This function provides the uncertainty in the determination of 
power and spectral width from the sigma fit
*/
void set_sigma_fit_errors_for_range(struct FitPrm *fitted_prms,struct FitRange *fit_range,
                                    int *lag, LS_DATA *ls_data){
    double e2, wbar,log_pwr_err, spectral_err, constant_a, constant_b;
    int k, npp;

    if (ls_data->sums->num_points > 3) {   
        e2 = 0.;
        wbar = 0.;
        npp = 0;

        for (k=0; k<fitted_prms->mplgs; k++){

            if ((lag[k] == GOOD) && (ls_data->pwr_level[k] == GOOD)) {
                constant_a = fit_range->p_s - ls_data->tau2[k] * ls_data->t2 * (fit_range->w_s);
                constant_b = ls_data->ln_pwr[k] - constant_a; 

                e2 += ls_data->w[k] * ls_data->w[k] * (constant_b * constant_b);

                wbar += ls_data->w[k];

                npp++;
            }
        }

        
        wbar = wbar/npp;
        constant_a = e2/ls_data->sums->w/(npp - 2);
        fit_range->sdev_s = sqrt(constant_a);

        if ((ls_data->sums->w * ls_data->sums->wk4 - ls_data->sums->wk2 * ls_data->sums->wk2) <= 0.0 ) {
            set_sigma_error_huge(fit_range);
        } 
        else{

            constant_a = ls_data->sums->w * ls_data->sums->wk4 - ls_data->sums->wk2 * ls_data->sums->wk2;
            log_pwr_err = fit_range->sdev_s * wbar * sqrt(ls_data->sums->wk4/constant_a);
            fit_range->p_s_err = log_pwr_err;

            constant_a = ls_data->sums->w * ls_data->sums->wk4 - ls_data->sums->wk2 * ls_data->sums->wk2;
            constant_b = ls_data->t4 * constant_a;
            spectral_err = fit_range->sdev_s * wbar * sqrt(ls_data->sums->w/constant_b);
            fit_range->w_s_err = spectral_err;
        }
    } else {
        set_sigma_error_huge(fit_range);
    }   
   
}

/**
This function performs the best fit for power and spectral width for 
a given range using the lambda fit technique
*/
void lambda_power_and_width_fit_for_range(LS_DATA *ls_data, struct FitRange *fit_range){
    double d, w, c_log;
    double d_aa, d_bb, d_cc, d_dd;
    double c_aa, c_bb, c_cc, c_dd;
    double w_aa, w_bb, w_cc, w_dd;

    d_aa = ls_data->sums->w;
    d_bb = -1 * ls_data->t0 * ls_data->sums->wk;
    d_cc = ls_data->t0 * ls_data->sums->wk;
    d_dd = -1 * ls_data->t2 * ls_data->sums->wk2;

    d = determ(d_aa,d_bb,d_cc,d_dd);

    c_aa = ls_data->sums->p;
    c_bb = -1 * ls_data->t0 * ls_data->sums->wk;
    c_cc = ls_data->t0 * ls_data->sums->pk;
    c_dd = -1 * ls_data->t2 * ls_data->sums->wk2;

    c_log = determ(c_aa,c_bb,c_cc,c_dd)/d;

    w_aa = ls_data->sums->w;
    w_bb = ls_data->sums->p;
    w_cc = ls_data->t0 * ls_data->sums->wk;
    w_dd = ls_data->t0 * ls_data->sums->pk;

    w = determ(w_aa,w_bb,w_cc,w_dd);

    fit_range->p_l = c_log;

    fit_range->w_l = w/d;
}

/**
This function provides the uncertainty in the determination of 
power and spectral width from the lambda fit
*/
void set_lambda_fit_errors_for_range(struct FitPrm *fitted_prms,struct FitRange *fit_range,
                                    int *lag, LS_DATA *ls_data){
    double e2, wbar,log_pwr_err, spectral_err, constant_a, constant_b;
    int k, npp;

    if (ls_data->sums->num_points > 3) {
        e2 = 0.;
        wbar = 0.;
        npp = 0;

        for (k=0; k<fitted_prms->mplgs; k++)
            if ((lag[k] == GOOD) && (ls_data->pwr_level[k] == GOOD)) {
                constant_a = fit_range->p_l - ls_data->tau[k] * ls_data->t0 * fit_range->w_l;
                constant_b = ls_data->ln_pwr[k] - constant_a;
                e2 = e2 + ls_data->w[k] * ls_data->w[k] * (constant_b * constant_b);
                wbar = wbar + ls_data->w[k];
                npp++;
            }

        wbar = wbar/npp;
        constant_a = e2/ls_data->sums->w/(npp - 2);
        fit_range->sdev_l = sqrt(constant_a);

        if ((ls_data->sums->w * ls_data->sums->wk2 - ls_data->sums->wk * ls_data->sums->wk) <=0) {
            set_lambda_error_huge(fit_range);
        } else {
            constant_a = ls_data->sums->w * ls_data->sums->wk2 - ls_data->sums->wk * ls_data->sums->wk;
            log_pwr_err = fit_range->sdev_l * wbar * sqrt(ls_data->sums->wk2/constant_a);
            fit_range->p_l_err = log_pwr_err;

            constant_a = ls_data->t2 * (ls_data->sums->w * ls_data->sums->wk2 - ls_data->sums->wk * ls_data->sums->wk);
            spectral_err = fit_range->sdev_l * wbar * sqrt(ls_data->sums->w/constant_a);
            fit_range->w_l_err = spectral_err ;
        }
    } else {
        set_lambda_error_huge(fit_range);
    }

}

/**
Performs a sigma power fit and its respective error calculations
*/
void do_sigma_fit(struct FitPrm *fitted_prms,struct FitRange *fit_range,
                  int *lag, LS_DATA *ls_data) {

    sigma_power_and_width_fit_for_range(ls_data,fit_range);

    set_sigma_fit_errors_for_range(fitted_prms,fit_range,lag,ls_data);

}

/**
Performs a lamda power fit and its respective error calculations
*/
void do_lambda_fit(struct FitPrm *fitted_prms, struct FitRange *fit_range,
                   int *lag,LS_DATA *ls_data) {


    lambda_power_and_width_fit_for_range(ls_data,fit_range);

    set_lambda_fit_errors_for_range(fitted_prms,fit_range,lag,ls_data);

}

