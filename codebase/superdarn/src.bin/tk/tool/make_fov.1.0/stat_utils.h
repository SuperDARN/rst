/* utils.h
   =============
   Author: Angeline G. Burrell - NRL - 2021
*/

/*
 LICENSE AND DISCLAIMER

 This file is part of the Radar Software Toolkit (RST).

 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.

 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef INFINITY
#define INFINITY 9.99E7
#endif

struct gauss_data
{
  double *x;
  double *y;
  double *y_error;
};

int num_unique_int_vals(int num, int array[]);
float stdev_float(int num, float array[]);
void mean_stdev_float(int num, float array[], float *mean, float *stdev);
void zscore(int num, float array[], float *zscore);
int get_unique_str(int in_num, char **in_str, char **out_str);
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
