/*Copyright (C) 2016  SuperDARN Canada

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
ACF least square fitting wrapper functions

Keith Kotyk
ISAS
July 2015

*/

#include "rtypes.h"
#include <math.h>
#include "fitting.h"
#include "preprocessing.h"
#include "leastsquares.h"
#include <stdio.h>

/**
 * @brief      Perform fit for ACF power.
 *
 * @param[in]  range    A range node(RANGENODE struct) stored in the list.
 *
 * The function is meant to be mapped to every range node via the list method llist_for_each.
 * The data for logarithm of ACF power are fitted using linear least squares for a two parameter straight line
 * fit (exponential decay) and a quadratic fit (Gaussian decay). 
 * Formally, weighting coefficients in the least square fit (variance) should have the 
 * same units as the fitted data (in our case it is log power) otherwise the fitting errors will be determined incorrectly.
 * However, fitting simulated ACFs showed that more accurate parameter estimates are obtained if the weights expressed 
 * in linear power units (not log power!). Therfore, we run the fitting pocedure twice: 
 * (1) with linear power weights to get lag 0 power and spectral width
 * and
 * (2) with log power weights to get correct error estimates.
 * 
 */
void Power_Fits(llist_node range){
    RANGENODE* range_node;

    range_node = (RANGENODE*) range;

    /*Here we fit for parameters*/
    two_param_straight_line_fit(range_node->l_pwr_fit,range_node->pwrs,1, 1);

    quadratic_fit(range_node->q_pwr_fit,range_node->pwrs,1, 1);

    /*Here we fit for errors using log corrected sigma*/

    llist_for_each(range_node->pwrs,calculate_log_pwr_sigma);

    two_param_straight_line_fit(range_node->l_pwr_fit_err,range_node->pwrs,1, 1);

    quadratic_fit(range_node->q_pwr_fit_err,range_node->pwrs,1, 1);



}


/**
 * @brief      Perform fit for ACF phase.
 *
 * @param[in]  ranges      The list of range nodes(RANGENODE structs).
 * @param      fit_prms    A pointer to a fitting parameters struct.
 *
 * For each range, all ACF phase lags have their sigma value calculated from fitted ACF power
 * and then the phase is unwrapped. The one parameter linear least squares fit is performed
 * to unwrapped phase.
 */
void ACF_Phase_Fit(llist ranges,FITPRMS *fit_prms){
    PHASETYPE acf = ACF;

    llist_for_each_arg(ranges,(node_func_arg)calculate_phase_sigma_for_range,fit_prms,&acf);

    llist_for_each_arg(ranges,(node_func_arg)ACF_Phase_Unwrap, fit_prms, NULL);

    llist_for_each_arg(ranges,(node_func_arg)phase_fit_for_range,&acf,NULL);


}

/**
 * @brief      Perform fit for XCF phase(elevation).
 *
 * @param[in]  ranges      The list of range nodes(RANGENODE structs).
 * @param      fit_prms    A pointer to a fitting parameters struct.
 *
 * For each range, all XCF phase lags have their sigma values calculated from fitted ACF power
 * and then the phase is unwrapped. 
 * The two parameter linear fit is performed
 * to unwrapped phase.
 * A correct procedure requires knowledge of the cross-correlation coefficient
 * |Rx(tau)|=XCF(tau)/sqrt(ACF(0)*ACF_I(0))
 * where ACF_I(0) if the lag 0 power of the signal received by the interfrometer antenna alone.
 * Currently this parameter is not stored in the raw structure so we have to use the normalised 
 * ACF power of the signal received by the main antenna, |R(tau)|, as a substitute for |Rx(tau)|.
 * It is necessary to mention that this substitution affects the elevation errors only 
 * but has no effect on the elevation itself because the latter is calulated directly from 
 * lag 0 XCF phase, i.e. bypassing any fitting.
 */
void XCF_Phase_Fit(llist ranges,FITPRMS *fit_prms){
    PHASETYPE xcf = XCF;
    llist_for_each_arg(ranges,(node_func_arg)calculate_phase_sigma_for_range,fit_prms,&xcf);

    llist_for_each(ranges,(node_func)XCF_Phase_Unwrap);

    llist_for_each_arg(ranges,(node_func_arg)phase_fit_for_range,&xcf,NULL);


}

/**
 * @brief      Selects which phase fit to use depending on type(ACF or XCF).
 *
 * @param[in]  range      A range node(RANGENODE struct) stored in the list.
 * @param      phasetype  An enum to select the type of fit to use.
 *
 * The function is meant to be mapped to every range node via the list method llist_for_each. Based
 * off the phase type, the one paramter or two parameter fit is done.
 */
void phase_fit_for_range(llist_node range,PHASETYPE *phasetype){
    RANGENODE* range_node;

    range_node = (RANGENODE*) range;

    switch(*phasetype){
        case ACF:
            one_param_straight_line_fit(range_node->phase_fit,range_node->phases,1, 1);
            break;
        case XCF:
            two_param_straight_line_fit(range_node->elev_fit,range_node->elev,1,1);
            break;
    }
}

/**
 * @brief      Calculates the phase sigmas for a range node.
 *
 * @param[in]  range      A range node(RANGENODE struct) stored in the list.
 * @param      fit_prms   A pointer to a fitting parameters struct.
 * @param      phasetype  An enum to switch between ACF or XCF data.
 *
 * The function is meant to be mapped to every range node via the list method llist_for_each. The
 * sigma values for all phase values are calculated. The fitted values of power are used to
 * calculate phase sigma, so the fitting for power must be done first.
 */
void calculate_phase_sigma_for_range(llist_node range,FITPRMS *fit_prms,PHASETYPE *phasetype){
    RANGENODE* range_node;
    PHASENODE* xcf0 = NULL,*xcf1 = NULL;
    range_node = (RANGENODE*) range;

    switch(*phasetype){
        case ACF:
            llist_for_each_arg(range_node->phases,(node_func_arg)calculate_phase_sigma,range_node,
                (void*)fit_prms);
            break;
        case XCF:
            llist_for_each_arg(range_node->elev,(node_func_arg)calculate_phase_sigma,range_node,
                (void*)fit_prms);
            llist_reset_iter(range_node->elev);

            /*Since lag 0 phase is included in the elevation fit but for ACF its variance is 0, 
            we have to set lag 0 phase sigma to its closest neighbour's value, i.e. the same as lag 1 sigma.*/
            
            llist_get_iter(range_node->elev,(void**)&xcf0);
            llist_go_next(range_node->elev);
            llist_get_iter(range_node->elev,(void**)&xcf1);

            xcf0->sigma = xcf1->sigma;

            break;
    }



}

/**
 * @brief      Calculates the phase sigma for a phase node.
 *
 * @param[in]  phase     A phase node(PHASENODE struct) stored in a range.
 * @param[in]  range     The range node(RANGENODE struct) associated with this phase value. (TODO)
 * @param      fit_prms  A pointer to a fitting parameters struct.
 *
 * The sigma value for a phase node is calculated using fitted power.
 */
void calculate_phase_sigma(llist_node phase, llist_node range, FITPRMS *fit_prms){
    PHASENODE* phase_node;
    RANGENODE* range_node;
    double inverse_alpha_2,pwr,inverse_pwr_2;

    phase_node = (PHASENODE*) phase;
    range_node = (RANGENODE*) range;

    inverse_alpha_2 = 1/phase_node->alpha_2;
    pwr = exp(-1 * fabs(range_node->l_pwr_fit->b) * phase_node->t);
    inverse_pwr_2 = 1/(pwr * pwr);
    phase_node->sigma = sqrt((inverse_alpha_2 * inverse_pwr_2 - 1)/(2 * fit_prms->nave));
    if(isnan(phase_node->sigma)){
      fprintf(stderr,"range: %d, inverse_alpha: %f, pwr slope: %f, pwr: %f, inverse pwr: %f\n",
        range_node->range,inverse_alpha_2, range_node->l_pwr_fit->b,pwr,inverse_pwr_2);
    }

}

/**
 * @brief      Calculates the log power sigmas for a range node.
 *
 * @param[in]  range  The range node(RANGENODE struct) associated with this phase value.
 *
 * The function is meant to be mapped to every range node via the list method llist_for_each.
 * Each range will have the sigma values calculated for the log power values.
 *
 */
void calculate_log_pwr_sigma_for_range(llist_node range){
    RANGENODE* range_node;

    range_node = (RANGENODE*) range;

    llist_for_each(range_node->pwrs,calculate_log_pwr_sigma);
}

/**
 * @brief      Calculates the log power sigma.
 *
 * @param[in]  pwr   A phase node(PHASENODE struct) stored in a range.
 *
 * Calcutes the log power sigma for a low power node.
 */
void calculate_log_pwr_sigma(llist_node pwr){
    PWRNODE* pwr_node;

    pwr_node = (PWRNODE*) pwr;

    pwr_node->sigma = pwr_node->sigma / exp(pwr_node->ln_pwr);


}
