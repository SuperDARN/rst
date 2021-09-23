/* sort_rout.c
   ===========
   Author: Angeline G. Burrell - UTDallas, NRL - 2021
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

#include "sort_rout.h"

/**
 * @brief Sorts input using the most effecient of the available sort routines
 *
 * @params[in] num   - Number of values in input array
 *             array - Array of double values to sort, has length `num`
 *
 * @notes Rearranges input in `array` to be ordered from least to greatest
 *
 * @reference Numerical Recipes in C, 2nd Ed.
 **/

void smart_sort(int num, double array[])
{
  if(num < MSTACK) straight_sort((short int)num, array);
  else             quicksort(num, array);

  return;
}

/**
 * @brief Returns the indices that would sort the input array
 *
 * @params[in] num   - number of values in input `array`
 *             array - input data array of float values with length `num`
 *
 * @params[out] sortargs - array of length `num` containing the zero-index
 *                         integers needed to sort `array`
 *
 * @notes Does not sort the input array
 *
 * @reference Numerical Recipes in C, 2nd Ed.
 **/

void smart_argsort_float(int num, float array[], int sortargs[])
{
  int i;
  double *sorted_array;

  sorted_array = (double *)calloc(num, sizeof(double));
  for(i = 0; i < num; i++) sorted_array[i] = (double)array[i];

  if(num < MSTACK) straight_argsort((short int)num, sorted_array, sortargs);
  else             quickargsort(num, sorted_array, sortargs);

  return;
}

/**
 * @brief Returns the indices that would sort the input array
 *
 * @params[in] num   - number of values in input `array`
 *             array - input data array of integer values with length `num`
 *
 * @params[out] sortargs - array of length `num` containing the zero-index
 *                         integers needed to sort `array`
 *
 * @notes Does not sort the input array
 *
 * @reference Numerical Recipes in C, 2nd Ed.
 **/

void smart_argsort_int(int num, int array[], int sortargs[])
{
  int i;
  double *sorted_array;

  sorted_array = (double *)calloc(num, sizeof(double));
  for(i = 0; i < num; i++) sorted_array[i] = (double)array[i];

  if(num < MSTACK) straight_argsort((short int)num, sorted_array, sortargs);
  else             quickargsort(num, sorted_array, sortargs);

  return;
}

/**
 * @brief Sorts input using the most effecient of the available sort routines
 *
 * @params[in] num   - Number of values in input array
 *             array - Array of integer values to sort, has length `num`
 *
 * @notes Rearranges input in `array` to be ordered from least to greatest
 *
 * @reference Numerical Recipes in C, 2nd Ed.
 **/

void smart_sort_int(int num, int array[])
{
  if(num < MSTACK) straight_sort_int((short int)num, array);
  else             quicksort_int(num, array);

  return;
}

/**
 * @brief Straight insertion method for sorting, N^2 method, best for N < 20.
 *
 * @params[in] num   - Number of values in input array
 *             array - Array of double values to sort, has length `num`
 *
 * @reference Numerical Recipes in C, 2nd Ed.
 **/

void straight_sort(short int num, double array[])
{
  int i, j;

  double array_val;

  for(j = 1; j < num; j++)
    {
      array_val = array[j];
      i         = j - 1;

      /* Search for the proper place to insert a */

      while(i >= 0 && array[i] > array_val)
	{
	  array[i + 1] = array[i];
	  i--;
	}

      /* Insert a */

      array[i + 1] = array_val;
    }

  return;
}

/**
 * @brief Straight insertion method for sorting; N^2 method, best for N < 20.
 *
 * @params[in] num   - Number of values in input array
 *             array - Array of integer values to sort, has length `num`
 *
 * @reference Numerical Recipes in C, 2nd Ed.
 **/

void straight_sort_int(short int num, int array[])
{
  int i, j, array_val;

  for(j = 1; j < num; j++)
    {
      array_val = array[j];
      i         = j - 1;

      /* Search for the proper place to insert a */
      while(i >= 0 && array[i] > array_val)
	{
	  array[i + 1] = array[i];
	  i--;
	}

      /* Insert the held array value into its new location */
      array[i + 1] = array_val;
    }

  return;
}

/**
 * @brief Straight insertion method for sorted; N^2 method, best for N < 20.
 *
 * @params[in] num   - length of the input array
 *             array - input data that will be sorted
 *
 * @params[out] argout - sorted indices that would sort the unsorted input
 *
 * @reference Numerical Recipes in C, 2nd Ed.
 **/

void straight_argsort(short int num, double array[], int argout[])
{
  int i, j, ival;

  double array_val;

  /* Initialize the arguement output array */
  for(i = 0; i < num; i++)
    argout[i] = i;

  /* Cycle through the array, starting at the second point */
  for(j = 1; j < num; j++)
    {
      array_val = array[j];
      ival      = argout[j];
      i         = j - 1;

      /* Search for the proper place to insert a */
      while(i >= 0 && array[i] > array_val)
	{
	  array[i + 1]  = array[i];
	  argout[i + 1] = argout[i];
	  i--;
	}

      /* Insert the held array value and value index */
      array[i + 1]  = array_val;
      argout[i + 1] = ival;
    }

  return;
}

/**
 * @brief Sorts data, typically the fasted sorting algorithm for large N.
 *
 * @params[in] num   - length of the input array
 *             array - input data that will be sorted
 *
 * @reference Numerical Recipes in C, 2nd Ed.
 **/

void quicksort(int num, double array[])
{
  int jstack = -1, i, ir = num - 1, j, k, l = 0, *istack;

  double temp, array_val;

  istack = (int *)calloc(NSTACK, sizeof(int));

  for(;;)
    {
      if(ir - l < MSTACK)
	{
	  /* Use insertion sort once subarray is small enough */
	  for(j = l + 1; j <= ir; j++)
	    {
	      array_val = array[j];

	      for(i = j - 1; i >= l; i--)
		{
		  if(array[i] <= array_val) break;
		  array[i + 1] = array[i];
		}

	      array[i + 1] = array_val;
	    }

	  if(jstack == 0) break;

	  /* Pop stack and begin a new round of partitioning */
	  ir = istack[jstack--];
	  l  = istack[jstack--];
	}
      else
	{
	  /* Choose median of left, center and right elements as */
	  /* partitioning element `array_val`.  Also rearrange   */
	  /* so that array[l-1] <= array[l] <= array[ir-1]       */
	  k = (l + ir) >> 1;
	  SWAP(array[k], array[l + 1]);

	  /* NOTE: compilation error if parenthesis excluded in if statements */
	  if(array[l] > array[ir])
	    {
	      SWAP(array[l], array[ir]);
	    }
	  if(array[l + 1] > array[ir])
	    {
	      SWAP(array[l + 1], array[ir]);
	    }
	  if(array[l] > array[l + 1])
	    {
	      SWAP(array[l], array[l + 1]);
	    }

	  /* Inistialize pointers for partitioning */
	  i         = l + 1;
	  j         = ir;
	  array_val = array[l + 1];

	  /* Beginning of innermost loop, scanning up and down to find */
	  /* elements > array_val and < array_val                      */
	  for(;;)
	    {
	      do i++; while(array[i] < array_val);
	      do j--; while(array[j] > array_val);

	      if(j < i) break;

	      SWAP(array[i], array[j]);
	    }

	  array[l + 1] = array[j];
	  array[j]     = array_val;
	  jstack      += 2;

	  /* Push pointers to larger subarray on stack, process smaller */
	  /* subarray immediately                                       */
	  if(jstack > NSTACK)
	    {
	      printf("%s ERROR: stack size of subarray too small [%d < %d]\n",
		     "quicksort", NSTACK, jstack);
	      exit(1);
	    }

	  if(ir - i + 1 >= j - 1)
	    {
	      istack[jstack]     = ir;
	      istack[jstack - 1] = i;
	      ir                 = j - 1;
	    }
	  else
	    {
	      istack[jstack]     = j - 1;
	      istack[jstack - 1] = l;
	      l                  = i;
	    }
	}
    }

  free(istack);
  return;
}

/**
 * @brief Sorts int data, typically the fasted sorting algorithm for large N.
 *
 * @params[in] num   - length of the input array
 *             array - input data that will be sorted
 *
 * @reference Numerical Recipes in C, 2nd Ed.
 **/

void quicksort_int(int num, int array[])
{
  int temp, array_val, jstack = -1, i, ir = num - 1, j, k, l = 0, *istack;

  istack = (int *)calloc(NSTACK, sizeof(int));

  for(;;)
    {
      if(ir - l < MSTACK)
	{
	  /* Use insertion sort once subarray is small enough */
	  for(j = l + 1; j <= ir; j++)
	    {
	      array_val = array[j];

	      for(i = j - 1; i >= l; i--)
		{
		  if(array[i] <= array_val) break;
		  array[i + 1] = array[i];
		}

	      array[i + 1] = array_val;
	    }

	  if(jstack == -1) break;

	  /* Pop stack and begin a new round of partitioning */
	  ir = istack[jstack--];
	  l  = istack[jstack--];
	}
      else
	{
	  /* Choose median of left, center and right elements as */
	  /* partitioning element `array_val`.  Also rearrange   */
	  /* so that array[l-1] <= array[l] <= array[ir-1]       */
	  k = (l + ir) >> 1;
	  SWAP(array[k], array[l + 1]);

	  /* NOTE: compilation error if parenthesis excluded in if statements */
	  if(array[l] > array[ir])
	    {
	      SWAP(array[l], array[ir]);
	    }
	  if(array[l + 1] > array[ir])
	    {
	      SWAP(array[l + 1], array[ir]);
	    }
	  if(array[l] > array[l + 1])
	    {
	      SWAP(array[l], array[l + 1]);
	    }

	  /* Initialize pointers for partitioning */
	  i         = l + 1;
	  j         = ir;
	  array_val = array[l + 1];

	  /* Beginning of innermost loop, scanning up and down to find */
	  /* elements > array_val and < array_val                      */
	  for(;;)
	    {
	      do i++; while(array[i] < array_val);
	      do j--; while(array[j] > array_val);

	      if(j < i) break;

	      SWAP(array[i], array[j]);
	    }

	  array[l + 1] = array[j];
	  array[j]     = array_val;
	  jstack      += 2;

	  /* Push pointers to larger subarray on stack, process smaller */
	  /* subarray immediately                                       */
	  if(jstack > NSTACK)
	    {
	      printf("%s ERROR: stack size of subarray too small [%d < %d]\n",
		     "quicksort_int", NSTACK, jstack);
	      exit(1);
	    }

	  if(ir - i + 1 >= j - 1)
	    {
	      istack[jstack]     = ir;
	      istack[jstack - 1] = i;
	      ir                 = j - 1;
	    }
	  else
	    {
	      istack[jstack]     = j - 1;
	      istack[jstack - 1] = l;
	      l                  = i;
	    }
	}
    }

  free(istack);
  return;
}

/**
 * @brief Sorts data, typically the fasted sorting algorithm for large N.
 *
 * @params[in] num   - length of the input array
 *             array - input data that will be sorted
 *
 * @params[out] argout - sorted indices that would sort the unsorted input
 *
 * @reference Numerical Recipes in C, 2nd Ed.
 **/

void quickargsort(int num, double array[], int argout[])
{
  int ival, jstack = -1, i, ir = num - 1, j, k, l = 0, *istack;

  double temp, array_val;

  /* Initialize the stack and the sorted index output */
  istack = (int *)calloc(NSTACK, sizeof(int));
  for(i = 0; i < num; i++)
    argout[i] = i;

  for(;;)
    {
      if(ir - l < MSTACK)
	{
	  /* Use insertion sort once subarray is small enough */
	  for(j = l + 1; j <= ir; j++)
	    {
	      array_val = array[j];
	      ival      = argout[j];

	      for(i = j - 1; i >= l; i--)
		{
		  if(array[i] <= array_val) break;
		  array[i + 1]  = array[i];
		  argout[i + 1] = argout[i];
		}

	      array[i + 1]  = array_val;
	      argout[i + 1] = ival;
	    }

	  if(jstack == 0) break;

	  /* Pop stack and begin a new round of partitioning */
	  ir = istack[jstack--];
	  l  = istack[jstack--];
	}
      else
	{
	  /* Choose median of left, center and right elements as */
	  /* partitioning element `array_val`.  Also rearrange   */
	  /* so that array[l-1] <= array[l] <= array[ir-1]       */
	  k = (l + ir) >> 1;
	  SWAP(array[k], array[l + 1]);
	  SWAP(argout[k], argout[l + 1]);

	  /* NOTE: compilation error if parenthesis excluded in if statements */
	  if(array[l] > array[ir])
	    {
	      SWAP(array[l], array[ir]);
	      SWAP(argout[l], argout[ir]);
	    }
	  if(array[l + 1] > array[ir])
	    {
	      SWAP(array[l + 1], array[ir]);
	      SWAP(argout[l + 1], argout[ir]);
	    }
	  if(array[l] > array[l + 1])
	    {
	      SWAP(array[l], array[l + 1]);
	      SWAP(argout[l], argout[l + 1]);
	    }

	  /* Inistialize pointers for partitioning */
	  i         = l + 1;
	  j         = ir;
	  array_val = array[l + 1];
	  ival      = argout[l + 1];

	  /* Beginning of innermost loop, scanning up and down to find */
	  /* elements > array_val and < array_val                      */
	  for(;;)
	    {
	      do i++; while(array[i] < array_val);
	      do j--; while(array[j] > array_val);

	      if(j < i) break;

	      SWAP(array[i], array[j]);
	      SWAP(argout[i], argout[j]);
	    }

	  array[l + 1]  = array[j];
	  array[j]      = array_val;
	  argout[l + 1] = argout[j];
	  argout[j]     = ival;
	  jstack       += 2;

	  /* Push pointers to larger subarray on stack, process smaller */
	  /* subarray immediately                                       */
	  if(jstack > NSTACK)
	    {
	      printf("%s ERROR: stack size of subarray too small [%d < %d]\n",
		     "quicksort", NSTACK, jstack);
	      exit(1);
	    }

	  if(ir - i + 1 >= j - 1)
	    {
	      istack[jstack]     = ir;
	      istack[jstack - 1] = i;
	      ir                 = j - 1;
	    }
	  else
	    {
	      istack[jstack]     = j - 1;
	      istack[jstack - 1] = l;
	      l                  = i;
	    }
	}
    }

  free(istack);
  return;
}
