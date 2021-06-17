/* utils.c
   =============
   Author: Angeline G. Burrell - NRL - 2020
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

#ifndef INFINITY
#define INFINITY 9.99E7
#endif

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
  
