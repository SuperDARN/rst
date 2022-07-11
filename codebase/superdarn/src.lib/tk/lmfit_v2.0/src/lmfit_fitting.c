/*
LMFIT2 least square fitting wrapper functions

//TODO Add copyright notice

Adapted by: Ashton Reimer
From code by: Keith Kotyk

ISAS
August 2016

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

#include <math.h>
#include "lmfit_fitting.h"
#include "lmfit_preprocessing.h"
#include "lmfit_leastsquares.h"
#include <stdio.h>

/* TODO, IMPLEMENT QUADRATIC ENVELOPE AND XCF FITTING */


/**
Helper function that calls the lmfit code with appropriate arguments
depending on whether we are fitting the ACF or the XCF.
*/
void do_LMFIT(llist_node range,PHASETYPE *phasetype,FITPRMS *fitted_prms){
	RANGENODE* range_node;
	range_node = (RANGENODE*) range;
    double lambda, mpinc;

    lambda = 299792458.0/(fitted_prms->tfreq*1000.0);
    mpinc = fitted_prms->mpinc;

	switch(*phasetype){
		case ACF:
		    /* Do exponential envelope fit */
			lmfit_acf(range_node->l_acf_fit,range_node->acf,lambda,mpinc,2,0);

            /* Do gaussian envelope fit */
/*			lmfit_acf(range_node->q_acf_fit,range_node->acf,lambda,mpinc,1,1); */
			break;

		case XCF:
		    /* Do both fits here, take phi0 with smallest error */
		    /* Do exponential envelope fit */
/*			lmfit_xcf(range_node->l_xcf_fit,range_node->xcf,lambda,mpinc,1,0); */

            /* Do gaussian envelope fit */
/*			lmfit_xcf(range_node->q_xcf_fit,range_node->xcf,lambda,mpinc,1,1); */
			break;
	}
}


/**
For a each range, fit the ACF
*/
void ACF_Fit(llist ranges,FITPRMS *fitted_prms){
	PHASETYPE acf = ACF;
	llist_for_each_arg(ranges,(node_func_arg)do_LMFIT,&acf,fitted_prms);
}


/**
For a each range, fit the XCF
*/
void XCF_Fit(llist ranges,FITPRMS *fitted_prms){
	PHASETYPE xcf = XCF;
	llist_for_each_arg(ranges,(node_func_arg)do_LMFIT,&xcf,&fitted_prms);
}
