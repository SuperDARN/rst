/* rylm.c
   ======
   Author: R.J.Barnes
*/

/*
   See license.txt
*/




#include <math.h>
#include "rmath.h"


int rylm(double colat,double lon,int order,
	  double *ylmval) {
   
    double d1;
    struct complex z1, z2;

    /* Local variables */
    struct complex q_fac;
    struct complex q_val;
    int l, m;
    int la,lb,lc,ld,le,lf;

    double cos_theta,sin_theta;  
    double ca,cb;
    double fac;
    double cos_lon, sin_lon;

    cos_theta = cos(colat);
    sin_theta = sin(colat);

    cos_lon = cos(lon);
    sin_lon = sin(lon);

    d1 = -sin_theta;
    z2.x = cos_lon;
    z2.y = sin_lon;

    z1.x = d1 * z2.x;
    z1.y = d1 * z2.y;

    q_fac.x = z1.x;
    q_fac.y = z1.y;

    ylmval[0] = 1;
    ylmval[2] = cos_theta;

    for (l = 1; l <= (order-1); l++) {
	la = (l - 1) * l + 1;
	lb = l * (l + 1) + 1;
	lc = (l + 1) * (l + 2) + 1;

	ca =  ((double) (l * 2 + 1)) / (l + 1);
	cb =  ((double) l) / (l + 1);

	ylmval[lc-1] = ca * cos_theta * ylmval[lb-1] - cb * ylmval[la-1];
    }

    q_val.x = q_fac.x;
    q_val.y = q_fac.y;

    ylmval[3] = q_val.x;
    ylmval[1] = -q_val.y;
    for (l = 2; l <= order; l++) {

	d1 = l*2 - 1.;
	z2.x = d1 * q_fac.x;
	z2.y = d1 * q_fac.y;
	z1.x = z2.x * q_val.x - z2.y * q_val.y;
	z1.y = z2.x * q_val.y + z2.y * q_val.x;
	q_val.x = z1.x;
	q_val.y = z1.y;

	la = l*l + (2*l) + 1;
	lb = l*l + 1;

	ylmval[la-1] = q_val.x;
	ylmval[lb-1] = -q_val.y;
    }

    for (l = 2; l <= order; l++) {

	la = l*l;
	lb = l*l - 2*(l - 1);

	lc = l*l + (2*l);
	ld = l*l + 2;

	fac = l*2 - 1;

	ylmval[lc-1] = fac * cos_theta * ylmval[la-1];
	ylmval[ld-1] = fac * cos_theta * ylmval[lb-1];
    }

    for (m = 1; m <= (order-2); m++) {

	la = (m+1)*(m+1);
        lb = (m+2)*(m+2)-1;
        lc = (m+3)*(m+3)-2;
	
	ld = la - (2*m);
	le = lb - (2*m);
	lf = lc - (2*m);

	for (l = m + 2; l <= order; l++) {
	    ca =  ((double) (2*l - 1)) / (l - m);
	    cb =  ((double) (l+m - 1)) / (l - m);

	    ylmval[lc-1] = ca * cos_theta *ylmval[lb-1] - cb *ylmval[la-1];
	    ylmval[lf-1] = ca * cos_theta *ylmval[le-1] - cb *ylmval[ld-1];

	    la = lb;
	    lb = lc;
	    lc = lb + (2*l) + 2;

	    ld = la - (2*m);
	    le = lb - (2*m);
	    lf = lc - (2*m);

	}
    }
    return 0;
} 














