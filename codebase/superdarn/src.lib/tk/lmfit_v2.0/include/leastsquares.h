/*Non-Linear Least squares fitting using Levenburg-Marquardt 
Algorithm implements in C (cmpfit)

Adapted by: Ashton Reimer
From code by: Keith Kotyk

ISAS
August 2016
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
