/* acf_preproc.c
   =============
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


