/* TimeEpochToYMDHMS.c
   ===================
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include "rtime.h"


int main(int argc,char *argv[]) {

  int yr,mo,dy,hr,mt;
  double sc;

  double tval=1030701600;

  TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sc);

  fprintf(stdout,"TimeEpochToYMDHMS\n");
  fprintf(stdout,"Tval: %d\n",(int) tval);
  fprintf(stdout,"Time: %.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
          yr,mo,dy,hr,mt, (int)sc);

  return 0;
}
