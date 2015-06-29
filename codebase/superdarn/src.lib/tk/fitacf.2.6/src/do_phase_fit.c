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
        lag - (int) array of bad lag flags
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
                struct complex *acf, int *lag,
                OMEGA location, LS_DATA *ls_data){
    double *omega;
    int icnt = 0;
    double phase_slope[3];
    double phase = 0.0;
    double phi_pred;
    double phi_tot;
    double phitmp,phifrc,phiint;
    int n_twopi;
    int nphi;
    int k;
    int i = 2;
    double d,d_aa, d_bb, d_cc, d_dd;
    double b_aa, b_bb, b_cc, b_dd; 
    double a_aa, a_bb, a_cc, a_dd; 

        
    phase_slope[2] = omega_guess;
    phase_slope[1] = 9999.0;
    phase_slope[0] = 9999.0;


    switch(location){
        case HIGH:
            omega = &ls_data->omega_high;
            break;

        case LOW :
            omega = &ls_data->omega_low;
            break;

        case BASE:
            omega = &ls_data->omega_base;
            break;

        default:
            break; 
    }


    while (fabs(phase_slope[i-1] - phase_slope[i]) > fabs(phase_slope[i] * PI/64.)) {

        /* if phase_slope[i] == phase_slope[i-2] it means we are oscillating between
             two different values of slope */

        if ((icnt>0) && (phase_slope[i] == phase_slope[i-2])) {
            *omega = (phase_slope[i-1] + phase_slope[i])/2.;
            /* return the average value of the two slope values and return
                with error code 16 */
            ls_data->phi_loc = phase;
            return 16;
        }

        /* if icnt >= 5 it means we aren't converging on a stable value for
             slope */

        if (++icnt >= 5) {
            /* return whatever we have at this moment
                and set error code 32 */
            *omega = phase_slope[i];
            ls_data->phi_loc = phase;
            return 32;
        }

        phase_slope[i-2] = phase_slope[i-1];
        phase_slope[i-1] = phase_slope[i];


        if (!xflag) phase = 0.;

        ls_data->sums->phi = atan2(acf[0].y,acf[0].x);
        ls_data->sums->phi = ls_data->sums->phi * ls_data->w[0] * ls_data->w[0];
        ls_data->sums->kphi = 0.0;
        n_twopi = 0;
        nphi = 0;

        /* now go through the point, one at a time, predicting the new
             value for phi_tot from the current best value for slope */

        for (k=1; k<mplgs; k++) {
            if (lag[k] != GOOD) continue;
            phi_pred = phase + phase_slope[i] * ls_data->tau[k] * ls_data->t0;
     
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
            

            ls_data->phi_k[k] = phi_tot;
            ls_data->sums->phi = ls_data->sums->phi + phi_tot * ls_data->w[k] * ls_data->w[k];
            ls_data->sums->kphi = ls_data->sums->kphi + ls_data->tau[k] * phi_tot * ls_data->w[k] * ls_data->w[k];
            ++nphi;

            /* if this is the first time through the *omega fit loop AND
            we are doing ACFs, NOT xcfs, and we've got enough points to
        draw a line, THEN compute a new value of omega_loc as we add each
        new point */

            if (!xflag && ls_data->sums->wk2_arr[k] && (phase_slope[i-2] == 9999.)) {
                phase_slope[i] = ls_data->sums->kphi/(ls_data->t0 * ls_data->sums->wk2_arr[k]);
                phase_slope[i] = (nphi * phase_slope[i] + phase_slope[i-1])/(nphi + 1);
            }
        }

        if (xflag) {
            d_aa = ls_data->sums->w;
            d_bb = ls_data->sums->wk * ls_data->t0;
            d_cc = ls_data->sums->wk * ls_data->t0;
            d_dd = ls_data->sums->wk2 * ls_data->t2;
            d = determ(d_aa,d_bb,d_cc,d_dd);
            if (d == 0) {
                return 8;
            }

            b_aa = ls_data->sums->phi;
            b_bb = ls_data->sums->wk * ls_data->t0;
            b_cc = ls_data->sums->kphi * ls_data->t0;
            b_dd = ls_data->sums->wk2 * ls_data->t2;

            phase = determ(b_aa,b_bb,b_cc,b_dd)/d;

            a_aa = ls_data->sums->w;
            a_bb = ls_data->sums->phi;
            a_cc = ls_data->sums->wk * ls_data->t0;
            a_dd = ls_data->sums->kphi * ls_data->t0;

            phase_slope[i] = determ(a_aa,a_bb,a_cc,a_dd)/d;
        } else {
            phase = 0;
            if (ls_data->sums->wk2 <= 0.0) {
                return 8;
            }
            phase_slope[i] = ls_data->sums->kphi/(ls_data->t0*ls_data->sums->wk2);
        }
    }
    /*  End of While loop */

    if (phase > PI) phase = phase - 2*PI;

    ls_data->phi_loc = phase;
    *omega = phase_slope[i];

    return 0;
}

/**
This function provides the uncertainty in the phase fit
*/
void phase_fit_error(LS_DATA *ls_data, OMEGA location,
                     int *lag, int xflag, int mplgs){
    double wbar;
    int nphi;
    double e2;
    int k;
    double constant_a;
    double d, d_aa, d_bb, d_cc, d_dd;
    double phase_slope;
    wbar = 0;
    e2 = 0.;
    nphi = 0;

    switch(location){
        case HIGH:
            phase_slope = ls_data->omega_high;
            break;

        case LOW :
            phase_slope = ls_data->omega_low;
            break;

        case BASE:
            phase_slope = ls_data->omega_base;
            break;

        default:
            break;
    }

    for (k=0; k<mplgs; k++) {
        if (lag[k] == GOOD) {
            constant_a = ls_data->phi_k[k] - ls_data->phi_loc - phase_slope * ls_data->tau[k] * ls_data->t0;
            e2 += ls_data->w[k]*ls_data->w[k] * constant_a * constant_a;
            wbar += ls_data->w[k];
            nphi++;
        }
    }
    wbar = wbar/nphi;
    if (xflag) ls_data->phase_sdev = sqrt(e2/(ls_data->sums->w)/(nphi-2));
    else ls_data->phase_sdev = sqrt(e2/ls_data->sums->w/(nphi-1));

    if (xflag) {
        d_aa = ls_data->sums->w;
        d_bb = ls_data->sums->wk * ls_data->t0;
        d_cc = ls_data->sums->wk * ls_data->t0;
        d_dd = ls_data->sums->wk2 * ls_data->t2;
        d = determ(d_aa,d_bb,d_cc,d_dd);

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
                struct complex *acf, int *lag,
                OMEGA location, LS_DATA *ls_data) {

    int status;
    ls_data->phi_k[0] = 0;

    status = phase_fit(omega_guess,xflag,mplgs,acf, lag, location, ls_data);
    
    if (status > 0) return status;

    /* Now we calculate the estimated error of the fit */

    phase_fit_error(ls_data, location, lag, xflag, mplgs);


    return 0;
}




