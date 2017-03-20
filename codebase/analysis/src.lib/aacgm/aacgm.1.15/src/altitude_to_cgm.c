/* altitude_to_cgm.c
   =================
   Author: R.J.Barnes
*/

/*
   See license.txt
*/




#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "math.h"

void altitude_to_cgm(double r_height_in,double  r_lat_alt,
		     double *r_lat_adj) {
   
  double eradius =6371.2;
  double eps =1e-9;
  double unim =0.9999999;

  double r1;
  double r0, ra;

   /* Computing 2nd power */
  r1 = cosd(r_lat_alt);
  ra = r1 * r1;
  if (ra < eps) ra = eps;
  r0 = (r_height_in/eradius+1) / ra;
  if (r0 < unim) r0 = unim;
  
  r1 = acos(sqrt(1/r0));
  *r_lat_adj= sgn(r1, r_lat_alt)*180/PI;

} 

