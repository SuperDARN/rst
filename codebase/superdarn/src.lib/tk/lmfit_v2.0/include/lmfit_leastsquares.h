/*Non-Linear Least squares fitting using Levenburg-Marquardt 
Algorithm implements in C (cmpfit)

/TODO Add copyright notice

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

#ifndef _LMFITLEASTSQUARES_H
#define _LMFITLEASTSQUARES_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "llist.h"
#define determ(aa,bb,cc,dd) (aa*dd - bb*cc)


#define BAD 1
#define GOOD 0

typedef enum fit_type{
	LINEAR,
	QUADRATIC
}FIT_TYPE;

struct datapoints
{
  double *x;
  double *y;
  double *ey;
  double lambda;
};

typedef struct fitting_data{
	double P;
	double wid;
	double vel;
	double phi0;
	double sigma_2_P;
	double sigma_2_wid;
	double sigma_2_vel;
	double sigma_2_phi0;
	double chi_2;
}LMFITDATA;

LMFITDATA* new_lmfit_data();

void free_lmfit_data(LMFITDATA *fit_data);
void print_lmfit_data(LMFITDATA *fit_data,FILE* fp);
int exp_acf_model(int m, int n, double *params, double *deviates, double **derivs, void *private);
void lmfit_acf(LMFITDATA *fit_data,llist data, double lambda, int mpinc, int confidence, int model);

#endif
