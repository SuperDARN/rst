/* sort_rout.h
   ===========
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
