/* sza.c
   =====
   Author: R.J.Barnes
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
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







