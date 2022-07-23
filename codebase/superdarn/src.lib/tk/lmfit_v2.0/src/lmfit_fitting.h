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

#ifndef _ACFFIT_H
#define _ACFFIT_H

#include "llist.h"
#include "lmfit_structures.h"

typedef enum{
	ACF,
	XCF
}PHASETYPE;

void ACF_Fit(llist ranges,FITPRMS *fitted_prms);
void XCF_Fit(llist ranges,FITPRMS *fitted_prms);
void do_LMFIT(llist_node range,PHASETYPE *phasetype,FITPRMS *fitted_prms);

#endif 
