/* acf_preproc.c
   =============
   Author: R.J.Barnes & K.Baker

 Copyright (C) <year>  <name of author>

Copyright (C) <year>  <name of author>

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
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 
 
*/



#include <stdio.h>
#include <math.h>
#include <complex.h>

#include "rmath.h"
#include "acf_preproc.h"

int acf_preproc(double complex *acf, double complex *orig_acf, 
        double *noise_lev, int range, int *badlag, int mplgs) {
  int k;
  
  for (k=0; k < mplgs; k++) {
    acf[k] = orig_acf[k];
  }
  return ACF_UNMODIFIED; 
}


