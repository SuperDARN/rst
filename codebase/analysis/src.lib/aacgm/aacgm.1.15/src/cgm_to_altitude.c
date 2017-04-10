/* cgm_to_altitude.c
   =================
   Author: R.J.Barnes
*/

/*
   See license.txt
*/




#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "math.h"


int cgm_to_altitude(double r_height_in,double r_lat_in,double *r_lat_adj) {
    double eradius = 6371.2;
    double unim=1;
    double  r1;
    double ra;
    int error=0;

   /* Compute the corresponding altitude adjusted dipole latitude. */
   /* Computing 2nd power */

    r1 = cosd(r_lat_in);
    ra = (r_height_in/ eradius+1)*(r1*r1);
    if (ra > unim) {
	ra = unim;
        error=1;
    }

    r1 = acos(sqrt(ra));
    *r_lat_adj = sgn(r1,r_lat_in)*180/PI;
    return error;
}

