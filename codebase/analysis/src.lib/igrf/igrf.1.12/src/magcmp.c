/* magcmp.c
   ========
   Author: R.J.Barnes
   Comments: E.G.Thomas (2016)
*/

/*
   See license.txt
*/

#include <math.h>
#include <stdio.h>
#include "igrfcall.h"



/**
 * Calculates the IGRF magnetic field components at a given location in
 * geocentric coordinates.
 **/
int IGRFMagCmp(double date, double frho, double flat, double flon, 
               double *bx, double *by, double *bz, double *b) {

    int s;

    /* Calculate virtual height of range/beam measurement [km]
     * using fixed value for Earth radius */
    frho = frho - 6372.;

    /* Calculate magnetic field componenets at field point */
    s=IGRFCall(date, flat, flon, frho, bx, by, bz);

    /* Calculate magnitude of magnetic field vector */
    *b = sqrt(*bx * *bx + *by * *by + *bz * *bz);

    /* For some reason we reverse the sign of Bx and Bz components? */
    *bx = -*bx;
    *bz = -*bz;

    return s;

}
