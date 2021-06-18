/* calc_phi_res.c
     ==============
     Author: R.J.Barnes & K.Baker

 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

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


int calc_phi_res(double complex *acf,int *badlag, double *phi_res,int mplgs) {
    int i, n_good_lags;
    for (i=0, n_good_lags=0; i< mplgs; ++i) { 
        if (badlag[i]){
            phi_res[i] = 0.0;
        }
        else {
            phi_res[i] = atan2(cimag(acf[i]), creal(acf[i]));
            n_good_lags++;
        }
    }
    if (n_good_lags > 0) {
        return 0;
    }
    else {
        return -1;
    }
}
