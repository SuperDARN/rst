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

#ifndef _ACFFIT_H
#define _ACFFIT_H

#include "llist.h"
#include "fit_structures.h"

typedef enum{
	ACF,
	XCF
}PHASETYPE;

void Power_Fits(llist_node range);
void ACF_Phase_Fit(llist ranges,FITPRMS *fit_prms);
void XCF_Phase_Fit(llist ranges,FITPRMS *fit_prms);

void calculate_log_pwr_sigma(llist_node pwr);
void phase_fit_for_range(llist_node range,PHASETYPE *phasetype);
void calculate_phase_sigma_for_range(llist_node range, FITPRMS *fit_prms,PHASETYPE *phasetype);
void calculate_phase_sigma(llist_node phase, llist_node range, FITPRMS *fit_prms);


#endif
