/* calcvector.c
   ============
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rmath.h"

#define Re 6362
#define h 400



void RPosCalcVector(double lat,double lon,double mag,double azm,
		 double *clat,double *clon) {
   
  /* a routine to transform vectors for plotting */

  double ar,cr,br,arg,asr,as;
 
  if (mag==0) {
     *clat=lat;
     *clon=lon;
     return;
  }
  ar=azm*PI/180.0;  
  cr=(90.0-lat)*PI/180.0;
  br=0.2*mag/(Re+h);
  
  arg=cos(br)*cos(cr)+sin(br)*sin(cr)*cos(ar);
  asr=acos(arg);
  as=asr*180.0/PI;
  *clat=90-as;
  if (asr !=0) arg=sin(ar)*sin(br)/sin(asr);
    else arg=0;
  *clon=lon+asin(arg)*180.0/PI;
}












