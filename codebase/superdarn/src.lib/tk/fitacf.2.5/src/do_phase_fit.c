/* do_phase_fit.c
     ==============
     Author: K.Baker
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




/*
    This routine does the fitting to the phase of the ACF.  It is called
    from 'fit_acf'.

    status values returned by the function are:
        0 = OK
        16 = PHASE_OSCILLATION
        32 = NO_CONVERGENCE

    returned values:
        *omega  -  (double) the least square error fit for the slope
        ls_data->phi_loc - (double) phase at lag 0 (may be non-zero for XCFs)
        ls_data->phase_sdev - (double) average standard deviation of the points in the fit
        ls_data->omega_err - (double) 1-sigma error estimate of the slope
        ls_data->phi_err - (double) 1-sima error estimate on lag-0 phase
    input values:
        omega_guess - (double) initial guess for the value of *omega
        xflag - (char) flag to indicate this is an XCF fit
        mplgs - (int) number of lags in the acf
        acf - (struct complex) the acf (or xcf) to be fitted
        tau - (double) array of lag values
        ls_data->w - (double) array of weights (powers) for each lag
        ls_data->sums->wk2_arr - (double) array of sum of ls_data->w*k^2 
        ls_data->phi_res - (double) array of measured phases
        badlag - (int) array of bad lag flags
        ls_data->t0 - (double) basic time lag
        ls_data->sums->w - (double) sum of the weights
        ls_data->sums->wk - (double) sum of ls_data->w*k
        ls_data->sums->wk2 - (double) sum of ls_data->w*k^2

*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rmath.h"
#include "do_phase_fit.h"
#include "badlags.h"

#define determ(aa,bb,cc,dd) (((aa)*(dd)) - ((bb)*(cc)))

/**
Fits a straight line to the phase using a iterative fitting approach
*/
int phase_fit(double omega_guess, char xflag, int mplgs,
                struct complex *acf, int *badlag,
                double *omega, LS_DATA *ls_data, double *phi_k, double *omega__loc,
                double *phi__loc, double *d){
    double omega_loc, omega_init;
    double omega_old_2=9999.0, omega_old = 9999.0;
    int icnt = 0;
    double phase_slope[3];
    double phase_diff = 0.0;
    double sum_phi = 0.0;
    double sum_kphi = 0.0;
    double phi_pred;
    double phi_tot;
    /*double ls_data->t2;*/
    double wbar;
    double phitmp,phifrc,phiint;
    int n_twopi;
    int nphi;
    int k;
    int i = 2;
    double d_aa, d_bb, d_cc, d_dd;
    double b_aa, b_bb, b_cc, b_dd; 
    double a_aa, a_bb, a_cc, a_dd; 

        
    phase_slope[2] = omega_guess;
    phase_slope[1] = 9999.0;
    phase_slope[0] = 9999.0;

    while (fabs(phase_slope[i-1] - phase_slope[i]) > fabs(phase_slope[i] * PI/64.)) {

        /* if omega_loc == omega_old_2 it means we are oscillating between
             two different values of *omega */

        if ((icnt>0) && (phase_slope[i] == phase_slope[i-2])) {
            *omega = (phase_slope[i-1] + phase_slope[i])/2.;
            /* return the average value of the two *omega values and return
                with error code 16 */
            ls_data->phi_loc = phase_diff;
            free(phi_k);
            return 16;
        }

        /* if icnt >= 5 it means we aren't converging on a stable value for
             *omega */

        if (++icnt >= 5) {
            /* return whatever we have at this moment
                and set error code 32 */
            *omega = phase_slope[i];
            ls_data->phi_loc = phase_diff;
            free(phi_k);
            return 32;
        }

        phase_slope[i-2] = phase_slope[i-1];
        phase_slope[i-1] = phase_slope[i];


        if (!xflag) phase_diff = 0.;

        sum_phi = atan2(acf[0].y,acf[0].x);
        sum_phi = sum_phi * ls_data->w[0] * ls_data->w[0];
        sum_kphi = 0.0;
        n_twopi = 0;
        nphi = 0;

        /* now go through the point, one at a time, predicting the new
             value for phi_tot from the current best value for *omega */

        for (k=1; k<mplgs; k++) {
            if (badlag[k]) continue;
            phi_pred = phase_diff + phase_slope[i] * ls_data->tau[k] * ls_data->t0;
     
            /* The code for calculating n_twopi had a problem, the conversion to
                 an integer sometimes produced the wrong result
            */

         
            phitmp = ((PI + phi_pred - ls_data->phi_res[k])/(2*PI) - 
                ((phase_slope[i] > 0) ? 0.0 : 1.0));

            phifrc=modf(phitmp,&phiint);
            n_twopi=(int) phiint;            
            if (phifrc>0.5) n_twopi++;
            if (phifrc<-0.5) n_twopi--;

            phi_tot = ls_data->phi_res[k] + n_twopi*(2*PI);

            if (fabs(phi_pred - phi_tot) > PI) {
                if (phi_pred > phi_tot) phi_tot = phi_tot + 2*PI;
                else phi_tot = phi_tot - 2*PI;
            }
            

            phi_k[k] = phi_tot;
            sum_phi = sum_phi + phi_tot * ls_data->w[k] * ls_data->w[k];
            sum_kphi = sum_kphi + ls_data->tau[k] * phi_tot * ls_data->w[k] * ls_data->w[k];
            ++nphi;

            /* if this is the first time through the *omega fit loop AND
            we are doing ACFs, NOT xcfs, and we've got enough points to
        draw a line, THEN compute a new value of omega_loc as we add each
        new point */

            if (!xflag && ls_data->sums->wk2_arr[k] && (phase_slope[i-2] == 9999.)) {
                phase_slope[i] = sum_kphi/(ls_data->t0 * ls_data->sums->wk2_arr[k]);
                phase_slope[i] = (nphi * phase_slope[i] + phase_slope[i-1])/(nphi + 1);
            }
        }

        if (xflag) {
            d_aa = ls_data->sums->w;
            d_bb = ls_data->sums->wk * ls_data->t0;
            d_cc = ls_data->sums->wk * ls_data->t0;
            d_dd = ls_data->sums->wk2 * ls_data->t2;
            *d = determ(d_aa,d_bb,d_cc,d_dd);
            if (d == 0) {
                free(phi_k);
                return 8;
            }

            b_aa = sum_phi;
            b_bb = ls_data->sums->wk * ls_data->t0;
            b_cc = sum_kphi * ls_data->t0;
            b_dd = ls_data->sums->wk2 * ls_data->t2;

            phase_diff = determ(b_aa,b_bb,b_cc,b_dd)/ *d;

            a_aa = ls_data->sums->w;
            a_bb = sum_phi;
            a_cc = ls_data->sums->wk * ls_data->t0;
            a_dd = sum_kphi * ls_data->t0;

            phase_slope[i] = determ(a_aa,a_bb,a_cc,a_dd)/ *d;
        } else {
            phase_diff = 0;
            if (ls_data->sums->wk2 <= 0.0) {
                free(phi_k);
                return 8;
            }
            phase_slope[i] = sum_kphi/(ls_data->t0*ls_data->sums->wk2);
        }
    }
    /*  End of While loop */

    if (phase_diff > PI) phase_diff = phase_diff - 2*PI;

    ls_data->phi_loc = phase_diff;
    *omega = phase_slope[i];

    *phi__loc=phase_diff;
    *omega__loc=phase_slope[i];

    return 0;
}

/**
This function provides the uncertainty in the phase fit
*/
void phase_fit_error(LS_DATA *ls_data, double phase_diff, double phase_slope, 
                     int *lag, int xflag, int mplgs, double *phi_k,
                     double d){
    double wbar;
    int nphi;
    double e2;
    int k;
    double constant_a, constant_b;
    wbar = 0;
    e2 = 0.;
    nphi = 0;
    for (k=0; k<mplgs; k++) {
        if (lag[k] == GOOD) {
            constant_a = phi_k[k] - phase_diff - phase_slope * ls_data->tau[k] * ls_data->t0;
            e2 += ls_data->w[k]*ls_data->w[k] * constant_a * constant_a;
            wbar += ls_data->w[k];
            nphi++;
        }
    }
    wbar = wbar/nphi;
    if (xflag) ls_data->phase_sdev = sqrt(e2/(ls_data->sums->w)/(nphi-2));
    else ls_data->phase_sdev = sqrt(e2/ls_data->sums->w/(nphi-1));

    if (xflag) {
        ls_data->phi_err =  ls_data->phase_sdev * wbar * sqrt(ls_data->sums->wk2*ls_data->t2/d);
        ls_data->omega_err = ls_data->phase_sdev * wbar * sqrt(ls_data->sums->w/d);
    }
    else {
        ls_data->phi_err = 0;
        ls_data->omega_err = ls_data->phase_sdev*wbar/sqrt(ls_data->sums->wk2)/ls_data->t0;
    }
}

/**
Performs a fit to the phase and its respective error calculations
*/
int do_phase_fit (double omega_guess, char xflag, int mplgs,
                struct complex *acf, int *badlag,
                double *omega, LS_DATA *ls_data) {

    /*  local declarations */
    double omega_loc, omega_init;
    double omega_old_2=9999.0, omega_old = 9999.0;
    int icnt = 0;

    double phi_loc = 0.0;
    double sum_phi = 0.0;
    double sum_kphi = 0.0;
    double phi_pred;
    double phi_tot;
    double *phi_k=NULL;
    /*double ls_data->t2;*/
    double wbar;
    double phitmp,phifrc,phiint;
    int n_twopi;
    int nphi;
    int k;
    int status;

    double d=0.0, e2;

    
    ls_data->t2 = ls_data->t0 * ls_data->t0;

    phi_k = malloc(sizeof(double)*mplgs);
    if (phi_k==NULL) {
        return -1;
    }

    for (k=0;k<mplgs;k++) {
        phi_k[k]=0;
    }


    status = phase_fit(omega_guess,xflag,mplgs,acf, badlag, omega, ls_data, phi_k ,&omega_loc,&phi_loc, &d);
    
    if (status > 0) return status;

    /* Now we calculate the estimated error of the fit */

    phase_fit_error(ls_data, phi_loc, omega_loc, badlag, xflag, mplgs, phi_k, d);

/*    wbar = 0;
    e2 = 0.;
    nphi = 0;
    for (k=0; k<mplgs; k++) {
        if (!badlag[k]) {
            e2 += ls_data->w[k]*ls_data->w[k]*(phi_k[k] - phi_loc - omega_loc * ls_data->tau[k]*ls_data->t0)*
                         (phi_k[k] - phi_loc - omega_loc * ls_data->tau[k] * ls_data->t0);
            wbar += ls_data->w[k];
            nphi++;
        }
    }
    wbar = wbar/nphi;
    if (xflag) ls_data->phase_sdev = sqrt(e2/(ls_data->sums->w)/(nphi-2));
    else ls_data->phase_sdev = sqrt(e2/ls_data->sums->w/(nphi-1));

    if (xflag) {
        ls_data->phi_err =  ls_data->phase_sdev * wbar * sqrt(ls_data->sums->wk2*ls_data->t2/d);
        ls_data->omega_err = ls_data->phase_sdev * wbar * sqrt(ls_data->sums->w/d);
    }
    else {
        ls_data->phi_err = 0;
        ls_data->omega_err = ls_data->phase_sdev*wbar/sqrt(ls_data->sums->wk2)/ls_data->t0;
    }*/
    free(phi_k);
    return 0;
}




