/* more_badlags.c
   ==============
   Author: R.J.Barnes & K.Baker & P.Ponomarenko
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/ 



#include <math.h>
#include <stdio.h>
 
double more_badlags(double *w,int *badlag,
                   double noise_lev,int mplgs,int nave) {

  double fluct0, fluct, fluct_old;
  short int badflag_1, badflag_2, k_old, k;
  short int sum_np;

  badflag_1 = 0;
  badflag_2 = 0;

  fluct0 =  w[0]/sqrt(2.0*(double) nave);
  fluct =  w[0] + 2.0*noise_lev+fluct0; 
  fluct_old = fluct;
  sum_np = 0;
  k_old = 0;

  for (k=0; k<mplgs; k++) {
   if (badlag[k]) continue;
   if (badflag_2) badlag[k]=7;
   else if (w[k] <= w[0]/sqrt((double) nave)) {  /* if (w[k] <= 1.0) { */ 
   	badlag[k] = 3;
   	badflag_2 = badflag_1;
   	badflag_1 = 1;
  }  else {
    badflag_1 = 0;
   	if (w[k] > fluct) {
	 badlag[k] = 5; 
	 if (k < (mplgs - 1)) {
		if ((w[k] < fluct_old) && (w[k+1] > fluct) &&
			(w[k+1] < w[k])) {
		     badlag[k_old] = 9; 
		     --sum_np;
		     badlag[k] = 0; 
		  }
        } 
	  }
	  fluct_old = fluct;
   	  fluct = 2.0*noise_lev + w[k] + fluct0; 
   } 

    if (!badlag[k]) {
	  ++sum_np;
	  k_old = k; 
    }
  }
  return (double) sum_np;
}
