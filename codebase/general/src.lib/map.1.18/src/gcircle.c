/* gcircle.c
   ========= 
   Author: R.J.Barnes
*/

/*
   See license.txt
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> 
#include <sys/types.h>
#include "rtypes.h"
#include "rfbuffer.h"
#include "iplot.h"
#include "polygon.h"
#include "rmath.h"
#include "rmap.h"


int MapGreatCircle(void *pnt1,void *pnt2,double f,void *dst) {

  double d;
  double A,B,x,y,z;
  double lat1,lat2,lon1,lon2;
  float *pp1,*pp2,*dp;

  pp1=(float *) pnt1;
  pp2=(float *) pnt2;
  dp=(float *) dst;
  
  lat1=pp1[0]*PI/180.0;
  lat2=pp2[0]*PI/180.0;

  lon1=pp1[1]*PI/180.0;
  lon2=pp2[1]*PI/180.0;

  d=acos(sin(lat1)*sin(lat2)+cos(lat1)*cos(lat2)*cos(lon1-lon2));

  A=sin((1-f)*d)/sin(d);
  B=sin(f*d)/sin(d);
  x = A*cos(lat1)*cos(lon1) +  B*cos(lat2)*cos(lon2);
  y = A*cos(lat1)*sin(lon1) +  B*cos(lat2)*sin(lon2);
  z = A*sin(lat1)           +  B*sin(lat2);
  dp[0]=180.0*atan2(z,sqrt(x*x+y*y))/PI;
  dp[1]=180.0*atan2(y,x)/PI;

  return 0;
}

