/* TimeJulianToYMDHMS.c
   ===================
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include "rtime.h"


int main(int argc,char *argv[]) {

  int yr,mo,dy,hr,mt;
  double sc;

  double tval=2452516;

  TimeJulianToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sc);

  fprintf(stdout,"TimeJulianToYMDHMS\n");
  fprintf(stdout,"Tval: %d\n",(int) tval);
  fprintf(stdout,"Time: %.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
          yr,mo,dy,hr,mt, (int)sc);

  return 0;
}
