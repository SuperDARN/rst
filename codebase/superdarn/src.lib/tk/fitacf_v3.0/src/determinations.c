/*Copyright (C) 2016  SuperDARN Canada, University of Saskatchewan

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

/*
ACF determinations from fitted parameters

author(s): Keith Kotyk
modifications: 
    2020-03-11 Marina Schmidt (SuperDARN Canada) removed all defined constants 
                              and include rmath.h

*/


#include "rtypes.h"
#include "determinations.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rmath.h"

/**
 * @brief      Allocates space needed for final data parameters.
 *
 * @param      fit_data  The FitData struct that holds parameters that have been extracted from
 *                       fitted data.
 * @param      fit_prms  The FITPRM struct holding rawacf record info.
 */
void allocate_fit_data(struct FitData* fit_data, FITPRMS* fit_prms){
    fit_data->rng = realloc(fit_data->rng,fit_prms->nrang * sizeof(*fit_data->rng));
    if(fit_data->rng == NULL){
        fprintf(stderr,"COULD NOT ALLOCATE fit_data->rng\n");
    }
    else{
        memset(fit_data->rng,0,sizeof(*fit_data->rng) * fit_prms->nrang);
    }

    fit_data->xrng = realloc(fit_data->xrng,fit_prms->nrang * sizeof(*fit_data->xrng));
    if(fit_data->xrng == NULL){
        fprintf(stderr,"COULD NOT ALLOCATE fit_data->xrng\n");
    }
    else{
        memset(fit_data->xrng,0,sizeof(*fit_data->xrng) * fit_prms->nrang);
    }

    fit_data->elv = realloc(fit_data->elv,fit_prms->nrang * sizeof(*fit_data->elv));
    if (fit_data->elv == NULL){
        fprintf(stderr,"COULD NOT ALLOCATE fit_data->xrng\n");
    }
    else{
        memset(fit_data->elv,0,sizeof(*fit_data->elv) * fit_prms->nrang);
    }

}

/**
 * @brief      Calls the overall set of functions that extract final data parameters from the
 *             fitted data.
 *
 * @param[in]  ranges     The list of RANGENODE structs.
 * @param      fit_prms   The FITPRM struct holding rawacf record info.
 * @param      fit_data   The FitData struct that holds parameters that have been extracted from
 *                        fitted data.
 * @param[in]  noise_pwr  The noise power.
 */
void ACF_Determinations(llist ranges, FITPRMS* fit_prms,struct FitData* fit_data,double noise_pwr){

    fit_data->revision.major=3;
    fit_data->revision.minor=0;

    allocate_fit_data(fit_data,fit_prms);

    fit_data->noise.vel = 0.0;
    fit_data->noise.skynoise = noise_pwr;
    fit_data->noise.lag0 = 0.0;

    lag_0_pwr_in_dB(fit_data->rng,fit_prms,noise_pwr);

    llist_for_each_arg(ranges,(node_func_arg)set_xcf_phi0,fit_data,fit_prms);
    llist_for_each_arg(ranges,(node_func_arg)find_elevation,fit_data,fit_prms); 
    llist_for_each_arg(ranges,(node_func_arg)set_xcf_phi0_err,fit_data->xrng,NULL);  
    llist_for_each_arg(ranges,(node_func_arg)set_xcf_sdev_phi,fit_data->xrng,NULL);


#ifdef _RFC_IDX
    llist_for_each_arg(ranges,(node_func_arg)refractive_index,fit_data->elv,NULL);
#endif

    llist_for_each_arg(ranges,(node_func_arg)set_qflg,fit_data->rng,NULL);
    llist_for_each_arg(ranges,(node_func_arg)set_p_l,fit_data->rng,&noise_pwr);
    llist_for_each_arg(ranges,(node_func_arg)set_p_l_err,fit_data->rng,&noise_pwr);
    llist_for_each_arg(ranges,(node_func_arg)set_p_s,fit_data->rng,&noise_pwr);
    llist_for_each_arg(ranges,(node_func_arg)set_p_s_err,fit_data->rng,&noise_pwr);
    llist_for_each_arg(ranges,(node_func_arg)set_v,fit_data->rng,fit_prms);
    llist_for_each_arg(ranges,(node_func_arg)set_v_err,fit_data->rng,fit_prms);
    llist_for_each_arg(ranges,(node_func_arg)set_w_l,fit_data->rng,fit_prms);
    llist_for_each_arg(ranges,(node_func_arg)set_w_l_err,fit_data->rng,fit_prms);
    llist_for_each_arg(ranges,(node_func_arg)set_w_s,fit_data->rng,fit_prms);
    llist_for_each_arg(ranges,(node_func_arg)set_w_s_err,fit_data->rng,fit_prms);
    llist_for_each_arg(ranges,(node_func_arg)set_sdev_l,fit_data->rng,NULL);
    llist_for_each_arg(ranges,(node_func_arg)set_sdev_s,fit_data->rng,NULL);
    llist_for_each_arg(ranges,(node_func_arg)set_sdev_phi,fit_data->rng,NULL);
    llist_for_each_arg(ranges,(node_func_arg)set_gsct,fit_data->rng,NULL);
    llist_for_each_arg(ranges,(node_func_arg)set_nump,fit_data->rng,NULL);

}

/**
 * @brief      Calculates refractive index for a given range.
 *
 * @param[in]  range           A RANGENODE struct.
 * @param      fit_elev_array  A FitElv array that holds calculated elevation.
 *
 * This function is meant to be mapped to a list of ranges using llist_for_each.
 */
void refractive_index(llist_node range, struct FitElv* fit_elev_array){
    double height;
    RANGENODE *range_node;
    double cos_elev_angle;
    double height_ratio;

    range_node = (RANGENODE*) range;

    height = (range_node->range <= 10) ? CLOSE_GATE_HEIGHT : FAR_GATE_HEIGHT;

    cos_elev_angle = cos(PI/180 * fit_elev_array[range_node->range].normal);

    height_ratio = (RE/(RE + height));

    range_node->refrc_idx =  height_ratio * (cos_elev_angle/MAGNETIC_DIP_ANGLE);


}

/**
 * @brief      Converts lag 0 power to dB.
 *
 * @param      fit_range_array  This struct holds fit results and is used by RST to write out final
 *                              final results.
 * @param      fit_prms         The FITPRM struct holding rawacf record info.
 * @param[in]  noise_pwr        The noise power
 *
 * An arbitrary value of -50 is assigned in the case that the lag 0 power minus the noise is below
 * 0.0.
 */
void lag_0_pwr_in_dB(struct FitRange* fit_range_array,FITPRMS* fit_prms,double noise_pwr){
    int i;

    for(i=0;i<fit_prms->nrang;i++){
        if((fit_prms->pwr0[i] - noise_pwr) > 0.0){
            fit_range_array[i].p_0 = 10 * log10((fit_prms->pwr0[i] - noise_pwr) / noise_pwr);
        }
        else{
            fit_range_array[i].p_0 = -50.0;
        }
    }
}

/**
 * @brief      Sets a flag showing that data for a range is valid.
 *
 * @param[in]  range            A RANGENODE struct.
 * @param      fit_range_array  This struct holds fit results and is used by RST to write out final
 *                              final results.
 *
 * This function is meant to be mapped to a list of ranges using llist_for_each.
 */
void set_qflg(llist_node range,struct FitRange* fit_range_array){
    RANGENODE* range_node;

    range_node = (RANGENODE*) range;

    fit_range_array[range_node->range].qflg = 1;

}

/**
 * @brief      Sets the value of the linear fitted lag 0 power in dB
 *
 * @param[in]  range            The FITPRM struct holding rawacf record info.
 * @param      fit_range_array  This struct holds fit results and is used by RST to write out final
 *                              final results.
 * @param      noise_pwr        The noise power.
 *
 * This function is meant to be mapped to a list of ranges using llist_for_each.
 */
void set_p_l(llist_node range, struct FitRange* fit_range_array, double* noise_pwr){
    RANGENODE* range_node;
    double noise_dB;

    range_node = (RANGENODE*) range;
    noise_dB = 10 * log10( *noise_pwr);

    fit_range_array[range_node->range].p_l = 10 * range_node->l_pwr_fit->a * LN_TO_LOG - noise_dB;

}

/**
 * @brief      Sets the value of the linear fitted lag 0 power error in dB
 *
 * @param[in]  range            A RANGENODE struct.
 * @param      fit_range_array  This struct holds fit results and is used by RST to write out final
 *                              final results.
 *
 * This function is meant to be mapped to a list of ranges using llist_for_each.
 */
void set_p_l_err(llist_node range, struct FitRange* fit_range_array){
    RANGENODE* range_node;

    range_node = (RANGENODE*) range;

    fit_range_array[range_node->range].p_l_err = 10 * sqrt(range_node->l_pwr_fit_err->sigma_2_a) * LN_TO_LOG;  /* Here pwr_fit_err!*/

}

/**
 * @brief      Sets the value of the quadratic fitted lag 0 power in dB.
 *
 * @param[in]  range            A RANGENODE struct.
 * @param      fit_range_array  This struct holds fit results and is used by RST to write out final
 *                              final results.
 * @param      noise_pwr        The noise power.
 *
 * This function is meant to be mapped to a list of ranges using llist_for_each.
 */
void set_p_s(llist_node range, struct FitRange* fit_range_array, double* noise_pwr){
    RANGENODE* range_node;
    double noise_dB;

    range_node = (RANGENODE*) range;
    noise_dB = 10 * log10( *noise_pwr);

    fit_range_array[range_node->range].p_s = 10 * range_node->q_pwr_fit->a * LN_TO_LOG - noise_dB;

}

/**
 * @brief      Sets the value of the quadratic fitted lag 0 power error in dB.
 *
 * @param[in]  range            A RANGENODE struct.
 * @param      fit_range_array  This struct holds fit results and is used by RST to write out final
 *                              final results.
 *
 * This function is meant to be mapped to a list of ranges using llist_for_each.
 */
void set_p_s_err(llist_node range, struct FitRange* fit_range_array){
    RANGENODE* range_node;

    range_node = (RANGENODE*) range;

    fit_range_array[range_node->range].p_s_err = 10 * sqrt(range_node->q_pwr_fit_err->sigma_2_a) * LN_TO_LOG;

}

/**
 * @brief      Sets the value of the determined velocity from the phase fit.
 *
 * @param[in]  range            A RANGENODE struct.
 * @param      fit_range_array  This struct holds fit results and is used by RST to write out final
 *                              final results.
 * @param      fit_prms         The FITPRM struct holding rawacf record info.
 *
 * This function is meant to be mapped to a list of ranges using llist_for_each.
 */
void set_v(llist_node range, struct FitRange* fit_range_array, FITPRMS* fit_prms){
    RANGENODE* range_node;
    double conversion_factor,velocity;

    range_node = (RANGENODE*) range;

    conversion_factor = C/((4*PI)*(fit_prms->tfreq * 1000.0)) * fit_prms->vdir;

    velocity = range_node->phase_fit->b * conversion_factor * (1/range_node->refrc_idx);

    fit_range_array[range_node->range].v = velocity;
}

/**
 * @brief      Sets the value of the determined velocity error from the phase fit.
 *
 * @param[in]  range            A RANGENODE struct.
 * @param      fit_range_array  This struct holds fit results and is used by RST to write out final
 *                              final results.
 * @param      fit_prms         The FITPRM struct holding rawacf record info.
 *
 * This function is meant to be mapped to a list of ranges using llist_for_each.
 */
void set_v_err(llist_node range, struct FitRange* fit_range_array, FITPRMS* fit_prms){
    RANGENODE* range_node;
    double conversion_factor,velocity_err;

    range_node = (RANGENODE*) range;
    conversion_factor = C/((4*PI)*(fit_prms->tfreq * 1000.0));

    velocity_err = sqrt(range_node->phase_fit->sigma_2_b) * conversion_factor * (1/range_node->refrc_idx);

    fit_range_array[range_node->range].v_err = velocity_err;
}

/**
 * @brief      Sets the value of the determined spectral width from the linear power fit.
 *
 * @param[in]  range            A RANGENODE struct.
 * @param      fit_range_array  This struct holds fit results and is used by RST to write out final
 *                              final results.
 * @param      fit_prms         The FITPRM struct holding rawacf record info.
 *
 * This function is meant to be mapped to a list of ranges using llist_for_each.
 */
void set_w_l(llist_node range, struct FitRange* fit_range_array, FITPRMS* fit_prms){
    RANGENODE* range_node;
    double conversion_factor;

    range_node = (RANGENODE*) range;
    conversion_factor = C/((4*PI)*(fit_prms->tfreq * 1000.0))*2.;

    fit_range_array[range_node->range].w_l = fabs(range_node->l_pwr_fit->b) * conversion_factor;
}

/**
 * @brief      Sets the value of the determined spectral width error from the linear power fit.
 *
 * @param[in]  range            A RANGENODE struct.
 * @param      fit_range_array  This struct holds fit results and is used by RST to write out final
 *                              final results.
 * @param      fit_prms         The FITPRM struct holding rawacf record info.
 *
 * This function is meant to be mapped to a list of ranges using llist_for_each.
 */
void set_w_l_err(llist_node range, struct FitRange* fit_range_array, FITPRMS* fit_prms){
    RANGENODE* range_node;
    double conversion_factor;

    range_node = (RANGENODE*) range;
    conversion_factor = C/(4*PI)/(fit_prms->tfreq * 1000.0)*2.;

    fit_range_array[range_node->range].w_l_err = sqrt(range_node->l_pwr_fit_err->sigma_2_b) * conversion_factor;
}

/**
 * @brief       Sets the value of the determined spectral width from the quadratic power fit.
 *
 * @param[in]  range            A RANGENODE struct.
 * @param      fit_range_array  This struct holds fit results and is used by RST to write out final
 *                              final results.
 * @param      fit_prms         The FITPRM struct holding rawacf record info.
 *
 * This function is meant to be mapped to a list of ranges using llist_for_each.
 */
void set_w_s(llist_node range, struct FitRange* fit_range_array, FITPRMS* fit_prms){
    RANGENODE* range_node;
    double conversion_factor;
    double fit_sqrt;

    range_node = (RANGENODE*) range;
    conversion_factor = C/(4*PI)/(fit_prms->tfreq * 1000.0) *4.* sqrt(log(2));

    fit_sqrt = sqrt(fabs(range_node->q_pwr_fit->b));
    fit_range_array[range_node->range].w_s = fit_sqrt * conversion_factor;
}

/**
 * @brief      Sets the value of the determined spectral width error from the quadratic power fit.
 *
 * @param[in]  range            A RANGENODE struct.
 * @param      fit_range_array  This struct holds fit results and is used by RST to write out final
 *                              final results.
 * @param      fit_prms         The FITPRM struct holding rawacf record info.
 *
 * This function is meant to be mapped to a list of ranges using llist_for_each.
 */
void set_w_s_err(llist_node range, struct FitRange* fit_range_array, FITPRMS* fit_prms){
    RANGENODE* range_node;
    double conversion_factor;
    double fit_sqrt, fit_std_dev, w_s_err;
    range_node = (RANGENODE*) range;

    conversion_factor = C/(4*PI)/(fit_prms->tfreq * 1000.0) * 4.*sqrt(log(2));
    fit_std_dev = sqrt(range_node->q_pwr_fit_err->sigma_2_b);
    fit_sqrt = sqrt(fabs(range_node->q_pwr_fit->b));
    w_s_err =  fit_std_dev/2./fit_sqrt * conversion_factor;
    fit_range_array[range_node->range].w_s_err = w_s_err;
}

/**
 * @brief      Sets the value of chi squared from the linear power fit.
 *
 * @param[in]  range            A RANGENODE struct.
 * @param      fit_range_array  This struct holds fit results and is used by RST to write out final
 *                              final results.
 * @param      fit_prms         The FITPRM struct holding rawacf record info.
 *
 * This function is meant to be mapped to a list of ranges using llist_for_each.
 */
void set_sdev_l(llist_node range, struct FitRange* fit_range_array){
    RANGENODE* range_node;

    range_node = (RANGENODE*) range;

    fit_range_array[range_node->range].sdev_l = range_node->l_pwr_fit->chi_2;
}

/**
 * @brief      Sets the value of chi squared from the quadratic power fit.
 *
 * @param[in]  range            A RANGENODE struct.
 * @param      fit_range_array  This struct holds fit results and is used by RST to write out final
 *                              final results.
 * @param      fit_prms         The FITPRM struct holding rawacf record info.
 *
 * This function is meant to be mapped to a list of ranges using llist_for_each.
 */
void set_sdev_s(llist_node range, struct FitRange* fit_range_array){
    RANGENODE* range_node;

    range_node = (RANGENODE*) range;

    fit_range_array[range_node->range].sdev_s = range_node->q_pwr_fit->chi_2;
}

/**
 * @brief      Sets the value of chi squared from the phase fit.
 *
 * @param[in]  range            A RANGENODE struct.
 * @param      fit_range_array  This struct holds fit results and is used by RST to write out final
 *                              final results.
 * @param      fit_prms         The FITPRM struct holding rawacf record info.
 *
 * This function is meant to be mapped to a list of ranges using llist_for_each.
 */
void set_sdev_phi(llist_node range, struct FitRange* fit_range_array){
    RANGENODE* range_node;

    range_node = (RANGENODE*) range;

    fit_range_array[range_node->range].sdev_phi = range_node->phase_fit->chi_2;
}

/**
 * @brief      Sets the flag of whether a range is ground scatter.
 *
 * @param[in]  range            A RANGENODE struct.
 * @param      fit_range_array  This struct holds fit results and is used by RST to write out final
 *                              final results.
 * @param      fit_prms         The FITPRM struct holding rawacf record info.
 *
 * This function is meant to be mapped to a list of ranges using llist_for_each.
 */
void set_gsct(llist_node range, struct FitRange* fit_range_array){
    RANGENODE* range_node;
    double v_abs,w;

    range_node = (RANGENODE*) range;

    v_abs = fabs(fit_range_array[range_node->range].v);
    w = fit_range_array[range_node->range].w_l;
    fit_range_array[range_node->range].gsct = (v_abs - (V_max - w * (V_max/W_max)) < 0) ? 1 : 0;
}

/**
 * @brief      Sets the number of good points used in the power fitting.
 *
 * @param[in]  range            A RANGENODE struct.
 * @param      fit_range_array  This struct holds fit results and is used by RST to write out final
 *                              final results.
 * @param      fit_prms         The FITPRM struct holding rawacf record info.
 *
 * This function is meant to be mapped to a list of ranges using llist_for_each.
 */
void set_nump(llist_node range, struct FitRange* fit_range_array){
    RANGENODE* range_node;

    range_node = (RANGENODE*) range;

    fit_range_array[range_node->range].nump = llist_size(range_node->pwrs);
}

/**
 * @brief      Determines the elevation angle from the fitted XCF phase.
 *
 * @param[in]  range            A RANGENODE struct.
 * @param      fit_range_array  This struct holds fit results and is used by RST to write out final
 *                              final results.
 * @param      fit_prms         The FITPRM struct holding rawacf record info.
 *
 * This function is meant to be mapped to a list of ranges using llist_for_each.
 */
void find_elevation(llist_node range, struct FitData* fit_data, FITPRMS* fit_prms){
    double x,y,z;
    double antenna_sep,elev_corr;
    int phi_sign;
    double azi_offset,phi_0,c_phi_0;
    double wave_num;
    double cable_offset;
    double phase_diff_max;
    double psi_uncorrected;
    double psi,theta,psi_kd,psi_k2d2,df_by_dy;
    double elevation;
    double psi_uncorrected_unfitted;

    RANGENODE* range_node;

    range_node = (RANGENODE*) range;


    x = fit_prms->interfer_x;
    y = fit_prms->interfer_y;
    z = fit_prms->interfer_z;

    antenna_sep = sqrt(x*x + y*y + z*z);

    elev_corr = asin(z/antenna_sep);
    if (y > 0.0){
        phi_sign = 1;
    }
    else{
        phi_sign = -1;
        elev_corr = -elev_corr;
    }

    azi_offset = fit_prms->maxbeam/2 - 0.5;
    phi_0 = fit_prms->bmsep * (fit_prms->bmnum - azi_offset) * PI/180;
    c_phi_0 = cos(phi_0);

    wave_num = 2 * PI * fit_prms->tfreq * 1000/C;

    cable_offset = -2 * PI * fit_prms->tfreq * 1000 * fit_prms->tdiff * 1.0e-6;

    phase_diff_max = phi_sign * wave_num * antenna_sep * c_phi_0 + cable_offset;

    psi_uncorrected = range_node->elev_fit->a + 2 * PI * floor((phase_diff_max-range_node->elev_fit->a)/(2*PI));

    if(phi_sign < 0) psi_uncorrected += 2 * PI;

    psi = psi_uncorrected - cable_offset;

    psi_kd = psi/(wave_num * antenna_sep);
    theta = c_phi_0 * c_phi_0 - psi_kd * psi_kd;
    if( (theta < 0.0) || (fabs(theta) > 1.0) ){
        elevation = -elev_corr;
    }
    else{
        elevation = asin(sqrt(theta));
    }

    fit_data->elv[range_node->range].high = 180/PI * (elevation + elev_corr);

    /*Elevation errors*/
    psi_k2d2 = psi/(wave_num * wave_num * antenna_sep * antenna_sep);
    df_by_dy = psi_k2d2/sqrt(theta * (1 - theta));
    fit_data->elv[range_node->range].low = 180/PI * sqrt(range_node->elev_fit->sigma_2_a) * fabs(df_by_dy);

    /*Experiment to compare fitted and measured elevation*/
    psi_uncorrected_unfitted = fit_data->xrng[range_node->range].phi0 + 2 * PI * floor((phase_diff_max-fit_data->xrng[range_node->range].phi0)/(2*PI));


    if(phi_sign < 0) psi_uncorrected_unfitted += 2 * PI;

    psi = psi_uncorrected_unfitted - cable_offset;

    psi_kd = psi/(wave_num * antenna_sep);
    theta = c_phi_0 * c_phi_0 - psi_kd * psi_kd;

    if( (theta < 0.0) || (fabs(theta) > 1.0) ){
        elevation = -elev_corr;
    }
    else{
        elevation = asin(sqrt(theta));
    }
    fit_data->elv[range_node->range].normal = 180/PI * (elevation + elev_corr);

}

/**
 * @brief      Sets the phase offset for the XCF from raw data.
 *
 * @param[in]  range            A RANGENODE struct.
 * @param      fit_range_array  This struct holds fit results and is used by RST to write out final
 *                              final results.
 * @param      fit_prms         The FITPRM struct holding rawacf record info.
 *
 * This function is meant to be mapped to a list of ranges using llist_for_each.
 */
void set_xcf_phi0(llist_node range, struct FitData* fit_data, FITPRMS* fit_prms){
    RANGENODE* range_node;
    double real, imag;

    range_node = (RANGENODE*) range;

    real = fit_prms->xcfd[range_node->range * fit_prms->mplgs][0];
    imag = fit_prms->xcfd[range_node->range * fit_prms->mplgs][1];

    /* Correct phase sign due to cable swapping */
    fit_data->xrng[range_node->range].phi0 = atan2(imag,real)*fit_prms->phidiff;
    range_node->elev_fit->a *= fit_prms->phidiff;
}


/**
 * @brief      Sets the phase offset error for the XCF from the XCF fit.
 *
 * @param[in]  range            A RANGENODE struct.
 * @param      fit_range_array  This struct holds fit results and is used by RST to write out final
 *                              final results.
 *
 * This function is meant to be mapped to a list of ranges using llist_for_each.
 */
void set_xcf_phi0_err(llist_node range, struct FitRange* fit_range_array){
    RANGENODE* range_node;

    range_node = (RANGENODE*) range;

    fit_range_array[range_node->range].phi0_err = sqrt(range_node->elev_fit->sigma_2_a);

}

/**
 * @brief      Sets the fitted phase chi squared value for the XCF.
 *
 * @param[in]  range            A RANGENODE struct.
 * @param      fit_range_array  This struct holds fit results and is used by RST to write out final
 *                              final results.
 *
 * This function is meant to be mapped to a list of ranges using llist_for_each.
 */
void set_xcf_sdev_phi(llist_node range, struct FitRange* fit_range_array){
    RANGENODE* range_node;

    range_node = (RANGENODE*) range;

    fit_range_array[range_node->range].sdev_phi = range_node->elev_fit->chi_2;

}










