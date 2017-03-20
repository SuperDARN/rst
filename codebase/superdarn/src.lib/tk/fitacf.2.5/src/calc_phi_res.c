/* calc_phi_res.c
     ==============
     Author: R.J.Barnes & K.Baker
*/

/*
   See license.txt
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
