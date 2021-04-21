/* f2c.c
   =====
   Author R.J.Barnes
*/

/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "f2c.h"

/* implements portions of the f2c library to allow compilation on 
   systems without f2c 
*/


integer f2c_s_wsle(cilist *a) {
  return 0;
}

integer f2c_s_wsfe(cilist *a) {
  return 0;
}


integer f2c_e_wsle(void) {
  return 0;
}

integer f2c_e_wsfe(void) {
  return 0;
}

integer f2c_do_fio(ftnint *type,char *ptr,ftnlen len) {
  return 0;
}



integer f2c_do_lio(ftnint *type,ftnint *number,char *ptr,ftnlen len) {
  return 0;
}





double f2c_d_mod (doublereal * x, doublereal * y) {
#ifdef IEEE_drem
  double xa, ya, z;
  if ((ya = *y) < 0.)
    ya = -ya;
  z = drem (xa = *x, ya);
  if (xa > 0)
    {
      if (z < 0)
	z += ya;
    }
  else if (z > 0)
    z -= ya;
  return z;
#else
  double quotient;
  if ((quotient = *x / *y) >= 0)
    quotient = floor (quotient);
  else
    quotient = -floor (-quotient);
  return (*x - (*y) * quotient);
#endif
}




double f2c_pow_dd(doublereal *ap,doublereal *bp) {
  return (pow(*ap,*bp));
}

