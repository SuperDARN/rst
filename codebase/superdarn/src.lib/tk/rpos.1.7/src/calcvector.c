/* calcvector.c
   ============
   Author: R.J.Barnes

    Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rmath.h"

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
  br=0.2*mag/(RE+h);
  
  arg=cos(br)*cos(cr)+sin(br)*sin(cr)*cos(ar);
  asr=acos(arg);
  as=asr*180.0/PI;
  *clat=90-as;
  if (asr !=0) arg=sin(ar)*sin(br)/sin(asr);
    else arg=0;
  *clon=lon+asin(arg)*180.0/PI;
}












