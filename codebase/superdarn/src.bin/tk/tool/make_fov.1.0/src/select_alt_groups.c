/* select_alt_groups.c
   ===============
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
#include <math.h>

#include "mpfit.h"
#include "multbsid.h"

/**
 * @brief A Gaussian function for MINPACK optimization
 *
 * @params[in] x     Independent variable value
 *             A     Amplitude
 *             mu    Mean
 *             sigma Standard deviation
 *
 * @params[out] y    Result of Gaussian calculation
 **/

mp_func gaussian(double x, double A, double mu, double sigma)
{
  double y;

  y = A * exp(-power((x - mu), 2.0) / (2.0 * sigma * sigma));
  return(y);
}


/**
 * @brief A histogram function
 *
 * @params[in] nvals    Number of values in `vals` array
 *             vals     Array of values
 *             nbin     Number of bins to use in histogram
 *             val_min  Minimum value to consider
 *             val_max  Maximum value to consider
 *
 * @params[out] hist_bins        Array of counts for values in this bin
 *              leading_bin_val  Array of values corresponding to the leading
 *                               edge of the histogram bins
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
 * @params[in] num    Number of samples in `vals`
 *             vals   Integer array of input values to be searched
 *             order  How many points to each side of a value to consider for
 *                    a comparison to be true
 *             clip   1 to treat the edge of the `vals` array as a hard edge,
 *                    any other value to wrap the `vals` array
 *
 * @params[out] ismax  Array of booleans specifying which vals indices are
 *                     a relative maxima
 *              nmax   Number of relative maxima identified
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
 * @params[in] num    Number of samples in `vals`
 *             vals   Integer array of input values to be searched
 *
 * @params[out] imax  Index of absolute maximum
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
 * @params[in] num    Number of samples in `vals`
 *             vals   Integer array of input values to be searched
 *
 * @params[out] max_val Index of absolute maximum
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
 * @params[in] num    Number of samples in `vals`
 *             vals   Integer array of input values to be searched
 *
 * @params[out] min_val Value absolute maximum
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
 * @brief Get the alt limits for a select group of data.  Look at the
 *        distribution of the data and fit a Gaussian curve to the occurance
 *        peaks to establish appropriate limits.
 *
 * @params[in] num - Number of points in `rg` and `vh` arrays
 *             rg - Array of range gates to consider
 *             vh - Array of virtual heights for each range gate
 *             vh_min - Minimum allowable virtual height in km
 *             vh_max - Maximum allowable virtual height in km
 *             vh_box - Width of virtual height box in km
 *             min_pnts - Minimum number of points allowed in a box
 *
 * @params[out] vh_mins - Lower virtual height limit of each bin
 *              vh_maxs - Upper virtual height limit of each bin
 *              out_num - Number of virtual height bins
 **/

int select_alt_groups(int num, int *rg, float *vh, float vh_min, float vh_max,
		      float vh_box, int min_pnts, float *vh_mins,
		      float *vh_maxs)
{
  int i, j, out_num, nbin, nmax, *hist_bins;

  float *hist_edges, local_min, local_max, vmin;

  double A, mu, sigma, *xdata;

  struct mp_result result;

  void histogram(int nvals, float vals[], int nbin, float val_min,
		 float val_max, int *hist_bins, float *leading_bin_val);
  int int_argrelmax(int num, int vals[], int order, int clip, int *ismax);
  int int_argabsmax(int num, int vals[]);
  float float_absmin(int num, float vals[]);
  float float_absmax(int num, float vals[]);

  out_num = 0;

  /* Create a histogram of the number of observations at each virtual height */
  nbin = (int)((vh_max - vh_min) / (vh_box * 0.25));
  if(nbin > 10) nbin = 10;
  hist_bins = (int *)calloc(num, sizeof(int));
  hist_edges = (float *)calloc(num, sizeof(float));
  histogram(num, vh, nbin, vh_min, vh_max, hist_bins, hist_edges);

  /* Find the maxima in the histogram */
  ismax = (int *)calloc(nbin, sizeof(int));
  nmax = int_argrelmax(nbin, hist_bins, 2, 1, ismax);

  if(nmax == 0)
    {
      /* A relative maximum can't be identified if two identical values */
      /* are side-by-side, use the absolute maximum.                    */
      nmax = int_argabsmax(nbin, hist_bins);

      /* Only use the absolute maximum if it is significant */
      if(hist_bins[nmax] >= min_points)
	{
	  ismax[nmax] = 1;
	  nmax = 1;
	}
      else nmax = 0;
    }

  /* Get the maximum and minimum of the virtual heights */
  local_min = float_absmin(num, vh);
  local_max = float_absmax(num, vh);

  /* Without a significant maximum, set the limits using the suggested width */
  if(nmax == 0)
    {
      out_num = (int)ceil((double)(local_max - local_min) / (double)vh_box);
      vmin    = (local_max - local_min) / (float)out_num + local_min - vh_box;

      for(i = 0; i < out_num; i++)
	{
	  vh_mins[i] = vmin + i * vh_box;
	  vh_maxs[i] = vh_mins[i] + vh_box;
	}
    }
  else
    {
      /* Cast the histogram bins as doubles for use by MINPACK */
      xdata = (double *)calloc(nbin, sizeof(double));
      for(i = 0; i < nbin; i++) xdata[i] = (double)hist_bins[i];

      /* For each maxima, fit a Gaussian */
      for(i = 0, j = 0; i < nmax && j < nbin; j++)
	{
	  if(ismax[j] == 1)
	    {
	      /* Initial amplitude is the number of points at the maximum */
	      A = xdata[j];

	      /* Initial mean is half the histogram box width */
	      if(nbin == 1)
		mu = (double)vh_box;
	      else if(j >= 0)
		mu = (double)(hist_edges[j + 1] - hist_edges[j]) / 2.0;
	      else
		mu = (double)(hist_edges[j] - hist_edges[j - 1]) / 2.0;

	      /* Initial sigma is half the virtual height box width */
	      sigma = 0.5 * vh_box;

	      /* Use non-linear least-squares to fit a Gaussian to the */
	      /* histogram.                                            */
	      /* To make this work:
	       * - redefine the gaussian routine to be the right format
	       * - get the inputs below correct
	       *
	       * Examples:
	       * codebase/superdarn/src.lib/tk/lmfit.1.0/src/lmfit.c
	       * include/analysis/mpfit.h
	       *
	       * AS WRITTEN BELOW, THIS DOES NOT WORK
	       */
	      stat = mpfit(gaussian, m, npar, xdata, pars, config, private_data,
			   &result);

	      if(result.status > 0)
		{
		  /* Get the 3-sigma limits */
	      
		  /* Get the 2-sigma limits */

		  /* Save the 3-sigma limits as the upper and lower virtual */
		  /* height box limits if the detected peak is within the   */
		  /* 2-sigma limits.  Also remove this peak from the xdata  */
		  /* array to allow lower level peaks to be identified.     */
		}
	    }
	}

      /* Evaluate the current limits to see if the overlap each or have gaps */
    }
	    
  
  /* Free the allocated memory */
  free(hist_bins);
  free(hist_edges);
  free(xdata);

  /* Return the number of virtual height bins as the routine status */
  return(out_num);
}
