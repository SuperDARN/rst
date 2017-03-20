/* f2c.c
   =====
   Author R.J.Barnes
*/

/*
   See license.txt
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

