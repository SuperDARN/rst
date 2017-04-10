/* crdshft.c
   =========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "rmath.h"

void CnvMapCrdShft(double *lat,double *lon,double *vazm,double lat_shft,
	           double lon_shft) {
   double aside;
   double bang;
   double cang;
   double bside;
   double dside;
   double signd;
   double arg,ang;
   
   *lon=*lon+lon_shft; 
   if (*lon>360) *lon=*lon-360;
   if (*lon<0) *lon=*lon+360;
   aside=(90.0-*lat)*PI/180.0;
   bang=(180.0-*lon)*PI/180.0;
   dside=lat_shft*PI/180.0;

   arg=cos(aside)*cos(dside)+sin(aside)*sin(dside)*cos(bang);

   if (fabs(arg)>1) arg=arg/fabs(arg); /* normalize */
   
   bside=acos(arg);
   if (bside==0) bside=0.1;

   arg=(cos(aside)-cos(bside)*cos(dside))/(sin(bside)*sin(dside));

   if (fabs(arg)>1) arg=arg/fabs(arg); /* normalize */

   ang=acos(arg);
   if (*lon>180.0) ang=2.0*PI-ang;
   
   *lon=ang*180.0/PI;
   *lat=(PI/2.0-bside)*180.0/PI;

   arg=(cos(dside)-cos(aside)*cos(bside))/(sin(aside)*sin(bside));
   if (fabs(arg)>1) arg=arg/fabs(arg);
   cang=acos(arg);
   signd=dside/fabs(dside);
    
   if (*lon<=180) *vazm=*vazm-signd*cang*180.0/PI;
   else *vazm=*vazm+signd*cang*180.0/PI;

   *lon=*lon-lon_shft;
 
   if (*lon>360) *lon=*lon-360;
   if (*lon<0) *lon=*lon+360;
}









