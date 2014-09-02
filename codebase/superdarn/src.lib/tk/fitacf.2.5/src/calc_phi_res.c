/* calc_phi_res.c
     ==============
     Author: R.J.Barnes & K.Baker
*/

/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
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



/*
    2003/09/04 Kile Baker
    changed the return type of this function from void to int
    the function now checks to see if all the phases turned out
    to be 0.  If so, it returns -1, otherwise it returns 0.

    The return code of -1 indicates that you can't really do a fit
    because all the phases were exactly 0.  This can happen when
    the power is low and we are reprocessing DAT files that have lost
    some precision.
*/

#include <math.h>
#include "rmath.h"



int calc_phi_res(struct complex *acf,int *badlag,
                    double *phi_res,int mplgs) {
    int i, n_good_lags;
    for (i=0, n_good_lags=0; i< mplgs; ++i) { 
        if (badlag[i]){
            phi_res[i] = 0.0;
        }
        else {
            phi_res[i] = atan2(acf[i].y, acf[i].x);
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
