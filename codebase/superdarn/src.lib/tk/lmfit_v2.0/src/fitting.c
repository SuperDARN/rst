/*
LMFIT2 least square fitting wrapper functions

Adapted by: Ashton Reimer
From code by: Keith Kotyk

ISAS
August 2016

*/

#include <math.h>
#include "fitting.h"
#include "preprocessing.h"
#include "leastsquares.h"
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