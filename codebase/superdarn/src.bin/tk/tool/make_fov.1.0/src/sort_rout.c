#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************************
 * Sort_Rout
 *
 * Author: Angeline G. Burrell, UTDallas, March 2009
 *
 * Reference: Numerical Recipes in C, 2nd Ed.
 **************************************************************************/

#ifndef SWAP
#define SWAP(a,b) temp=(a);(a)=(b);(b)=temp;
#endif

#ifndef SIGN
#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))
#endif

#ifndef SHIFT
#define SHIFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d);
#endif

#ifndef NSTACK
#define NSTACK 50
#endif

#ifndef MSTACK
#define MSTACK 7
#endif

/***************************************************************************
 * Smart_Sort: Chooses the most effecient sort routine based on available
 *             tested options.  Rearranges the input array.
 *
 * Reference: Numerical Recipes in C, 2nd Ed.
 **************************************************************************/

void smart_sort(int num, double array[])
{
  void straight_sort(short int n, double array[]);
  void shell_sort(int num, double array[]);
  void quicksort(int num, double array[]);

  if(num < MSTACK)       straight_sort((short int)num, array);
  else if(num <= NSTACK) shell_sort(num, array);
  else                   quicksort(num, array);

  return;
}

/***************************************************************************
 * Smart_Sort_Int: Chooses the most effecient sort routine based on available
 *                 tested options.  Rearranges the integer input array.
 *
 * Reference: Numerical Recipes in C, 2nd Ed.
 **************************************************************************/

void smart_sort_int(int num, int array[])
{
  void straight_sort_int(short int n, int array[]);
  void shell_sort_int(int num, int array[]);
  void quicksort_int(int num, int array[]);

  if(num < MSTACK)       straight_sort_int((short int)num, array);
  else                   quicksort_int(num, array);
  //  else if(num <= NSTACK) shell_sort_int(num, array);

  return;
}

/***************************************************************************
 * Straight_Sort: Straight insertion method for sorted, N^2 method that works
 *                best for N < 20.
 *
 * Reference: Numerical Recipes in C, 2nd Ed.
 **************************************************************************/

void straight_sort(short int n, double array[])
{
  int i, j;

  double a;

  for(j = 1; j < n; j++)
    {
      a = array[j];
      i = j - 1;

      /* Search for the proper place to insert a */

      while(i >= 0 && array[i] > a)
	{
	  array[i + 1] = array[i];
	  i--;
	}

      /* Insert a */

      array[i + 1] = a;
    }

  return;
}

/***************************************************************************
 * Straight_Sort_Int: Straight insertion method for sorted, N^2 method that
 *                    works best for N < 20.
 *
 * Reference: Numerical Recipes in C, 2nd Ed. p 330
 **************************************************************************/

void straight_sort_int(short int n, int array[])
{
  int i, j, a;

  for(j = 1; j < n; j++)
    {
      a = array[j];
      i = j - 1;

      /* Search for the proper place to insert a */

      while(i >= 0 && array[i] > a)
	{
	  array[i + 1] = array[i];
	  i--;
	}

      /* Insert a */

      array[i + 1] = a;
    }

  return;
}

/***************************************************************************
 * Shell_Sort: A sorting routine that uses straight insertion for incrimental,
 *             sequential, partial sorts.  Works best for randomly ordered
 *             arrays, with operations of order of N^1.5 - N^1.25 depending
 *             on sequencing and size of the array.  Works for N < 6,000 but
 *             not as well as quicksort for N > 50.
 *
 * Reference: Numerical Recipes in C, 2nd Ed.
 **************************************************************************/

void shell_sort(int num, double array[])
{
  int i, j, inc = 1;

  double a;

  printf("SHELL SORT IS BROKE\n");
  exit(1);
  
  /* Determine the starting incriment */

  do
    {
      inc *= 3;
      inc++;
    } while (inc <= num);

  /* Loop over the partial sorts */

  do
    {
      inc /= 3;

      /* Outer loop of straight insertion */

      for(i = inc; i < num; i++)
	{
	  a = array[i];
	  j = i;

	  /* Inner loop of straight insertion */

	  while(array[j - inc] > a)
	    {
	      array[j] = array[j - inc];
	      j       -= inc;

	      if(j <= inc) break;
	    }

	  array[j] = a;
	}
    } while(inc > 1);

  return;
}

/***************************************************************************
 * Shell_Sort_Int: A sorting routine that uses straight insertion for
 *                 incrimental, sequential, partial sorts.  Works best for
 *                 randomly ordered arrays, with operations of order of
 *                 N^1.5 - N^1.25 depending on sequencing and size of the 
 *                 array.  Works for N < 6,000 but not as well as quicksort for
 *                 N > 50.
 *
 * Reference: Numerical Recipes in C, 2nd Ed.
 **************************************************************************/

void shell_sort_int(int num, int array[])
{
  int i, j, inc = 1;

  int a;

  /* Determine the starting incriment */
  printf("SHELL SORT INT IS BROKE\n");
  exit(1);

  do
    {
      inc *= 3;
      inc++;
    } while (inc <= num);

  /* Loop over the partial sorts */

  do
    {
      inc /= 3;

      /* Outer loop of straight insertion */
      for(i = inc; i < num; i++)
	{
	  a = array[i];
	  j = i;

	  /* Inner loop of straight insertion */

	  while(array[j - inc] > a)
	    {
	      array[j] = array[j - inc];
	      j       -= inc;

	      if(j <= inc) break;
	    }

	  array[j] = a;
	}
    } while(inc > 1);

  return;
}

/***************************************************************************
 * Quicksort: Typically the fasted sorting algorithm for large N.
 *
 * Reference: Numerical Recipes in C, 2nd Ed.
 **************************************************************************/

void quicksort(int num, double array[])
{
  int jstack = -1, i, ir = num - 1, j, k, l = 0, *istack;

  double a;

  char module_name[20];

  sprintf(module_name, "quicksort");

  istack = (int *)calloc(NSTACK, sizeof(int));

  for(;;)
    {
      if(ir - l < MSTACK)
	{
	  /* Use insertion sort once subarray is small enough */

	  for(j = l + 1; j <= ir; j++)
	    {
	      a = array[j];

	      for(i = j - 1; i >= l; i--)
		{
		  if(array[i] <= a) break;
		  array[i + 1] = array[i];
		}

	      array[i + 1] = a;
	    }

	  if(jstack == 0) break;

	  /* Pop stack and begin a new round of partitioning */

	  ir = istack[jstack--];
	  l  = istack[jstack--];
	}
      else
	{
	  /* Choose median of left, center and right elements as */
	  /* partitioning element a.  Also rearrange so that     */
	  /* array[l-1] <= array[l] <= array[ir-1]               */

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

	  i = l + 1;
	  j = ir;
	  a = array[l + 1];

	  /* Beginning of innermost loop, scanning up and down to find */
	  /* elements > a and < a                                      */

	  for(;;)
	    {
	      do i++; while(array[i] < a);
	      do j--; while(array[j] > a);

	      if(j < i) break;

	      SWAP(array[i], array[j]);
	    }

	  array[l + 1] = array[j];
	  array[j] = a;
	  jstack  += 2;

	  /* Push pointers to larger subarray on stack, process smaller */
	  /* subarray immediately                                       */

	  if(jstack > NSTACK)
	    {
	      printf("%s ERROR: stack size of subarray too small [%d < %d]\n",
		     module_name, NSTACK, jstack);
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

/***************************************************************************
 * Quicksort_Int: Typically the fasted sorting algorithm for large N.
 *
 * Reference: Numerical Recipes in C, 2nd Ed.
 **************************************************************************/

void quicksort_int(int num, int array[])
{
  int a, jstack = -1, i, ir = num - 1, j, k, l = 0, *istack;

  char module_name[20];

  sprintf(module_name, "quicksort");
  istack = (int *)calloc(NSTACK, sizeof(int));

  for(;;)
    {
      if(ir - l < MSTACK)
	{
	  /* Use insertion sort once subarray is small enough */

	  for(j = l + 1; j <= ir; j++)
	    {
	      a = array[j];

	      for(i = j - 1; i >= l; i--)
		{
		  if(array[i] <= a) break;
		  array[i + 1] = array[i];
		}

	      array[i + 1] = a;
	    }

	  if(jstack == -1) break;

	  /* Pop stack and begin a new round of partitioning */

	  ir = istack[jstack--];
	  l  = istack[jstack--];
	}
      else
	{
	  /* Choose median of left, center and right elements as */
	  /* partitioning element a.  Also rearrange so that     */
	  /* array[l-1] <= array[l] <= array[ir-1]               */

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

	  i = l + 1;
	  j = ir;
	  a = array[l + 1];

	  /* Beginning of innermost loop, scanning up and down to find */
	  /* elements > a and < a                                      */

	  for(;;)
	    {
	      do i++; while(array[i] < a);
	      do j--; while(array[j] > a);

	      if(j < i) break;

	      SWAP(array[i], array[j]);
	    }

	  array[l + 1] = array[j];
	  array[j] = a;
	  jstack  += 2;

	  /* Push pointers to larger subarray on stack, process smaller */
	  /* subarray immediately                                       */

	  if(jstack > NSTACK)
	    {
	      printf("%s ERROR: stack size of subarray too small [%d < %d]\n",
		     module_name, NSTACK, jstack);
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
