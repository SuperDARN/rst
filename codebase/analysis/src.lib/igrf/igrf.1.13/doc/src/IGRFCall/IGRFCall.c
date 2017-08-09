/* igrfcall.c
   ========== 
   Author: R.J.Barnes
*/

#include <stdio.h>
#include <stdlib.h>
#include "igrfcall.h"

int main() {
   double dte=1997.0;
   double lat=83.0;
   double lon=110.0;
   double elv=100.0;

   double x,y,z;
 
   fprintf(stdout,"lat=%g lon=%g elv=%g\n",lat,lon,elv);
 
   IGRFCall(dte,lat,lon,elv,&x,&y,&z);

   fprintf(stdout,"x=%g y=%g z=%g\n",x,y,z);
   return 0;
}

 
