/* rmath.h
   =======
   Author: R.J.Barnes
*/


/*
   See license.txt
*/




#ifndef _RMATH_H
#define _RMATH_H

#ifndef _COMPLEX
struct complex {
    double x,y;
};
#define cabs(a) sqrt((a.x*a.x)+(a.y*a.y))
#endif

#define C 2.997e+8
#define LN_TO_LOG 4.342944819

#ifndef HUGE_VAL
#define HUGE_VAL 1.0e36                /* default value of HUGE */
#endif
 
#ifndef PI
#define PI 3.14159265358979323846
#endif

#define asind(x) asin(x)*180/PI
#define acosd(x) acos(x)*180/PI
#define cosd(x) cos(PI*(x)/180.0)
#define sind(x) sin(PI*(x)/180.0)
#define tand(x) tan(PI*(x)/180.0)
#define atand(x) atan(x)*180.0/PI
#define atan2d(x,y) atan2(x,y)*180.0/PI
#endif













