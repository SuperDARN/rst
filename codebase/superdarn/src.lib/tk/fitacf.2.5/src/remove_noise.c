/* remove_noise.c
   ==============
   Author: R.J.Barnes & K.Baker
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
 
    2020-11-12 Marina Schmidt Converted RST complex -> C library complex

 
*/



#include <math.h>
#include <complex.h>
#include "rmath.h"
#include "badsmp.h"
#include "fitblk.h"
#include "noise_stat.h"

void remove_noise(int mplgs, double complex *acf, double complex *ncf) {
  int i;
  double pa, plim;

  plim = lag_power(&ncf[0]);
  pa = lag_power(&acf[0]);

  if( pa > plim ) for (i=0; i < mplgs; i++) {
      acf[i] -= ncf[i];	
    }
  else for (i=0; i < mplgs; i++) {
    acf[i] = 0.0 + 0.0 * I;
  }
  return;
}
