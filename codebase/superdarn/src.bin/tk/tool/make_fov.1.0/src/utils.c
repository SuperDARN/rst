/* utils.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mpfit.h"

#include "utils.h"
#include "sort_rout.h"

/**
 * @brief Calculate the number of unique values for an integer array
 *
 * @params[in] num   - Number of elements in `array`
 *             array - Data array with `num` elements
 *
 * @params[out] num_unique  - Number of unique elements in `array`
 **/

int num_unique_int_vals(int num, int array[])
{
  int i, num_unique, *sorted_array;

  /* Initialize the sorted array and load it with the input data to */
  /* prevent the input array from being re-arranged.                */
  sorted_array = (int *)calloc(sizeof(int) * num);
  for(i = 0; i < num; i++) sorted_array[i] = array[i];

  /* Sort the array */
  smart_sort_int(num, sorted_array);

  /* Count the number of unique elements */
  for(num_unique = 1, i = 1; i < num; i++)
    {
      if(sorted_array[i - 1] < sorted_array[i])
	num_unique++;
    }

  free(sorted_array);
  return(num_unique);
}

/**
 * @brief Calculate the standard deviation
 *
 * @params[in] num   - Number of elements in `array`
 *             array - Data array with `num` elements
 *
 * @params[out] stdev - Standard deviation of the input data, or zero if
 *                      there isn't enough data for a calculation
 **/

float stdev_float(int num, float array[])
{
  int i;

  float sum, mean, stdev;

  if(num <= 1)
    stdev = 0.0;
  else
    {
      for(sum = 0.0, i = 0; i < num; i++)
	sum += array[i];

      mean = sum / (float)num;

      for(sum = 0.0, i = 0; i < num; i++)
	sum += pow(array[i] - mean, 2);

      stdev = sqrt(sum / (float)num);
    }

  return(stdev);
}

/**
 * @brief Calculate the mean and standard deviation
 *
 * @params[in] num   - Number of elements in `array`
 *             array - Data array with `num` elements
 *
 * @params[out] mean  - Arithmatic mean of the input data, or zero if no data
 *              stdev - Standard deviation of the input data, or zero if
 *                      there isn't enough data for a calculation
 **/

void mean_stdev_float(int num, float array[], float *mean, float *stdev)
{
  int i;

  float sum;

  /* Ensure there is enough data to calculate a value */
  if(num <= 1)
    {
      /* A single value has no deviation, for no values both are zero */
      *mean  = (num == 1) ? array[0] : 0.0;
      *stdev = 0.0;
    }
  else
    {
      /* Calculate the mean */
      for(sum = 0.0, i = 0; i < num; i++)
	sum += array[i];

      *mean = sum / (float)num;

      for(sum = 0.0, i = 0; i < num; i++)
	sum += pow(array[i] - mean, 2);

      *stdev = sqrt(sum / (float)num);
    }

  return;
}

/**
 * @brief Calculate the z-score
 *
 * @params[in] num   - Number of elements in `array`
 *             array - Data array with `num` elements
 *
 * @params[out] zscore - z-score, a single float value
 **/

void zscore(int num, float array[], float *zscore)
{
  int i;

  float mean, stdev;

  void mean_stdev_float(int num, float array[], float *mean, float *stdev);

  /* Get the mean and standard deviation */
  mean_stdev_float(num, array, &mean, &stdev);

  /* Calculate the z-score */
  for(i = 0; i < num; i++)
    {
      if(stdev == 0.0) zscore[i] = INFINITY;
      else             zscore[i] = (array[i] - mean) / stdev;
    }

  return;
}
  

/**
 * @brief Take an array of strings and return an array of the unique strings
 *
 * @param[in] int_num    Number of strings contained within in_str
 *            in_str     Array of strings to evaluate
 *
 * @param[out] out_num  Number of unique strings found within in_str
 *             out_str  Array of unique strings
 **/

int get_unique_str(int in_num, char **in_str, char **out_str)
{
  int i, j, out_num, match;

  out_num = 1;
  strcpy(out_str[0], in_str[0]);

  for(i = 1; i < in_num; i++)
    {
      /* Look for a matching string */
      for(match = 0, j = 0; j < out_num && match == 0; j++)
	{
	  if(strcmp(out_str[j], in_str[i]) == 0) match = 1;
	}

      /* If there is no matching string, add this to the output */
      if(match == 0)
	{
	  strcpy(out_str[out_num], in_str[i]);
	  out_num++;
	}
    }

  return(out_num);
}

/**
 * @brief A Gaussian function for MINPACK optimization
 *
 * @params[in] x      - Independent variable value
 *             params -  Pointer holding amplitude, mean, and standard deviation
 *
 * @params[out] y - Result of Gaussian calculation
 **/

double gaussian(double x, double *params)
{
  double y;

  y = params[0] * exp(-pow((x - params[1]), 2.0)
		      / (2.0 * params[2] * params[2]));
  return(y);
}

/**
 * @brief Calculates the deviation of a gaussian function for minimization
 *
 * @params[in] m - Number of samples in `private` data structure
 *             n - Number of samples in `p` parameter pointer
 *             p - Parameter pointer (amplitude, mean, standard deviation)
 *             private - Structure holding the input data
 *
 * @params[out] deviates - deviation between the model and data
 *              derivs   - optional derivative values, not used 
 **/

int gaussian_dev(int m, int n, double *p, double *deviates, double **derivs,
		 void *private)
{
  int i;
  double *x, *y, *y_err;

  double gaussian(double x, double *params);

  struct gauss_data *gdat = (struct gauss_data *)private;

  x     = gdat->x;
  y     = gdat->y;
  y_err = gdat->y_error;

  for(i=0; i<m; i++)
    deviates[i] = (y[i] - gaussian(x[i], p)) / y_err[i];

  return(0);
}

/**
 * @brief A histogram function
 *
 * @params[in] nvals   - Number of values in `vals` array
 *             vals    - Array of values
 *             nbin    - Number of bins to use in histogram
 *             val_min - Minimum value to consider
 *             val_max - Maximum value to consider
 *
 * @params[out] hist_bins       - Array of counts for values in this bin
 *              leading_bin_val - Array of values corresponding to the leading
 *                                edge of the histogram bins
 **/

void histogram(int nvals, float vals[], int nbin, float val_min, float val_max,
	       int *hist_bins, float *leading_bin_val)
{
  int i, ival;

  float val_inc, trailing_bin_val;

  /* Calculate the bin increment */
  val_inc = (val_max - val_min) / (float)nbin;

  /* Initalize the output */
  for(i = 0; i < nbin; i++)
    {
      leading_bin_val[i] = min_val + (float)i * val_inc;
      hist_bins[i]       = 0;
    }

  /* Cycle through each bin, identifying the number of points in each */
  for(ival = 0; ival < nvals; ival++)
    {
      for(i = 0; i < nbin; i++)
	{
	  trailing_bin_val = leading_bin_val[i] + val_inc;
	  if(leading_bin_val[i] <= vals[ival] && vals[ival] < trailing_bin_val)
	    {
	      hist_bins[i]++;
	      break;
	    }
	}
    }

  return;
}

/**
 * @brief Routine to find the relative maximum based on numpy _boolrelextrema
 *
 * @params[in] num   - Number of samples in `vals`
 *             vals  - Integer array of input values to be searched
 *             order - How many points to each side of a value to consider for
 *                     a comparison to be true
 *             clip  - 1 to treat the edge of the `vals` array as a hard edge,
 *                     any other value to wrap the `vals` array
 *
 * @params[out] ismax - Array of booleans specifying which vals indices are
 *                      a relative maxima
 *              nmax  - Number of relative maxima identified
 **/

int int_argrelmax(int num, int vals[], int order, int clip, int *ismax)
{
  int i, shift, iplus, iminus, nmax;

  /* Test the order input */
  if(order < 1) order = 1;

  /* Initialize the output and working variables*/
  for(i = 0; i < num; i++) ismax[i] = 0;
  nmax = 0;

  /* Cycle through each order of consideration */
  for(shift = 1; shift <= order; shift++)
    {
      for(i = 0; i < num; i++)
	{
	  iminus = i - shift;
	  iplus  = i + shift;

	  /* If the plus/minus indices extend beyond the array limits, */
	  /* either clip them (keep them at the edge) or wrap them.    */
	  if(iminus < 0)
	    {
	      if(clip == 1) iminus  = 0;
	      else          iminus += num;
	    }
	  if(iplus >= num)
	    {
	      if(clip == 1) iplus  = num - 1;
	      else          iplus -= num;
	    }

	  /* Evaluate the values, looking for a relative maximum */
	  if(vals[i] > vals[iplus] && vals[i] > vals[iminus])
	    {
	      nmax++;
	      ismax[i] = 1;
	    }
	}

      /* If all potential relative maxima have been eliminated, exit */
      if(nmax == 0) return(nmax);
    }

  return(nmax);
}

/**
 * @brief Routine to find the absolute maximum for int intput
 *
 * @params[in] num  - Number of samples in `vals`
 *             vals - Integer array of input values to be searched
 *
 * @params[out] imax - Index of absolute maximum
 **/

int int_argabsmax(int num, int vals[])
{
  int i, imax;

  for(imax = 0, i = 1; i < num; i++)
    {
      if(vals[imax] < vals[i]) imax = i;
    }

  return(imax);
}


/**
 * @brief Routine to find the absolute maximum for float input
 *
 * @params[in] num  - Number of samples in `vals`
 *             vals - Integer array of input values to be searched
 *
 * @params[out] max_val - Index of absolute maximum
 **/

float float_absmax(int num, float vals[])
{
  int i;

  float max_val;

  max_val = vals[0];
  for(imax = 0, i = 1; i < num; i++)
    {
      if(max_val < vals[i]) max_val = vals[i];
    }

  return(max_val);
}


/**
 * @brief Routine to find the absolute minimum for float input
 *
 * @params[in] num  - Number of samples in `vals`
 *             vals - Integer array of input values to be searched
 *
 * @params[out] min_val - Value absolute maximum
 **/

float float_absmin(int num, float vals[])
{
  int i;

  float min_val;

  min_val = vals[0];
  for(i = 1; i < num; i++)
    {
      if(min_val > vals[i]) min_val = vals[i];
    }

  return(min_val);
}
