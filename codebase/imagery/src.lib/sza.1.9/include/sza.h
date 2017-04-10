/* sza.h
   =====
   Author: R.J.Barnes
*/

/*
   See license.txt
*/



#ifndef _SZA_H
#define _SZA_H

double SZASolarDec(int yr,int mo,int dy,int hr,int mt,int sc);
double SZAEqOfTime(int yr,int mo,int dy,int hr,int mt,int sc);
double SZAAngle(double lon,double lat,double dec,double Hangle);


#endif
