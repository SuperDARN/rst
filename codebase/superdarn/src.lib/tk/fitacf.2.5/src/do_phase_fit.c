/* do_phase_fit.c
     ==============
     Author: K.Baker

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




/*
    This routine does the fitting to the phase of the ACF.  It is called
    from 'fit_acf'.

    status values returned by the function are:
        0 = OK
        16 = PHASE_OSCILLATION
        32 = NO_CONVERGENCE

    returned values:
        omega  -  (double) the least square error fit for the slope
        phi0 - (double) phase at lag 0 (may be non-zero for XCFs)
        sdev - (double) average standard deviation of the points in the fit
        omega_err - (double) 1-sigma error estimate of the slope
        phi0_err - (double) 1-sima error estimate on lag-0 phase
    input values:
        omega_guess - (double) initial guess for the value of omega
        xflag - (char) flag to indicate this is an XCF fit
        mplgs - (int) number of lags in the acf
        acf - (double complex) the acf (or xcf) to be fitted
        tau - (double) array of lag values
        w - (double) array of weights (powers) for each lag
        sum_wk2_arr - (double) array of sum of w*k^2
        phi_res - (double) array of measured phases
        badlag - (int) array of bad lag flags
        t0 - (double) basic time lag
        sum_w - (double) sum of the weights
        sum_wk - (double) sum of w*k
        sum_wk2 - (double) sum of w*k^2

*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include "rmath.h"

#define determ(aa,bb,cc,dd) (((aa)*(dd)) - ((bb)*(cc)))

int do_phase_fit (double omega_guess,
                    char xflag,
                    int mplgs,
                    double complex *acf,
                    double *tau,
                    double *w,
                    double *sum_wk2_arr,
                    double *phi_res,
                    int *badlag,
                    double t0,
                    double sum_w,
                    double sum_wk,
                    double sum_wk2,
                    double *omega,
                    double *phi0,
                    double *sdev,
                    double *phi0_err,
                    double *omega_err) {

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
    t2 = t0*t0;

    phi_k = malloc(sizeof(double)*mplgs);
    if (phi_k==NULL) {
        return -1;
    }

    for (k=0;k<mplgs;k++) {
        phi_k[k]=0;
    }

    while (fabs(omega_old - omega_loc) > fabs(omega_loc * PI/64.)) {

        /* if omega_loc == omega_old_2 it means we are oscillating between
             two different values of omega */

        if ((icnt>0) && (omega_loc == omega_old_2)) {
            *omega = (omega_old + omega_loc)/2.;
            /* return the average value of the two omega values and return
                with error code 16 */
            *phi0 = phi_loc;
            free(phi_k);
            return 16;
        }

        /* if icnt >= 5 it means we aren't converging on a stable value for
             omega */

        if (++icnt >= 5) {
            /* return whatever we have at this moment
                and set error code 32 */
            *omega = omega_loc;
            *phi0 = phi_loc;
            free(phi_k);
            return 32;
        }

        omega_old_2 = omega_old;
        omega_old = omega_loc;
        omega_init = omega_loc;

        if (!xflag) phi_loc = 0.;

        sum_phi = atan2(cimag(acf[0]), creal(acf[0]));
        sum_phi = sum_phi*w[0]*w[0];
        sum_kphi = 0.0;
        n_twopi = 0;
        nphi = 0;

        /* now go through the point, one at a time, predicting the new
             value for phi_tot from the current best value for omega */

        for (k=1; k<mplgs; k++) {
            if (badlag[k]) continue;
            phi_pred = phi_loc + omega_loc*tau[k]*t0;

            /* The code for calculating n_twopi had a problem, the conversion to
                 an integer sometimes produded the wrong result
            */


            phitmp = ((PI + phi_pred - phi_res[k])/(2*PI) -
                ((omega_loc > 0) ? 0.0 : 1.0));

            phifrc=modf(phitmp,&phiint);
            n_twopi=(int) phiint;
            if (phifrc>0.5) n_twopi++;
            if (phifrc<-0.5) n_twopi--;

            phi_tot = phi_res[k] + n_twopi*(2*PI);

            if (fabs(phi_pred - phi_tot) > PI) {
                if (phi_pred > phi_tot) phi_tot = phi_tot + 2*PI;
                else phi_tot = phi_tot - 2*PI;
            }


            phi_k[k] = phi_tot;
            sum_phi = sum_phi + phi_tot*w[k]*w[k];
            sum_kphi = sum_kphi + tau[k]*phi_tot*w[k]*w[k];
            ++nphi;

            /* if this is the first time through the omega fit loop AND
            we are doing ACFs, NOT xcfs, and we've got enough points to
        draw a line, THEN compute a new value of omega_loc as we add each
        new point */

            if (!xflag && sum_wk2_arr[k] && (omega_old_2 == 9999.)) {
                omega_loc = sum_kphi/(t0*sum_wk2_arr[k]);
                omega_loc = (nphi*omega_loc + omega_init)/(nphi + 1);
            }
        }

        if (xflag) {
            d = determ(sum_w,sum_wk*t0,sum_wk*t0,sum_wk2*t2);
            if (d == 0) {
                free(phi_k);
                return 8;
            }
            phi_loc = determ(sum_phi,sum_wk*t0,sum_kphi*t0,sum_wk2*t2)/d;
            omega_loc = determ(sum_w,sum_phi,sum_wk*t0,sum_kphi*t0)/d;
        } else {
            phi_loc = 0;
            if (sum_wk2 <= 0.0) {
                free(phi_k);
                return 8;
            }
            omega_loc = sum_kphi/(t0*sum_wk2);
        }
    }
    /*  End of While loop */

    if (phi_loc > PI) phi_loc = phi_loc - 2*PI;

    *phi0 = phi_loc;
    *omega = omega_loc;

    /* Now we calculate the estimated error of the fit */

    wbar = 0;
    e2 = 0.;
    nphi = 0;
    for (k=0; k<mplgs; k++) {
        if (!badlag[k]) {
            e2 += w[k]*w[k]*(phi_k[k] - phi_loc - omega_loc*tau[k]*t0)*
                         (phi_k[k] - phi_loc - omega_loc*tau[k]*t0);
            wbar += w[k];
            nphi++;
        }
    }
    wbar = wbar/nphi;
    if (xflag) *sdev = sqrt(e2/(sum_w) * nphi/(nphi-2));
    else *sdev = sqrt(e2/sum_w * nphi/(nphi-1));

    if (xflag) {
        *phi0_err =  *sdev * wbar * sqrt(sum_wk2*t2/d);
        *omega_err = *sdev * wbar * sqrt(sum_w/d);
    }
    else {
        *phi0_err = 0;
        *omega_err = *sdev*wbar/sqrt(sum_wk2)/t0;
    }
    free(phi_k);
    return 0;
}




