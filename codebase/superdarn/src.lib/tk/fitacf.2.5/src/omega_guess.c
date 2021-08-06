/* omega_guess.c
   =============
   Author: R.J.Barnes & K.Baker
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
 
    2020-11-12 Marina Schmidt Converted RST complex -> C library complex

 
*/




/* Revision 1.4 corrected the method of weighting the
   estimate for omega to be consistent with the way
   it is done in fit_acf and do_phase_fit.

   The error on any given point is estimated to be
   delta_phase[i] = <delta_phase>*<P>/P[i]
*/

#include <math.h>
#include <complex.h>
#include "rmath.h"

double omega_guess(double complex *acf,double *tau, int *badlag,
        double *phi_res, double *omega_err,int mpinc,int mplgs) {

    int i,j,nave=0;
    double delta_tau, delta_phi, omega=0.0, 
         omega2=0.0, average=0.0, sigma, tau_lim=1.0;
    double sum_W=0.0, W, W2;
    register double temp;
    double two_sigma;

    two_sigma = sigma = 2*PI;
    *omega_err = 9999.;

    while (tau_lim < 3 && nave < 3) {
        for (j=1; j<=tau_lim; ++j) {
            for (i=0; i< mplgs - j; ++i) {
                /*calculate the sums for the initial guess of the phase slope*/
                if (badlag[i+j] || badlag[i]) {
                    continue;
                }
                delta_tau = tau[i+j] - tau[i];
                if (delta_tau != tau_lim) {
                    continue;
                }
                delta_phi = phi_res[i+j] - phi_res[i];

                /*calculate the weights for the weighted average 
                    as the mean power between consecutive lags*/
                W = (cabs(acf[i]) + cabs(acf[i+j]))/2.0;
                W2 = W*W;

                /*pi correction*/
                if (delta_phi > PI) {
                    delta_phi = delta_phi - 2*PI;
                }
                if (delta_phi < -PI) {
                    delta_phi = delta_phi + 2*PI;
                }

                /**/
                if ((average != 0.0) && (fabs(delta_phi - average) > two_sigma)) {
                    continue;
                }
                temp = delta_phi/tau_lim;
                omega = omega + temp*W2;
                omega2 = omega2 + W2*(temp*temp);
                sum_W = sum_W + W2;
                nave++;
            }
        }

        if (nave >= 3 && (sigma == 2*PI)) {
            average = omega/sum_W;
            sigma = ((omega2/sum_W) - average*average)/(nave-1);
            sigma = (sigma > 0.0) ? sqrt(sigma) : 0.0;
            two_sigma = 2.0*sigma;
            omega = 0.0;
            omega2 = 0.0; 
            sum_W = 0;
            nave = 0;
            tau_lim = 1;
        } else if (nave >=3) {
            omega = omega/sum_W;
            omega = omega/(mpinc*1.0e-6);
            sigma = ((omega2/sum_W) - average*average)/(nave-1);
            sigma = (sigma > 0.0) ? sqrt(sigma) : 0.0;
            *omega_err = sigma/(mpinc*1.0e-6);
            return omega;
        }
        else {
            ++tau_lim;
        }
    }

    return 0.0;
}
