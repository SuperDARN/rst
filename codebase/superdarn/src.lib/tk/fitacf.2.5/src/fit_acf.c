/* fit_acf.c
     =========
     Author: R.J.Barnes & K.Baker & P.Ponomarenko

Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory


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
*/



#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <complex.h>

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

int fit_acf (double complex *acf,int range, int *badlag, 
        struct FitACFBadSample *badsmp,int lag_lim, struct FitPrm *prm, 
        double noise_lev_in,char xflag,double xomega, struct FitRange *ptr) {

    double sum_np,sum_w,sum_wk,sum_wk2,*sum_wk2_arr=NULL,sum_wk4, 
    sum_p,sum_pk,sum_pk2;
    //double sum_phi;
    //double sum_kphi;
    double t0,t2,t4,*phi_res=NULL;
    int j, npp, s = 0, last_good, status, *bad_pwr = NULL;
    long k;
    double *tau=NULL, *tau2=NULL, *phi_k=NULL, *w=NULL, *pwr=NULL,
            *wt=NULL, *wt2=NULL, *wp=NULL, c_log,c_log_err,
            omega_loc, omega_err_loc, phi_loc, noise_lev, omega_base,
            omega_high, omega_low, phase_sdev, phi_err, omega_err;

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
        for (j=0; j<prm->mplgs; j++) {
            badlag[j]=3;
        }
        /*set the fit structure to zeroes*/
        zero_fitrange(ptr);
        return 2;
    }

    /* allocate memory for least square arrays */
    s = allocate_ls_arrays(prm, &sum_wk2_arr, &phi_res, &tau, &tau2,
                            &phi_k, &w, &pwr, &wt, &wt2, &wp, &bad_pwr);

    if (s == -1){
        return -1;
    }

    /* initialize the table of abs(acf[k]) and log(abs(acf[k])) */
    FitACFCkRng(range, badlag, badsmp, prm);

    /* Save the original ACF in a new variable so we can try some
         preprocessing on it.

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
            range, badlag, prm->mplgs);
    */

    /*
        fill in arrays for tau, tau^2, and power
        and then mark badlags with power below the noise level
    */
    for (k=0; k<prm->mplgs; k++) {
        tau[k] = prm->lag[1][k] - prm->lag[0][k];
        tau2[k] = tau[k] * tau[k];
        w[k] = cabs(acf[k]); /* w[k] = cabs(acf[k])- noise_lev; */
        if (w[k] <= noise_lev) {
            w[k] = 0.1; /* if (w[k] <= 0.0) w[k] = 0.1; */
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

    P0n = w[0]/sqrt((double) prm->nave);
    if ((w[0] - P0n) < noise_lev) {
        free_arrays(&sum_wk2_arr, &phi_res, &tau, &tau2,
                    &phi_k, &w, &pwr, &wt, &wt2, &wp, &bad_pwr);
        return 2;
    }
    /* give up if left over pwr is too low */


    /*  identify any additional bad lags */
    sum_np = more_badlags(w, badlag, noise_lev, prm->mplgs,prm->nave);

    ptr->nump = (char) sum_np;

    /*  We must have at least lag_lim good lags */
    if (sum_np < lag_lim) {
        free_arrays(&sum_wk2_arr, &phi_res, &tau, &tau2,
                    &phi_k, &w, &pwr, &wt, &wt2, &wp, &bad_pwr);
        return 4;
    }

    /* this is required to make logs ok */
    if (noise_lev <= 0.0) {
        noise_lev = 0.1;
    }

    /* This is to remove background delta-correlated noise from lag 0 power (version 2.0)*/
    w[0] = w[0]-prm->noise;

    /* OK, now we have determined the good lags for the phase fit.
         Now subtract of P0n from the power profile */

    /* calculate the power values for each lag.  'w' is the linear power.
         wt is the power times the lag.  wt2 is power times lag**2.
         pwr is the log of the power. wp is the linear power times the log of
         the power.  The items that involve the linear power are all parts of
         the least squares fits with the weighting done by the linear power. */
    for (k=0; k<prm->mplgs; k++) {
        if (w[k] <= P0n) {
            w[k] = 0.1; /* if (w[k] <= 0.0) w[k] = 0.1; */
        }
        wt[k] = w[k]*w[k]*tau[k];
        wt2[k] = wt[k]*tau[k];
        pwr[k] = log(w[k]);
        wp[k] = w[k]*w[k]*pwr[k];
    }

    /* we now have to check to see how many additional bad lags have been
         introduced by subtracting off P0n. */
    for (k=0, npp=0; k < prm->mplgs; k++) {
        if (w[k] < noise_lev+P0n && !badlag[k]) bad_pwr[k] = 1;
        /* if (w[k] < noise_lev && !badlag[k]) bad_pwr[k] = 1; */
        else bad_pwr[k] = 0;
        if (! (badlag[k] || bad_pwr[k])) ++npp;
    }

    /* set the sums to initial values*/
    sum_np = 1;
    sum_w = w[0]*w[0];
    sum_wk = 0;
    sum_wk2 = 0;
    sum_wk2_arr[0] = 0;
    sum_wk4 = 0;
    sum_p = w[0]*w[0]*pwr[0];
    sum_pk = 0;
    sum_pk2 = 0;
    phi_loc = atan2(cimag(acf[0]), creal(acf[0]));
    //sum_kphi = 0;
    t0 =  prm->mpinc * 1.0e-6;
    t2 = t0 * t0;
    t4 = t2 * t2;

    /* calculate all the residual phases */
    /* if calc_phi_res returns a bad status abort the fit */
    s = calc_phi_res(acf, badlag, phi_res, prm->mplgs);
    if (s != 0) {
        free_arrays(&sum_wk2_arr, &phi_res, &tau, &tau2,
                    &phi_k, &w, &pwr, &wt, &wt2, &wp, &bad_pwr);
        return 2;
    }


    if (!xflag) {
        /*if it's a regular fit (not XCF)*/
        if (acf_stat == ACF_GROUND_SCAT) omega_loc = 0.0;
        else omega_loc = omega_guess(acf, tau, badlag, phi_res, &omega_err_loc,prm->mpinc,prm->mplgs);
        phi_k[0] = 0;
        //sum_phi = 0;
    } else {
        /*if it's an XCF fit (not ACF)*/
        phi_k[0] = phi_loc;
        //sum_phi = phi_loc * w[0] * w[0];
        omega_loc = xomega;
    }


    /*  The preliminaries are now over.
    Now start the fitting process */

    /* first, calculate the sums needed for the phase fit */
    for (k=1; k<prm->mplgs; k++) {
        if (badlag[k]) {
            sum_wk2_arr[k] = sum_wk2_arr[k-1];
            continue;
        }
        sum_w = sum_w + w[k]*w[k];
        sum_np = sum_np + 1;
        sum_wk = sum_wk + w[k]*w[k]*tau[k];
        sum_wk2 = sum_wk2 + wt2[k];
        sum_wk2_arr[k] = sum_wk2;
    }

    /* Now do the phase fit using the best initial guess for omega */


    status = do_phase_fit (omega_loc, xflag, prm->mplgs, acf, tau,
             w, sum_wk2_arr, phi_res, badlag, t0,
             sum_w, sum_wk, sum_wk2,
             &omega_base, &phi_loc, &phase_sdev,
             &phi_err, &omega_err);

    ptr->phi0 = phi_loc;
    ptr->v = omega_base;
    ptr->sdev_phi = phase_sdev;
    ptr->phi0_err = phi_err;
    ptr->v_err = omega_err;

    /* check the status of the phase fit to see if it was actually OK.
         if not, set error bars to HUGE_VAL */

    if (status != 0) {
        ptr->sdev_phi = HUGE_VAL;
        ptr->v_err = HUGE_VAL;
        if (xflag) ptr->phi0_err = HUGE_VAL;
    }

    /* OK, we now have our baseline value for omega.  Now re-do the
         phase fit, but using omega_loc + omega__err_loc. */


    if (!xflag && (status == 0)) {
        status = do_phase_fit (omega_loc + omega_err_loc,
                                xflag, prm->mplgs, acf, tau,
                                w, sum_wk2_arr, phi_res, badlag, t0,
                                sum_w, sum_wk, sum_wk2,
                                &omega_high, &phi_loc, &phase_sdev,
                                &phi_err, &omega_err);

        status = do_phase_fit (omega_loc - omega_err_loc,
                                xflag, prm->mplgs, acf, tau,
                                w, sum_wk2_arr, phi_res, badlag, t0,
                                sum_w, sum_wk, sum_wk2,
                                &omega_low, &phi_loc, &phase_sdev,
                                &phi_err, &omega_err);

        /* if the difference between the high and low values of omega
             is greater than the error estimate of the original fit,
             we will use the original fit as our best guess for the
             velocity, but we'll set the error to be the difference between
             the high and low values.  Actually, at this point we should have
             non-symmetric error bar, but the file format has no provision
             for that. */

        if (fabs(omega_high - omega_low) >= 2*ptr->v_err) {
            ptr->v = omega_base;
        
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
        c_log = pwr[0];

        /* if c_log < 0 it means that after subtracting the noise and P0n,
         the result is less than 1.0.  This must really be pretty meaningless
         It shouldn't even be possible since we have supposedly already checked
         this at the beginning. */

        if (c_log < 0 ) {
            free_arrays(&sum_wk2_arr, &phi_res, &tau, &tau2,
                        &phi_k, &w, &pwr, &wt, &wt2, &wp, &bad_pwr);
            return 2;
        }

        ptr->p_l = c_log;
        ptr->p_s = c_log;

        /* find the last good lag */
        last_good = -1;
        for (k= 0; k < prm->mplgs; k++) if (!badlag[k]) last_good = k;

        /* if there are no good lags, or only lag-0 is good, set the width
             to a high negative value, by setting the last_good lag to 1
             */

        if (last_good <=0 ) {
            ptr->w_l = -9999.0;
            ptr->w_s = -9999.0;
            set_lambda_error_huge(ptr);
            set_sigma_error_huge(ptr);
        } else {
            /* now calculate the width as the lag-0 power divided by the
             time to the last good lag. */

            ptr->w_l = c_log/(tau[last_good]*t0);
            ptr->w_s = c_log/(tau2[last_good]*t2);

            /* set the errors to the maximum value */
            set_lambda_error_huge(ptr);
            set_sigma_error_huge(ptr);
        }
    } else {
        /*  Calculate the sums that were not used in the phase fit */
        for (k=1; k < prm->mplgs; k++) {
            if (badlag[k] || bad_pwr[k]) {
                continue;
            }
            sum_p = sum_p + wp[k];
            sum_pk = sum_pk + pwr[k]*wt[k];
            sum_pk2 = sum_pk2 + pwr[k]*wt2[k];
            sum_wk4 = sum_wk4 + wt2[k]*tau2[k];
        }

        /* Now adjust the sums that were used in the phase fit, but that
             have changed because of additional bad lags */

        for (k=1; k< prm->mplgs; k++) {
            if (bad_pwr[k]) {
                sum_w = sum_w - w[k]*w[k];
                sum_np = sum_np - 1;
                sum_wk = sum_wk - w[k]*w[k]*tau[k];
                sum_wk2 = sum_wk2 - wt2[k];
            }
        }

        c_log_err = 0;
        /*  start with the lamda fit */
        do_lambda_fit(prm, ptr, badlag,
                        bad_pwr, w,  tau, pwr, sum_np, sum_w, t0,
                        sum_wk, sum_wk2, c_log_err, sum_p, t2, sum_pk);

        /* ----------------now do the sigma fit ------------------------ */
        do_sigma_fit(prm, ptr, badlag,
                        bad_pwr,  w,  tau,  tau2, pwr, sum_np,
                        sum_w, t0, sum_wk, sum_wk2, c_log_err,
                        sum_p, t2, sum_pk, sum_wk4, t4,
                        sum_pk2);

        /* finally check for ground scatter fit */

        /*  first, see if an ACF preprocessor has already identified the
            scatter as being ground scatter.  */
        if (acf_stat == ACF_GROUND_SCAT) {
            ptr->gsct = 1;
        }
        else {
            ptr->gsct = 0;
        }
    }

    free_arrays(&sum_wk2_arr, &phi_res, &tau, &tau2,
                &phi_k, &w, &pwr, &wt, &wt2, &wp, &bad_pwr);

    /* all done - return code = 1 */
    if (npp < 1) {
        return 4;
    }
    else return 1;
}
