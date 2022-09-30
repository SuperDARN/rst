/* selfclutter.c
   ==============

 Copyright (c) 2014 University of Saskatchewan
 Author: A.S.Reimer
 
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

*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "rtypes.h"
#include "error_estimates.h"

/* Lag0 power minus noise power error estimate:
  calculates the error in the lag0 power estimate given by

        P = 1/K sum( |V_i|^2 ) - 1/Kn sum( |V_n|^2 )

  where K is nave and subscript i is for voltage samples for lag0 power
  where Kn is nave for noise voltage samples and subscript n is for the
  noise voltage samples.
*/
int lag0_error(int nrang, float *pwr0, float noise_power, float K, float Kn, float *error) {

    int i;
    for (i=0;i<nrang;i++){
        error[i] = sqrt( (1.0/K) * pow(pwr0[i],2) + (1.0/Kn) * pow(noise_power,2) );
    }
    return 0;
}

/* calculates the error in the normalized acf estimator as given by Farley 1969 equation (16) for rho_1 */
int norm_acf_error1(int nrang, float *pwr0, float noise_power, float *selfclutter, float *acf, float K, float Kn, float *error) {

    int i;
    float rho_p;
    for (i=0;i<nrang;i++){
        rho_p = (acf[i]*pwr0[i])/(pwr0[i] + noise_power + selfclutter[i]);
        error[i] = sqrt( (1.0/K) * pow(((pwr0[i] + noise_power + selfclutter[i])/pwr0[i]),2)*(1.0 + pow(acf[i],2) + pow(rho_p,2) + pow(acf[i]*rho_p,2) - 4.0*acf[i]*rho_p ) + (1.0/Kn)*pow(noise_power/pwr0[i],2)*(1.0 + pow(acf[i],2)) );
    }

    return 0;
}


/* calculates the error in the normalized acf estimator as given by Farley 1969 equation (16) for rho_2 */
int norm_acf_error2(int nrang, float *pwr0, float noise_power, float *selfclutter, float *acf, float K, float Kn, float *error) {

    int i;
    float rho_p;
    for (i=0;i<nrang;i++){
        rho_p = (acf[i]*pwr0[i])/(pwr0[i] + noise_power + selfclutter[i]);
        error[i] = sqrt( (1.0/K) * pow(((pwr0[i] + noise_power + selfclutter[i])/pwr0[i]),2)*(1.0 + 2.0*pow(acf[i],2) + pow(rho_p,2) - 4.0*acf[i]*rho_p ) + (1.0/Kn)*pow(noise_power/pwr0[i],2)*(1.0 + 2.0*pow(acf[i],2)) );
    }

    return 0;
}

/* Unnormalized ACF error estimator in the presense of self-clutter
  calculates the error in the unnormalized acf estimator with self-clutter
  using the equation given by:

        ACF = 1/K sum( (V_1i)(V_2i)* )

  where * denotes the complex conjugate and indicies 1 and 2 denote voltage 
  samples at times 1 and 2 respectively.
*/
int acf_error(int mplgs, float pwr0, float noise_power, float *selfclutter, float K, float *error) {

    int i;
    for (i=0;i<mplgs;i++){
        error[i] = sqrt(1.0/K) * (pwr0 + noise_power + selfclutter[i]);
    }

    return 0;
}


