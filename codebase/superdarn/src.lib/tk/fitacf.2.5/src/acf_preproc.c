/* acf_preproc.c
   =============
   Author: R.J.Barnes & K.Baker
*/

/*
   See license.txt
*/



#include <stdio.h>
#include <math.h>

#include "rmath.h"
#include "acf_preproc.h"

int acf_preproc(struct complex *acf,
                struct complex *orig_acf,
                double *noise_lev,
                int range,int *badlag,
                int mplgs) {
  int k;
  
  for (k=0; k < mplgs; k++) {
    acf[k].x = orig_acf[k].x;
    acf[k].y = orig_acf[k].y;
  }
  return ACF_UNMODIFIED; 
}


