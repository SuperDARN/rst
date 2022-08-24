/*
 Least squares fitting header

 Copyright (c) 2016 University of Saskatchewan
 Author: Keith Kotyk


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


*/


#ifndef _LEASTSQUARES_H
#define _LEASTSQUARES_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "llist.h"
#define determ(aa,bb,cc,dd) (aa*dd - bb*cc)


#define BAD 1
#define GOOD 0

typedef struct sums{
	double S;
	double S_x;
	double S_y;
	double S_xx;
	double S_xy;
}SUMS;

typedef enum fit_type{
	LINEAR,
	QUADRATIC
}FIT_TYPE;

typedef struct fitting_data{
	SUMS *sums;
	double delta;
	double a;
	double b;
	double sigma_2_a;
	double sigma_2_b;
	double delta_a;
	double delta_b;
	double cov_ab;
	double r_ab;
	double Q;
	double chi_2;

}FITDATA;

FITDATA* new_fit_data();

void free_fit_data(FITDATA *fit_data);

void one_param_straight_line_fit(FITDATA *fit_data,llist data,int confidence, int DoF);
void quadratic_fit(FITDATA *fit_data,llist data,int confidence, int DoF);
void two_param_straight_line_fit(FITDATA *fit_data,llist data,int confidence, int DoF);

void find_chi_2(llist_node data,FITDATA *fit_data,FIT_TYPE* fit_type);
void calculate_sums(llist_node data,FITDATA *fit_data,FIT_TYPE* fit_type);

void print_fit_data(FITDATA *fit_data,FILE* fp);

#endif
