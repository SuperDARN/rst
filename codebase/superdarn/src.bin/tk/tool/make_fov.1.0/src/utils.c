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

#ifndef INFINITY
#define INFINITY 9.99E7
#endif

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

  void smart_sort_int(int num, int array[]);

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

int get_unique_str(int in_num, char **in_str, ichar **out_str)
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
