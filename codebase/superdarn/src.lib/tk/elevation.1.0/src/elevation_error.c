/* elevation_error.c
 *===================
 * Authors: Marina Schmidt, Pasha-ponomareko 
 * 
 * This was taken from Keith Kotyk's fitacf 3.0 code and altered 
 * to meet the new moduralization design for RST
 *
 * 
 * Description:
 * References:
 *          - Numerical Recipes book on least squares fit
 *
 */

#include <math.h>
#include <stdio.h>
#include "fitblk.h"
#include "rmath.h"
#include "elevation.h"
/*
 *Elevation_error: calculates the error in the elevation angles 
 *
 *parameters:
 *      elev_data - structure containing data for elevation calculations
 *                       defined in fitdata.h 
 *      sigma_2_a      - sigma 2 of a in linear fit f = ax + b 
 *                       (nomenclature from Numerical Recipes book)
 *      slope_a        - is the slope in the linear fit f = ax + b
 *                       addative slope to the name for better description... sorry Keith
 *algorithm: 
 *      Pasha?
 *
 *returns: 
 *      elev_err - elevation error 
 */
double elevation_error(struct elevation_data *elev_data, double sigma_2_a, double slope_a)
{
    /*Defined variables for the function*/
    double elev_err; /* elevation error */
    double deriv_theta;
    double antenna_sep;
    double elev_corr;
    double azimuth_offset, cable_offset;
    double psi, psi_uncorrected, psi_kd, psi_k2d2;
    double wave_num;
    double phi_0, c_phi_0;
    double theta;
    double phase_diff_max;

    int phi_sign;
    
    /*Elevation error calculation*/
    antenna_sep = sqrt(elev_data->interfer_x*elev_data->interfer_x + elev_data->interfer_y*elev_data->interfer_y + elev_data->interfer_z*elev_data->interfer_z);
    elev_corr = elev_data->phidiff * asin(elev_data->interfer_z/antenna_sep);
    if (elev_data->interfer_y > 0.0)
    {
        phi_sign = 1;
    }
    else
    {
        phi_sign = -1;
        elev_corr = -elev_corr;
    }

    azimuth_offset = elev_data->maxbeam/2.0 - 0.5;

    phi_0 = elev_data->bmsep * (elev_data->bmnum - azimuth_offset) * M_PI/180;
    c_phi_0 = cos(phi_0);
    wave_num = 2.0 * M_PI * elev_data->tfreq * 1000/C; /* C - speed of light m/s */

    cable_offset = -2.0 * M_PI * elev_data->tfreq * 1000 * elev_data->tdiff * 1.0e-6;

    phase_diff_max = phi_sign * wave_num * antenna_sep * c_phi_0 + cable_offset;

    psi_uncorrected = slope_a + 2.0 * M_PI * floor((phase_diff_max-slope_a)/(2.0*M_PI));

    if(phi_sign < 0) 
        psi_uncorrected += 2.0 * M_PI;

    psi = psi_uncorrected - cable_offset;

    psi_kd = psi/(wave_num * antenna_sep);
    theta = c_phi_0 * c_phi_0 - psi_kd * psi_kd;
    psi_k2d2 = psi/(wave_num * wave_num * antenna_sep * antenna_sep);
    deriv_theta = psi_k2d2/sqrt(theta * (1.0 - theta));

    elev_err = 180.0/M_PI * sqrt(sigma_2_a) * fabs(deriv_theta);

    return elev_err;

}
