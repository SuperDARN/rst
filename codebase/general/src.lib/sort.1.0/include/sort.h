/* sort.h
   ======
   Author: Angeline G. Burrell - NRL - 2021
   This is a U.S. government work and not under copyright protection in the U.S.

   This file is part of the Radar Software Toolkit (RST).

   Disclaimer: RST is licensed under GPL v3.0. Please visit 
               <https://www.gnu.org/licenses/> to see the full license

   Modifications:

*/

#ifndef _SORT_H
#define _SORT_H

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

void smart_sort(int num, double array[]);
void smart_argsort_float(int num, float array[], int sortargs[]);
void smart_sort_int(int num, int array[]);
void straight_sort(short int num, double array[]);
void straight_sort_int(short int num, int array[]);
void straight_argsort(short int num, double array[], int argout[]);
void smart_argsort_int(int num, int array[], int sortargs[]);
void quicksort(int num, double array[]);
void quicksort_int(int num, int array[]);
void quickargsort(int num, double array[], int argout[]);

#endif
