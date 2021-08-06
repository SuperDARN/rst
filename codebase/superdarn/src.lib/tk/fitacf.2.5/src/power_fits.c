/* power_fits.c
     =========
     Author: R.J.Barnes & K.Baker & P.Ponomarenko
*/

/*
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
     refactored from fit_acf.c by Pasha and AJ
*/

#include <math.h>

#include "fitblk.h"

#define determ(aa,bb,cc,dd) (aa*dd - bb*cc)

void set_sigma_error_huge(struct FitRange *ptr){
    ptr->p_s_err = HUGE_VAL;
    ptr->w_s_err = HUGE_VAL;
    ptr->sdev_s = HUGE_VAL;
}

void set_lambda_error_huge(struct FitRange *ptr){
    ptr->p_l_err = HUGE_VAL;
    ptr->w_l_err = HUGE_VAL;
    ptr->sdev_l = HUGE_VAL;
}

void do_sigma_fit(struct FitPrm *prm, struct FitRange *ptr, int *badlag, 
                    int *bad_pwr, double *w, double *tau, double *tau2, double *pwr, double sum_np, 
                    double sum_w, double t0, double sum_wk, double sum_wk2, double c_log_err,
                    double sum_p, double t2, double sum_pk, double sum_wk4, double t4, 
                    double sum_pk2) {

    double d, c_log, e2, wbar, temp;
    int k, npp;
    d = determ(sum_w,-t2*sum_wk2,t2*sum_wk2,-t4*sum_wk4);
    c_log = determ(sum_p,-t2*sum_wk2,t2*sum_pk2,-t4*sum_wk4)/d;

    ptr->p_s = c_log;

    ptr->w_s = determ(sum_w,sum_p,t2*sum_wk2,t2*sum_pk2)/d;

    if (sum_np > 3) {   
        e2 = 0.;
        wbar = 0.;
        npp = 0;
        for (k=0; k<prm->mplgs; k++)
        {
            if ((badlag[k] == 0) && (bad_pwr[k] == 0)) {
                temp = pwr[k] - (c_log - tau2[k]*t2* (ptr->w_s)); 
                e2 = e2 + w[k]*w[k]*(temp*temp);
                wbar = wbar + w[k];
                npp++;
            }
        }

        wbar = wbar/npp;
        ptr->sdev_s = sqrt(e2/sum_w*npp/(npp - 2));

        if ((sum_w*sum_wk4 - sum_wk2*sum_wk2) <= 0.0 ) {
            set_sigma_error_huge(ptr);
        } else {
            c_log_err = ptr->sdev_s * wbar *
                sqrt(sum_wk4/(sum_w*sum_wk4 - sum_wk2*sum_wk2));
            ptr->p_s_err = c_log_err;       
            ptr->w_s_err = ptr->sdev_s * wbar * 
                sqrt(sum_w/(t4*(sum_w*sum_wk4 - sum_wk2*sum_wk2)));
        }
    } else {
        set_sigma_error_huge(ptr);
    }   
}

void do_lambda_fit(struct FitPrm *prm, struct FitRange *ptr, int *badlag, 
                    int *bad_pwr, double *w, double * tau, double *pwr, double sum_np, 
                    double sum_w, double t0, double sum_wk, double sum_wk2, double c_log_err,
                    double sum_p, double t2, double sum_pk) {

    double d, c_log, e2, wbar, temp;
    int k, npp;
    
    d = determ(sum_w,-t0*sum_wk,t0*sum_wk,-t2*sum_wk2);
    c_log = determ(sum_p,-t0*sum_wk,t0*sum_pk,-t2*sum_wk2)/d;

    ptr->p_l = c_log;

    ptr->w_l = determ(sum_w,sum_p,t0*sum_wk,t0*sum_pk)/d;

    if (sum_np > 3) {
        e2 = 0.;
        wbar = 0.;
        npp = 0;
        for (k=0; k<prm->mplgs; k++)
            if ((badlag[k] == 0) && (bad_pwr[k] == 0)) {
                temp = pwr[k] - (c_log - tau[k]*t0* (ptr->w_l));
                e2 = e2 + w[k]*w[k]*(temp*temp);
                wbar = wbar + w[k];
                npp++;
            }
        wbar = wbar/npp;
        ptr->sdev_l = sqrt(e2/sum_w*npp/(npp - 2));

        if ((sum_w*sum_wk2 - sum_wk*sum_wk) <=0) {
            set_lambda_error_huge(ptr);
        } else {
            c_log_err = ptr->sdev_l * wbar *
                sqrt(sum_wk2/(sum_w*sum_wk2 - sum_wk*sum_wk));
            ptr->p_l_err = c_log_err;
            ptr->w_l_err = ptr->sdev_l * wbar *
                sqrt(sum_w/(t2*(sum_w*sum_wk2 - sum_wk*sum_wk)));
        }
    } else {
        set_lambda_error_huge(ptr);
    }
}

