/*
LMFIT2 least square fitting wrapper functions

Adapted by: Ashton Reimer
From code by: Keith Kotyk

ISAS
August 2016

*/

#ifndef _ACFFIT_H
#define _ACFFIT_H

#include "llist.h"
#include "fit_structures.h"

typedef enum{
	ACF,
	XCF
}PHASETYPE;

void ACF_Fit(llist ranges,FITPRMS *fitted_prms);
void XCF_Fit(llist ranges,FITPRMS *fitted_prms);
void do_LMFIT(llist_node range,PHASETYPE *phasetype,FITPRMS *fitted_prms);

#endif 
