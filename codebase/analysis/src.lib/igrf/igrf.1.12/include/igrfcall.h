/* igrf.h
   ==========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/



#ifndef _IGRF_H
#define _IGRF_H


int IGRFCall(double date, double flat, double flon, 
         double elev, double *x, double *y, double *z);
 
#endif


