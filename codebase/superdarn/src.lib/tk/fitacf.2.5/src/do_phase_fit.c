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

#define determ(aa,bb,cc,dd) (((aa)*(dd)) - ((bb)*(cc)))

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
    double t2;
    double wbar;
    double phitmp,phifrc,phiint;
    int n_twopi;
    int nphi;
    int k;

    double d=0.0, e2;

    omega_loc = omega_guess;
    t2 = ls_data->t0 * ls_data->t0;

    phi_k = malloc(sizeof(double)*mplgs);
    if (phi_k==NULL) {
        return -1;
    }

    for (k=0;k<mplgs;k++) {
        phi_k[k]=0;
    }

    while (fabs(omega_old - omega_loc) > fabs(omega_loc * PI/64.)) {

        /* if omega_loc == omega_old_2 it means we are oscillating between
             two different values of *omega */

        if ((icnt>0) && (omega_loc == omega_old_2)) {
            *omega = (omega_old + omega_loc)/2.;
            /* return the average value of the two *omega values and return
                with error code 16 */
            ls_data->phi_loc = phi_loc;
            free(phi_k);
            return 16;
        }

        /* if icnt >= 5 it means we aren't converging on a stable value for
             *omega */

        if (++icnt >= 5) {
            /* return whatever we have at this moment
                and set error code 32 */
            *omega = omega_loc;
            ls_data->phi_loc = phi_loc;
            free(phi_k);
            return 32;
        }

        omega_old_2 = omega_old;
        omega_old = omega_loc;
        omega_init = omega_loc;

        if (!xflag) phi_loc = 0.;

        sum_phi = atan2(acf[0].y,acf[0].x);
        sum_phi = sum_phi * ls_data->w[0] * ls_data->w[0];
        sum_kphi = 0.0;
        n_twopi = 0;
        nphi = 0;

        /* now go through the point, one at a time, predicting the new
             value for phi_tot from the current best value for *omega */

        for (k=1; k<mplgs; k++) {
            if (badlag[k]) continue;
            phi_pred = phi_loc + omega_loc * ls_data->tau[k] * ls_data->t0;
     
            /* The code for calculating n_twopi had a problem, the conversion to
                 an integer sometimes produced the wrong result
            */

         
            phitmp = ((PI + phi_pred - ls_data->phi_res[k])/(2*PI) - 
                ((omega_loc > 0) ? 0.0 : 1.0));

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

            if (!xflag && ls_data->sums->wk2_arr[k] && (omega_old_2 == 9999.)) {
                omega_loc = sum_kphi/(ls_data->t0 * ls_data->sums->wk2_arr[k]);
                omega_loc = (nphi * omega_loc + omega_init)/(nphi + 1);
            }
        }

        if (xflag) {
            d = determ(ls_data->sums->w,ls_data->sums->wk * ls_data->t0,ls_data->sums->wk * ls_data->t0,ls_data->sums->wk2 * t2);
            if (d == 0) {
                free(phi_k);
                return 8;
            }
            phi_loc = determ(sum_phi,ls_data->sums->wk * ls_data->t0,sum_kphi * ls_data->t0,ls_data->sums->wk2 * t2)/d;
            omega_loc = determ(ls_data->sums->w,sum_phi,ls_data->sums->wk * ls_data->t0,sum_kphi * ls_data->t0)/d;
        } else {
            phi_loc = 0;
            if (ls_data->sums->wk2 <= 0.0) {
                free(phi_k);
                return 8;
            }
            omega_loc = sum_kphi/(ls_data->t0*ls_data->sums->wk2);
        }
    }
    /*  End of While loop */

    if (phi_loc > PI) phi_loc = phi_loc - 2*PI;

    ls_data->phi_loc = phi_loc;
    *omega = omega_loc;

    /* Now we calculate the estimated error of the fit */

    wbar = 0;
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
        ls_data->phi_err =  ls_data->phase_sdev * wbar * sqrt(ls_data->sums->wk2*t2/d);
        ls_data->omega_err = ls_data->phase_sdev * wbar * sqrt(ls_data->sums->w/d);
    }
    else {
        ls_data->phi_err = 0;
        ls_data->omega_err = ls_data->phase_sdev*wbar/sqrt(ls_data->sums->wk2)/ls_data->t0;
    }
    free(phi_k);
    return 0;
}




