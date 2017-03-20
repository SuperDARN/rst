/* sza.c
   =====
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> 
#include "rmath.h"
#include "astalg.h"

double SZASolarDec(int yr,int mo,int dy,int hr,int mt,int sc) {

  double jd;
  double dd;
  
  dd=AstAlg_dday(dy-1,hr,mt,sc);
  jd=AstAlg_jde(yr,mo,dd);
  
  return AstAlg_solar_declination(jd); 
}

double SZAEqOfTime(int yr,int mo,int dy,int hr,int mt,int sc) {

  double jd;
  double dd;
  double eqt;

  dd=AstAlg_dday(dy-1,hr,mt,sc);
  jd=AstAlg_jde(yr,mo,dd);

  eqt=AstAlg_equation_of_time(jd);
  return eqt*60.0;
}


double SZAAngle(double lon,double lat,double dec,double Hangle) {
  double Z;
  Z=acos(sin(PI*lat/180.0)*sin(PI*dec/180.0)+
         cos(PI*lat/180.0)*cos(PI*dec/180.0)*cos(PI*Hangle/180.0))*180.0/PI;
  return Z;
}







