/*
 ACF determinations from fitted parameters

 Copyright (c) 2015 University of Saskatchewan
 author: Keith Kotyk


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
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 Modifications: 
     2020-03-11 Marina Schmidt (University of Saskatchewan) removed all defined constants and include rmath.h
     2020-08-12 Marina Schmidt (University of Saskatchewan) removed map function for better decoupling abilities
     2020-10-29 Marina Schmidt (University of Saskatchewan) & Emma Bland (UNIS) 
                Changed default elevation calculation to elevation_v2()
     2021-06-01 Emma Bland (UNIS) Consolidated elevation angle calculations into a single function
     E.G.Thomas 2021-08: added support for bmoff parameter
     2021-11-12 Emma Bland (UNIS) Changed elevation angle field names (elv.high --> elv.fitted, elv.low --> elv.error)
*/


#include "llist.h"
#include "rtypes.h"
#include "determinations.h"
#include "fitblk.h"
#include "elevation.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rmath.h"
/**
 * Allocates space needed for final data parameters.
 *
 * @param fit_data:  The FitData struct that holds parameters that have been extracted from
 *                       fitted data.
 * @param fit_prms:  The FITPRM struct holding rawacf record info.
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
void ACF_Determinations(llist ranges, FITPRMS* fit_prms,struct FitData* fit_data,double noise_pwr, int elv_version){
    int list_null_flag = LLIST_SUCCESS;
    fit_data->revision.major=3;
    fit_data->revision.minor=0;

    allocate_fit_data(fit_data,fit_prms);

    llist_node node; 
    fit_data->noise.vel = 0.0;
    fit_data->noise.skynoise = noise_pwr;
    fit_data->noise.lag0 = 0.0;

    lag_0_pwr_in_dB(fit_data->rng,fit_prms,noise_pwr);
    
    if (ranges == NULL)
    {
        fprintf(stderr, "List is empty\n");
    }
    // get the first node of the list called the head 
    llist_reset_iter(ranges);
    llist_get_iter(ranges, &node);
    while(node != NULL && list_null_flag == LLIST_SUCCESS)
    {
       set_xcf_phi0(node, fit_data, fit_prms); 
       list_null_flag = llist_go_next(ranges);
       llist_get_iter(ranges, &node); 
    }
    list_null_flag = LLIST_SUCCESS;
    llist_reset_iter(ranges);
    llist_get_iter(ranges, &node); 
    while(node != NULL && list_null_flag == LLIST_SUCCESS)
    {
       find_elevation(node, fit_data, fit_prms, elv_version);
       find_elevation_error(node, fit_data, fit_prms);
       list_null_flag = llist_go_next(ranges);
       llist_get_iter(ranges, &node); 

    }
    list_null_flag = LLIST_SUCCESS;
    llist_reset_iter(ranges);
    llist_get_iter(ranges, &node);
    while(node != NULL && list_null_flag == LLIST_SUCCESS)
    {
        set_xcf_phi0_err(node, fit_data->xrng);
        set_xcf_sdev_phi(node, fit_data->xrng);

        // if refractive index is not set to a constant
        // then calculate it
        #ifdef _RFC_IDX
            refractive_index(node, fit_data->elv);
        #endif

        // setting quality flag
        set_qflg(node, fit_data->rng);
        
        // setting Signal-to-Noise fields (dB)
        set_p_l(node, fit_data->rng, &noise_pwr);
        set_p_l_err(node, fit_data->rng);
        set_p_s(node, fit_data->rng, &noise_pwr);
        set_p_s_err(node, fit_data->rng);

        // setting velocity fields (m/s)
        set_v(node, fit_data->rng, fit_prms);
        set_v_err(node, fit_data->rng, fit_prms);

        // setting the spectral width fields (m/s)
        set_w_l(node, fit_data->rng, fit_prms);
        set_w_l_err(node, fit_data->rng, fit_prms);
        set_w_s(node, fit_data->rng, fit_prms);
        set_w_s_err(node, fit_data->rng, fit_prms);

        // setting standard deviation fields 
        set_sdev_l(node, fit_data->rng);
        set_sdev_s(node, fit_data->rng);
        set_sdev_phi(node, fit_data->rng);
        
        // setting ground scatter field
        set_gsct(node, fit_data->rng);

        // TODO: ???
        set_nump(node, fit_data->rng);

       list_null_flag = llist_go_next(ranges);
       llist_get_iter(ranges, &node); 
    }
    llist_reset_iter(ranges);
}

/**
 * @brief      Calculates refractive index for a given range.
 *
 * @param[in]  range           A RANGENODE struct.
 * @param      fit_elev_array  A FitElv array that holds calculated elevation.
 *
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
 */
void set_nump(llist_node range, struct FitRange* fit_range_array){
    RANGENODE* range_node;

    range_node = (RANGENODE*) range;

    fit_range_array[range_node->range].nump = llist_size(range_node->pwrs);
}

/**
 * @brief      Determines the elevation angle from (1) lag zero phase and (2) fitted XCF phase.
 *
 * @param[in]  range            A RANGENODE struct.
 * @param      fit_range_array  This struct holds fit results and is used by RST to write out final
 *                              final results.
 * @param      fit_prms         The FITPRM struct holding rawacf record info.
 *
 */
void find_elevation(llist_node range, struct FitData* fit_data, FITPRMS* fit_prms, int elv_version){

    RANGENODE* range_node;
    range_node = (RANGENODE*) range;
    struct FitPrm* fitprm;
    fitprm = malloc(sizeof(struct FitPrm));
    
    // need this for elevation algorithms to work. 
    // TODO: make consistent structs between fitacf versions so we don't have this problem again
    fitprm->interfer[0] = fit_prms->interfer[0];
    fitprm->interfer[1] = fit_prms->interfer[1];
    fitprm->interfer[2] = fit_prms->interfer[2];
    fitprm->maxbeam = fit_prms->maxbeam;
    fitprm->bmoff = fit_prms->bmoff;
    fitprm->bmsep = fit_prms->bmsep;
    fitprm->bmnum = fit_prms->bmnum;
    fitprm->tfreq = fit_prms->tfreq;
    fitprm->tdiff = fit_prms->tdiff;
    fitprm->phidiff = fit_prms->phidiff;

    // elevation angle calculated from the lag zero phase is stored in fit_data->elv[range_node->range].normal
    // elevation angle calculated from the fitted phase is stored in fit_data->elv[range_node->range].fitted
    // elv_version 2 is the Shepherd [2017] elevation calculation
    // elv_version 1 is original elevation calculation
    // elv_version 0 is specifically for the GBR radar when -old_elev is specified
    if (elv_version == 2)
    {
        fit_data->elv[range_node->range].normal = elevation_v2( fitprm, fit_data->xrng[range_node->range].phi0);
        fit_data->elv[range_node->range].fitted = elevation_v2( fitprm, range_node->elev_fit->a);
    }
    else if (elv_version == 1)
    {
        fit_data->elv[range_node->range].normal = elevation( fitprm, fit_data->xrng[range_node->range].phi0);
        fit_data->elv[range_node->range].fitted = elevation( fitprm, range_node->elev_fit->a);
    }
    else if (elv_version == 0)
    {
        fit_data->elv[range_node->range].normal = elev_goose( fitprm, fit_data->xrng[range_node->range].phi0);
        fit_data->elv[range_node->range].fitted = elev_goose( fitprm, range_node->elev_fit->a);
    }
    else
    {
        fprintf(stderr, "Error: Elevation version does not exist\n");
    }
    free(fitprm);
    
}


// TODO Integrate this calculation into find_elevation() - requires update to elevation library
void find_elevation_error(llist_node range, struct FitData* fit_data, FITPRMS* fit_prms)
{
    
    double x,y,z;
    double antenna_sep,elev_corr;
    int phi_sign;
    double azi_offset,phi_0,c_phi_0;
    double wave_num;
    double cable_offset;
    double phase_diff_max;
    double psi_uncorrected;
    double psi,theta,psi_kd,psi_k2d2,df_by_dy;


    RANGENODE* range_node;


    range_node = (RANGENODE*) range;


    x = fit_prms->interfer[0];
    y = fit_prms->interfer[1];
    z = fit_prms->interfer[2];

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
    phi_0 = (fit_prms->bmoff + fit_prms->bmsep * (fit_prms->bmnum - azi_offset)) * PI/180;
    c_phi_0 = cos(phi_0);

    wave_num = 2 * PI * fit_prms->tfreq * 1000/C;

    cable_offset = -2 * PI * fit_prms->tfreq * 1000 * fit_prms->tdiff * 1.0e-6;

    phase_diff_max = phi_sign * wave_num * antenna_sep * c_phi_0 + cable_offset;

    psi_uncorrected = range_node->elev_fit->a + 2 * PI * floor((phase_diff_max-range_node->elev_fit->a)/(2*PI));

    if(phi_sign < 0) psi_uncorrected += 2 * PI;

    psi = psi_uncorrected - cable_offset;


    psi_kd = psi/(wave_num * antenna_sep);
    theta = c_phi_0 * c_phi_0 - psi_kd * psi_kd;
    /*Elevation errors*/
    psi_k2d2 = psi/(wave_num * wave_num * antenna_sep * antenna_sep);
    df_by_dy = psi_k2d2/sqrt(theta * (1 - theta));

   fit_data->elv[range_node->range].error = 180/PI * sqrt(range_node->elev_fit->sigma_2_a) * fabs(df_by_dy);

}

/**
 * @brief      Sets the phase offset for the XCF from raw data.
 *
 * @param[in]  range            A RANGENODE struct.
 * @param      fit_range_array  This struct holds fit results and is used by RST to write out final
 *                              final results.
 * @param      fit_prms         The FITPRM struct holding rawacf record info.
 *
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
 */
void set_xcf_sdev_phi(llist_node range, struct FitRange* fit_range_array){
    RANGENODE* range_node;

    range_node = (RANGENODE*) range;

    fit_range_array[range_node->range].sdev_phi = range_node->elev_fit->chi_2;

}
