/* fit_acf.c
     =========
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
#include <stdio.h>
#include <stdlib.h>

#include "rmath.h"
#include "badsmp.h"
#include "fitblk.h"


#include "acf_preproc.h" 
#include "calc_phi_res.h"
#include "omega_guess.h"
#include "badlags.h"
#include "more_badlags.h"
#include "do_phase_fit.h"  
#include "power_fits.h"    
#include "fit_mem_helpers.h"



/**
Prepares data for ACF fitting and calls procedures for phase and power fitting
*/
int fit_acf (struct complex *acf,int range,
                int *lag,struct FitACFBadSample *badsmp,int lag_lim,
                struct FitPrm *fitted_prms,
                double noise_lev_in,char xflag,double xomega,
                struct FitRange *fit_range) {

/*    double sum_np,sum_w,sum_wk,sum_wk2,*sum_wk2_arr=NULL,sum_wk4,
            sum_p,sum_pk,sum_pk2,sum_phi,sum_kphi, t0,t2,t4,*phi_res=NULL;*/
    int j, npp, s = 0, last_good, status; /**bad_pwr = NULL; */   
    long k;
  /*  double *tau=NULL, *tau2=NULL, *phi_k=NULL, *w=NULL, *pwr=NULL,
            *wt=NULL, *wt2=NULL, *wp=NULL, 
            omega_loc, omega_err_loc, phi_loc, noise_lev, omega_base, 
            omega_high, omega_low, phase_sdev, phi_err, omega_err;*/
    double c_log,c_log_err, noise_lev;

    LS_DATA *ls_data = NULL;
    FILE *fp;

    /*  The following variables have been added for version 2.0 of cfitacf */
    double /*P0, */ P0n;   /* this is the power level where the acf levels off */

    /* the following array has been added to support preprocessing of the
     acf */
    int acf_stat=ACF_UNMODIFIED;

    /* ----------------End of declarations ----------------------------------*/
 
    /*if the lag 0 power is less than the noise level,
        then assign label 3 to badlag and return zeros in the
        fitrange object*/
    if (cabs(acf[0]) < noise_lev_in) {
        for (j=0; j<fitted_prms->mplgs; j++) {
            lag[j]=3;
        }
        /*set the fit structure to zeroes*/
        zero_fitrange(fit_range);
        return 2;
    }

    /*Creating new data structure for least square fitting*/
    if( (ls_data = new_least_squares_data(fitted_prms)) == NULL){
        free_ls_data(ls_data);
        fprintf(stderr,"Error allocating new least squares data\n");
        return -1;
    }
    

    /* initialize the table of abs(acf[k]) and log(abs(acf[k])) */
    FitACFCkRng(range, lag, badsmp, fitted_prms);

    /* Save the original ACF in a new variable so we can try some
         preprocessing on it.

        for (k=0; k < fitted_prms->mplgs; k++) {
            orig_acf[k].x = acf[k].x;
            orig_acf[k].y = acf[k].y;
        }
    */

    /* 
        This next statement provides a hook for a routine to pre-process
        the ACF and return a modified ACF that will actually be fitted.

        The function acf_preproc should return a 0 if no change was made
        and a non-zero value otherwise - the actual non-zero value may be
        used to provide information about the change that was made.  Specifically
        the following values should be defined:
        ACF_UNMODIFIED  the ACF was unchanged
        ACF_GROUND_SCAT the preprocessing indicates the ACF is ground
                                scatter.
        ACF_ION_SCAT    the preprocessing indicates the ACF is Ionospheric
        ACF_MIXED_SCAT  the preprocessing indicates a mixture of
                                ionospheric and ground scatter.

        The original acf will be in the array "orig_acf" and the modified
        acf will be in "acf".

        To write an acf_preprocessor you must use the calling convention below.
        The revised acf will be returned in the array acf.  You must also provide
        the value of the noise_level (noise_lev_in), the range number, the badlag
        table, and the number of lags
    */

    noise_lev = noise_lev_in;
    /*
    if (noise_lev != 0.0) acf_stat = acf_preproc (acf, orig_acf, &noise_lev,
            range, badlag, fitted_prms->mplgs);
    */

    /*
        fill in arrays for tau, tau^2, and power
        and then mark badlags with power below the noise level
    */
    for (k=0; k<fitted_prms->mplgs; k++) {
        ls_data->tau[k] = fitted_prms->lag[1][k] - fitted_prms->lag[0][k];
        ls_data->tau2[k] = ls_data->tau[k] * ls_data->tau[k];
        ls_data->w[k] = cabs(acf[k]); /* w[k] = cabs(acf[k])- noise_lev; */
        if (ls_data->w[k] <= noise_lev) {
            ls_data->w[k] = 0.1; /* if (w[k] <= 0.0) w[k] = 0.1; */
        }
    }

    /* 
        Check if lag 0 power minus the fluctuation level is above the noise level.
        The amount to be subtracted is P(0)/sqrt(nave)
        which is the approximate expectation value of the power level after the
        ACF has decorrelated. 

        [ To derive this, determine the expectation value of
        P**2 = R(tau)*conj(R(tau))]
    */

    P0n = ls_data->w[0]/sqrt((double) fitted_prms->nave);
    if ((ls_data->w[0] - P0n) < noise_lev) {
        free_ls_data(ls_data);
        return 2; 
    } 
    /* give up if left over pwr is too low */


    /*  identify any additional bad lags */
    ls_data->sums->num_points = more_badlags(ls_data->w, lag, noise_lev, fitted_prms->mplgs,fitted_prms->nave);

    fit_range->nump = (char) ls_data->sums->num_points;

    /*  We must have at least lag_lim good lags */
    if (ls_data->sums->num_points < lag_lim) {
        free_ls_data(ls_data);
        return 4;
    }

    /* this is required to make logs ok */
    if (noise_lev <= 0.0) {
        noise_lev = 0.1; 
    }
    
    /* This is to remove background delta-correlated noise from lag 0 power (version 2.0)*/
    ls_data->w[0] = ls_data->w[0] - fitted_prms->noise; 

    /* OK, now we have determined the good lags for the phase fit.  
         Now subtract of P0n from the power profile */

    /* calculate the power values for each lag.  'w' is the linear power.
         wt is the power times the lag.  wt2 is power times lag**2.  
         pwr is the log of the power. wp is the linear power times the log of
         the power.  The items that involve the linear power are all parts of 
         the least squares fits with the weighting done by the linear power. */
    for (k=0; k<fitted_prms->mplgs; k++) {
        if (ls_data->w[k] <= P0n) {
            ls_data->w[k] = 0.1; /* if (w[k] <= 0.0) w[k] = 0.1; */
        }
        ls_data->wt[k] = ls_data->w[k] * ls_data->w[k] * ls_data->tau[k];
        ls_data->wt2[k] = ls_data->wt[k] * ls_data->tau[k];
        ls_data->ln_pwr[k] = log(ls_data->w[k]);
        ls_data->wp[k] = ls_data->w[k] * ls_data->w[k] * ls_data->ln_pwr[k];
    }

    /* we now have to check to see how many additional bad lags have been
         introduced by subtracting off P0n. */
    for (k=0, npp=0; k < fitted_prms->mplgs; k++) {
        if (ls_data->w[k] < noise_lev + P0n && (lag[k]==GOOD)) ls_data->pwr_level[k] = BAD; 
        /* if (w[k] < noise_lev && !badlag[k]) bad_pwr[k] = 1; */
        else ls_data->pwr_level[k] = GOOD;
        if ( (lag[k] == GOOD) && (ls_data->pwr_level[k] == GOOD)) ++npp;
    }

    /* set the sums to initial values*/

    ls_data->sums->num_points = 1;
    ls_data->sums->w = ls_data->w[0] * ls_data->w[0];
    ls_data->sums->wk = 0;
    ls_data->sums->wk2 = 0;
    ls_data->sums->wk2_arr[0] = 0;
    ls_data->sums->wk4 = 0;
    ls_data->sums->p = ls_data->w[0] * ls_data->w[0] * ls_data->ln_pwr[0];
    ls_data->sums->pk = 0;
    ls_data->sums->pk2 = 0;
    ls_data->sums->kphi = 0;

    ls_data->phi_loc = atan2(acf[0].y, acf[0].x);
    ls_data->t0 =  fitted_prms->mpinc * 1.0e-6;
    ls_data->t2 = ls_data->t0 * ls_data->t0;
    ls_data->t4 = ls_data->t2 * ls_data->t2;


    /* calculate all the residual phases */
    /* if calc_phi_res returns a bad status abort the fit */
    s = calc_phi_res(acf, lag, ls_data->phi_res, fitted_prms->mplgs);
    if (s != 0) {
        free_ls_data(ls_data);
        return 2;
    }


    if (!xflag) {
        /*if it's a regular fit (not XCF)*/
        if (acf_stat == ACF_GROUND_SCAT) ls_data->omega_loc = 0.0;
        else ls_data->omega_loc = omega_guess(acf, ls_data->tau, lag, ls_data->phi_res, &ls_data->omega_err_loc,fitted_prms->mpinc,fitted_prms->mplgs);
        ls_data->phi_k[0] = 0;
        ls_data->sums->phi = 0;
    } else {
        /*if it's an XCF fit (not ACF)*/
        ls_data->phi_k[0] = ls_data->phi_loc;
        ls_data->sums->phi = ls_data->phi_loc * ls_data->w[0] * ls_data->w[0];
        ls_data->omega_loc = xomega;
    }


    /*  The preliminaries are now over.  
    Now start the fitting process */

    /* first, calculate the sums needed for the phase fit */
    for (k=1; k<fitted_prms->mplgs; k++) {
        if (lag[k] != GOOD) {
            ls_data->sums->wk2_arr[k] = ls_data->sums->wk2_arr[k-1];
            continue;
        }
        ls_data->sums->w = ls_data->sums->w + ls_data->w[k] * ls_data->w[k];
        ls_data->sums->num_points = ls_data->sums->num_points + 1;
        ls_data->sums->wk = ls_data->sums->wk + ls_data->w[k] * ls_data->w[k] * ls_data->tau[k];
        ls_data->sums->wk2 = ls_data->sums->wk2 + ls_data->wt2[k];
        ls_data->sums->wk2_arr[k] = ls_data->sums->wk2;
    }

    /* Now do the phase fit using the best initial guess for omega */

 
    status = do_phase_fit (ls_data->omega_loc, xflag, fitted_prms->mplgs, acf, ls_data->tau,
             ls_data->w, ls_data->sums->wk2_arr, ls_data->phi_res, lag, ls_data->t0,
             ls_data->sums->w, ls_data->sums->wk, ls_data->sums->wk2,
             &ls_data->omega_base, &ls_data->phi_loc, &ls_data->phase_sdev,
             &ls_data->phi_err, &ls_data->omega_err);

    fit_range->phi0 = ls_data->phi_loc;
    fit_range->v = ls_data->omega_base;
    fit_range->sdev_phi = ls_data->phase_sdev;
    fit_range->phi0_err = ls_data->phi_err;
    fit_range->v_err = ls_data->omega_err;

    /* check the status of the phase fit to see if it was actually OK.  
         if not, set error bars to HUGE_VAL */

    if (status != 0) {
        fit_range->sdev_phi = HUGE_VAL;
        fit_range->v_err = HUGE_VAL;
        if (xflag) fit_range->phi0_err = HUGE_VAL;
    }
    
    /* OK, we now have our baseline value for omega.  Now re-do the
         phase fit, but using omega_loc + omega__err_loc. */


    if (!xflag && (status == 0)) {
        status = do_phase_fit (ls_data->omega_loc + ls_data->omega_err_loc,
                                xflag, fitted_prms->mplgs, acf, ls_data->tau,
                                ls_data->w, ls_data->sums->wk2_arr, ls_data->phi_res, lag, ls_data->t0,
                                ls_data->sums->w, ls_data->sums->wk, ls_data->sums->wk2,
                                &ls_data->omega_high, &ls_data->phi_loc, &ls_data->phase_sdev,
                                &ls_data->phi_err, &ls_data->omega_err);

        status = do_phase_fit (ls_data->omega_loc - ls_data->omega_err_loc, 
                                xflag, fitted_prms->mplgs, acf, ls_data->tau,
                                ls_data->w, ls_data->sums->wk2_arr, ls_data->phi_res, lag, ls_data->t0,
                                ls_data->sums->w, ls_data->sums->wk, ls_data->sums->wk2,
                                &ls_data->omega_low, &ls_data->phi_loc, &ls_data->phase_sdev,
                                &ls_data->phi_err, &ls_data->omega_err);

        /* if the difference between the high and low values of omega
             is greater than the error estimate of the original fit,
             we will use the original fit as our best guess for the
             velocity, but we'll set the error to be the difference between
             the high and low values.  Actually, at this point we should have
             non-symmetric error bar, but the file format has no provision 
             for that. */

        if (fabs(ls_data->omega_high - ls_data->omega_low) >= 2*fit_range->v_err) {
            fit_range->v = ls_data->omega_base;
            fit_range->v_err = fabs(ls_data->omega_high - ls_data->omega_low);
        }
    }
    

    /* POWER FITS:  We now turn to the power fits.  The sums have to be
    partially redone, since we have subtracted P0n. */

    /* We are now faced with the question of what to do if we don't have enough 
    lags left to do a fit.  we can't abaondon the data because the phase fit is
    actually ok.  we have to have at least 3 points to do the fit and estimate 
    an error on the fit.

    If we don't have at least 3 good points, then simply set the lamda and
    sigma powers both to the power_lag0 level.  If there are only 2 good points
    then calculate the value of sigma and lamda, but set the error estimate
    to HUGE_VAL.

    If we end up with only lag-0 being good, then flag the width estimate
    by setting it to a large negative value.

*/

    if (npp < 3) {
        c_log = ls_data->ln_pwr[0];

        /* if c_log < 0 it means that after subtracting the noise and P0n,
         the result is less than 1.0.  This must really be pretty meaningless
         It shouldn't even be possible since we have supposedly already checked
         this at the beginning. */

        if (c_log < 0 ) {
            free_ls_data(ls_data);
           /* free_arrays(&sum_wk2_arr, &phi_res, &tau, &tau2, 
                        &phi_k, &w, &pwr, &wt, &wt2, &wp, &bad_pwr);*/
            return 2;
        }

        fit_range->p_l = c_log;
        fit_range->p_s = c_log;

        /* find the last good lag */
        last_good = -1;
        for (k= 0; k < fitted_prms->mplgs; k++) if (lag[k] == GOOD) last_good = k;

        /* if there are no good lags, or only lag-0 is good, set the width
             to a high negative value, by setting the last_good lag to 1
             */

        if (last_good <=0 ) {
            fit_range->w_l = -9999.0;
            fit_range->w_s = -9999.0;
            set_lambda_error_huge(fit_range);
            set_sigma_error_huge(fit_range);
        } else {
            /* now calculate the width as the lag-0 power divided by the
             time to the last good lag. */

            fit_range->w_l = c_log/(ls_data->tau[last_good] * ls_data->t0);
            fit_range->w_s = c_log/(ls_data->tau2[last_good] * ls_data->t2);

            /* set the errors to the maximum value */
            set_lambda_error_huge(fit_range);
            set_sigma_error_huge(fit_range);
        }
    } else {
        /*  Calculate the sums that were not used in the phase fit */
        for (k=1; k < fitted_prms->mplgs; k++) {
            if (lag[k] != GOOD || ls_data->pwr_level[k] == BAD) {
                continue;
            }
            ls_data->sums->p = ls_data->sums->p + ls_data->wp[k];
            ls_data->sums->pk = ls_data->sums->pk + ls_data->ln_pwr[k] * ls_data->wt[k];
            ls_data->sums->pk2 = ls_data->sums->pk2 + ls_data->ln_pwr[k] * ls_data->wt2[k];
            ls_data->sums->wk4 = ls_data->sums->wk4 + ls_data->wt2[k] * ls_data->tau2[k];
        }

        /* Now adjust the sums that were used in the phase fit, but that
             have changed because of additional bad lags */

        for (k=1; k< fitted_prms->mplgs; k++) {
            if (ls_data->pwr_level[k]) {
                ls_data->sums->w = ls_data->sums->w - ls_data->w[k] * ls_data->w[k];
                ls_data->sums->num_points = ls_data->sums->num_points - 1;
                ls_data->sums->wk = ls_data->sums->wk - ls_data->w[k] * ls_data->w[k] * ls_data->tau[k];
                ls_data->sums->wk2 = ls_data->sums->wk2 - ls_data->wt2[k];
            }
        }

        c_log_err = 0;
        /*  start with the lamda fit */
        do_lambda_fit(fitted_prms, fit_range, lag,ls_data);

        /* ----------------now do the sigma fit ------------------------ */
        do_sigma_fit(fitted_prms, fit_range, lag,ls_data);
        
        /* finally check for ground scatter fit */

        /*  first, see if an ACF preprocessor has already identified the
            scatter as being ground scatter.  */
        if (acf_stat == ACF_GROUND_SCAT) {
            fit_range->gsct = 1; 
        }
        else {
            fit_range->gsct = 0;
        }
    }

    free_ls_data(ls_data);

    /* all done - return code = 1 */
    if (npp < 1) {
        return 4;
    }
    else return 1;
}
