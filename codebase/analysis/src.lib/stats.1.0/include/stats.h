/* stats.h
   =======
   Authors: Keith Kotyk - ISAS - 2015 and Angeline G. Burrell - NRL - 2021
   Copyright (C) 2016  SuperDARN Canada (gamma functions)
   This is a U.S. government work and not under copyright protection in the U.S.

   This file is part of the Radar Software Toolkit (RST).

   Disclaimer: RST is licensed under GPL v3.0. Please visit 
               <https://www.gnu.org/licenses/> to see the full license

   Modifications:

*/

#ifndef _STATS_H
#define _STATS_H

#ifndef _MPFIT_H
#import "mpfit.h"
#endif

#ifndef _SORT_H
#import "sort.h"
#endif

#ifndef INFINITY
#define INFINITY 9.99E7
#endif

#ifndef ITMAX
#define ITMAX 100
#endif

#ifndef EPS
#define EPS 3.0e-7
#endif

#ifndef FPMIN
#define FPMIN 1.0e-30
#endif

struct gauss_data
{
  double *x;
  double *y;
  double *y_error;
};

int num_unique_int_vals(int num, int array[]);
int get_unique_str(int in_num, char **in_str, char **out_str);
float stdev_float(int num, float array[]);
void mean_stdev_float(int num, float array[], float *mean, float *stdev);
void zscore(int num, float array[], float *zscore);
double gaussian(double x, double *params);
double mult_gaussian(double x, double *params);
int gaussian_dev(int m, int n, double *p, double *deviates, double **derivs,
		 void *private);
int mult_gaussian_dev(int m, int n, double *p, double *deviates,
		      double **derivs, void *private);
void histogram(int nvals, float vals[], int nbin, float val_min, float val_max,
	       int *hist_bins, float *leading_bin_val);
int int_argrelmax(int num, int vals[], int order, int clip, int *ismax);
int int_argabsmax(int num, int vals[]);
float float_absmax(int num, float vals[]);
float float_absmin(int num, float vals[]);
double gammaq(double a, double x);
void gamma_series_rep(double *gamser, double a, double x, double *gln);
void gamma_continued_frac(double *gammcf, double a, double x, double *gln);
double gammln(double xx);

#endif
