/* IGRFMagCmp.c
   ==========
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include "magcmp.h"

int main() {

   double dte=1997.0;
   double lat=83.0;
   double lon=110.0;
   double rho=6400.0;

   double x,y,z,m;
 
   fprintf(stdout,"lat=%g lon=%g rho=%g\n",lat,lon,rho);
 
   IGRFMagCmp(dte,rho,lat,lon,&x,&y,&z,&m);

   fprintf(stdout,"x=%g y=%g z=%g m=%g\n",x,y,z,m);

 
   return 0;
}

 
