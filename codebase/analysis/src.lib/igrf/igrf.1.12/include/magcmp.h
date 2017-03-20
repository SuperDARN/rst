/* magcmp.h
   ========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/




#ifndef _MAGCMP_H
#define _MAGCMP_H

int IGRFMagCmp(double date, double frho, double flat, double flon, 
                double *bx, double *by, double *bz, double *b);

#endif
