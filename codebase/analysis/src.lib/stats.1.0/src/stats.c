/* stats.c
   =======
   Author: Angeline G. Burrell - NRL - 2021
   This is a U.S. government work and not under copyright protection in the U.S.

   This file is part of the Radar Software Toolkit (RST).

   Disclaimer: RST is licensed under GPL v3.0. Please visit 
               <https://www.gnu.org/licenses/> to see the full license

   Modifications:

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "mpfit.h"
#include "sort.h"
#include "stats.h"

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
  sorted_array = (int *)calloc(num, sizeof(int));
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
	sum += pow(array[i] - *mean, 2);

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
 * @brief A Multi-Gaussian function for MINPACK optimization
 *
 * @params[in] x      - Independent variable value
 *             params - Pointer holding the number of gaussian peaks, followed
 *                      by the amplitude, mean, and standard deviation for
 *                      each peak.
 *
 * @params[out] y - Result of Gaussian calculation
 **/

double mult_gaussian(double x, double *params)
{
  int i, imax;

  double y, nparams[3];

  imax = (int)params[0];

  for(y = 0.0, i = 0; i < imax; i++)
    {
      nparams[0] = params[1 + i * 3];
      nparams[1] = params[2 + i * 3];
      nparams[2] = params[3 + i * 3];
      y += gaussian(x, nparams);
    }

  return y;
}

/**
 * @brief Calculates the deviation of a multi-gaussian function for minimization
 *
 * @params[in] m - Number of samples in `private` data structure
 *             n - Number of samples in `p` parameter pointer
 *             p - Parameter pointer (see mult_gaussian)
 *             private - Structure holding the input data
 *
 * @params[out] deviates - deviation between the model and data
 *              derivs   - optional derivative values, not used 
 **/

int mult_gaussian_dev(int m, int n, double *p, double *deviates,
		      double **derivs, void *private)
{
  int i;
  double *x, *y, *y_err;

  struct gauss_data *gdat = (struct gauss_data *)private;

  x     = gdat->x;
  y     = gdat->y;
  y_err = gdat->y_error;

  for(i = 0; i < m; i++)
    deviates[i] = (y[i] - mult_gaussian(x[i], p)) / y_err[i];

  return(0);
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

  struct gauss_data *gdat = (struct gauss_data *)private;

  x     = gdat->x;
  y     = gdat->y;
  y_err = gdat->y_error;

  for(i = 0; i < m; i++)
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
      leading_bin_val[i] = val_min + (float)i * val_inc;
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
 * @brief Routine to find the relative maxima based on numpy _boolrelextrema
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
  int i, shift, iplus, iminus, nmax, val_plus, val_minus;

  /* Test the order input */
  if(order < 1) order = 1;

  /* Initialize the output and working variables*/
  for(i = 0; i < num; i++) ismax[i] = 0;
  nmax = 0;

  /* Cycle through each order of consideration */
  for(shift = order; shift >= 1; shift--)
    {
      for(i = 0; i < num; i++)
	{
	  iminus = i - shift;
	  iplus  = i + shift;

	  /* If the plus/minus indices extend beyond the array limits, */
	  /* either clip them (keep them at the edge) or wrap them.    */
	  if(iminus < 0)
	    {
	      if(clip == 1) val_minus = vals[i] - 1;
	      else          val_minus = vals[iminus + num];
	    }
	  else val_minus = vals[iminus];

	  if(iplus >= num)
	    {
	      if(clip == 1) val_plus = vals[i] - 1;
	      else          val_plus = vals[iplus - num];
	    }
	  else val_plus = vals[iplus];

	  /* Evaluate the values, looking for a relative maximum */
	  if(vals[i] > val_plus && vals[i] > val_minus)
	    {
	      if(ismax[i] == 0)
		{
		  nmax++;
		  ismax[i] = 1;
		}
	    }
	  else if(ismax[i] == 1)
	    {
	      ismax[i] = 0;
	      nmax--;
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

  for(max_val = vals[0], i = 1; i < num; i++)
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

/**
 * @brief Computes the incomplete upper gamma function.
 *
 * Computes the incomplete upper gamma function using either series
 * representation or continued fractions, depending on which will be faster.
 * Directly taken from Numerical Recipies in C.  Refer to text for more
 * description.
 * 
 * @references NUMERICAL RECIPES IN C: THE ART OF SCIENTIFIC COMPUTING.
 **/

double gammaq(double a, double x)
{
  double gamser = 0.0, gammcf = 0.0, gln = 0.0;
 
  if (x < 0.0 || a <= 0.0) return -1.0;

  if (x < (a + 1.0))
    {
      gamma_series_rep(&gamser, a, x, &gln);
      return 1.0 - gamser;
    }
  else
    {
      gamma_continued_frac(&gammcf, a, x, &gln);
      return gammcf;
    }
}

/**
 * @brief Computes the gamma function as a series representation
 *
 * Computes the gamma function as a series representation. Directly taken from
 * Numerical Recipies in C.  Refer to text for more description.
 * 
 * @references NUMERICAL RECIPES IN C: THE ART OF SCIENTIFIC COMPUTING.
 **/

void gamma_series_rep(double *gamser, double a, double x, double *gln)
{
  int n;
  double sum, del, ap;

  *gln = gammln(a);

  if (x <= 0.0)
    {
      if (x < 0.0) *gamser = 0.0;
      return;
    }
  else
    {
      ap = a;
      del = sum = 1.0 / a;
      for (n = 1;n <= ITMAX; n++)
	{
	  ++ap;
	  del *= x / ap;
	  sum += del;
	  if (fabs(del) < fabs(sum) * EPS)
	    {
	      *gamser = sum * exp(-x + a * log(x) - (*gln));
	      return;
	    }
	}
      return;
    }
}

/**
 * @brief Computes the gamma function as continued fractions
 *
 * Computes the gamma function as continued fractions. Directly taken from
 * Numerical Recipies in C.  Refer to text for more description.
 * 
 * @references NUMERICAL RECIPES IN C: THE ART OF SCIENTIFIC COMPUTING.
 **/

void gamma_continued_frac(double *gammcf, double a, double x, double *gln)
{
  int i;
  double an,b,c,d,del,h;

  *gln = gammln(a);
  b = x + 1.0 - a;
  c = 1.0 / FPMIN;
  d = 1.0 / b;
  h = d;

  for (i = 1; i <= ITMAX; i++)
    {
      an = -i * (i - a);
      b += 2.0;
      d = an * d + b;
      if (fabs(d) < FPMIN) d = FPMIN;
      c = b + an / c;
      if (fabs(c) < FPMIN) c = FPMIN;
      d = 1.0 / d;
      del = d * c;
      h *= del;
      if (fabs(del-1.0) < EPS) break;
    }
  if (i > ITMAX)
    *gammcf = exp(-x + a * log(x) - (*gln)) * h;
}

/**
 * @brief Computes log of gamma function.
 *
 * Computes the log of a gamma function to prevent overflows. Directely taken
 * from Numerical Recipies in C.  Refer to text for more description.
 * 
 * @references NUMERICAL RECIPES IN C: THE ART OF SCIENTIFIC COMPUTING.
 **/

double gammln(double xx)
{
  double x, y, tmp, ser;

  static double cof[6] = {76.18009172947146, -86.50532032941677,
			  24.01409824083091, -1.231739572450155,
			  0.1208650973866179e-2, -0.5395239384953e-5};
  int j;

  y = x = xx;
  tmp = x + 5.5;
  tmp -= (x + 0.5) * log(tmp);
  ser = 1.000000000190015;

  for (j = 0;j <= 5; j++) ser += cof[j] / ++y;
  return -tmp + log(2.5066282746310005 * ser / x);
}
