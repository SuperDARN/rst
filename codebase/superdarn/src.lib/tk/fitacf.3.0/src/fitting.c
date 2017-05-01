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

#include <math.h>
#include "fitting.h"
#include "preprocessing.h"
#include "leastsquares.h"
#include <stdio.h>

/**
For a given range, both the two parameter linear and quadratic fits are
performed for power
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
For a given range, each phase lag has sigma calculated from fitted power
and then the phase is unwrapped. The one parameter linear fit is performed
to unwrapped phase.
*/
void ACF_Phase_Fit(llist ranges,FITPRMS *fit_prms){
	PHASETYPE acf = ACF;

	llist_for_each_arg(ranges,(node_func_arg)calculate_phase_sigma_for_range,fit_prms,&acf);

	llist_for_each_arg(ranges,(node_func_arg)ACF_Phase_Unwrap, fit_prms, NULL);

	llist_for_each_arg(ranges,(node_func_arg)phase_fit_for_range,&acf,NULL);


}

/**
For a given range, each XCF phase lag has sigma calculated from fitted ACF power
and then the phase is unwrapped. The two parameter linear fit is performed
to unwrapped phase.
*/
void XCF_Phase_Fit(llist ranges,FITPRMS *fit_prms){
	PHASETYPE xcf = XCF;
	llist_for_each_arg(ranges,(node_func_arg)calculate_phase_sigma_for_range,fit_prms,&xcf);

	llist_for_each(ranges,(node_func)XCF_Phase_Unwrap);

	llist_for_each_arg(ranges,(node_func_arg)phase_fit_for_range,&xcf,NULL);


}

/**
Helper function to do the ACF phase or XCF elevation phase fit for an individual range
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
Helper function to calculate sigmas for a list of phase lags at a given range for
either XCF or ACF. For now XCF uses the same routine as ACF.
*/
void calculate_phase_sigma_for_range(llist_node range,FITPRMS *fit_prms,PHASETYPE *phasetype){
	RANGENODE* range_node;
	PHASENODE* xcf0 = NULL,*xcf1 = NULL;
	range_node = (RANGENODE*) range;

	switch(*phasetype){
		case ACF:
			llist_for_each_arg(range_node->phases,(node_func_arg)calculate_phase_sigma,range_node,(void*)fit_prms);
			break;
		case XCF:
			llist_for_each_arg(range_node->elev,(node_func_arg)calculate_phase_sigma,range_node,(void*)fit_prms);
			llist_reset_iter(range_node->elev);

			/*Since lag 0 phase is included for elevation fit, we set lag 0 sigma the
			same as lag 1 sigma*/
			llist_get_iter(range_node->elev,(void**)&xcf0);
			llist_go_next(range_node->elev);
			llist_get_iter(range_node->elev,(void**)&xcf1);

			xcf0->sigma = xcf1->sigma;

			break;
	}



}


/**
Helper function to calculate sigma at individual lags
*/
void calculate_phase_sigma(llist_node phase, llist_node range, FITPRMS *fit_prms){
	PHASENODE* phase_node;
	RANGENODE* range_node;
	double inverse_alpha_2,pwr,inverse_pwr_2;

	phase_node = (PHASENODE*) phase;
	range_node = (RANGENODE*) range;

	/*at this point sigma is holding its value of alpha to make
	  list traversal easier at this point. This value gets set in
	  new_phase_node()*/
	inverse_alpha_2 = 1/phase_node->sigma;
	pwr = exp(-1 * fabs(range_node->l_pwr_fit->b) * phase_node->t);

	inverse_pwr_2 = 1/(pwr * pwr);
	phase_node->sigma = sqrt((inverse_alpha_2 * inverse_pwr_2 - 1)/(2 * fit_prms->nave));
	if(isnan(phase_node->sigma)){
	  fprintf(stderr,"range: %d, inverse_alpha: %f, pwr slope: %f, pwr: %f, inverse pwr: %f\n",range_node->range,inverse_alpha_2, range_node->l_pwr_fit->b,pwr,inverse_pwr_2);
	}
	/*Sigma values larger than PI make no physical sense so anything larger is set to PI*/
	if (phase_node->sigma > M_PI) phase_node->sigma = M_PI;

}

void calculate_log_pwr_sigma_for_range(llist_node range){
	RANGENODE* range_node;

	range_node = (RANGENODE*) range;

	llist_for_each(range_node->pwrs,calculate_log_pwr_sigma);
}

void calculate_log_pwr_sigma(llist_node pwr){
	PWRNODE* pwr_node;

	pwr_node = (PWRNODE*) pwr;

	pwr_node->sigma = pwr_node->sigma / exp(pwr_node->ln_pwr);


}
