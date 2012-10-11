/* TimeYrsecToYMDHMS.c
   ===================
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include "rtime.h"


int main(int argc,char *argv[]) {

  int yr,mo,dy,hr,mt,sc;

  int tval=18000;
  yr=2002;

  TimeYrsecToYMDHMS(tval,yr,&mo,&dy,&hr,&mt,&sc);

  fprintf(stdout,"TimeYrsecToYMDHMS\n");
  fprintf(stdout,"Tval: %d\n",tval);
  fprintf(stdout,"Time: %.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
          yr,mo,dy,hr,mt,sc);

  return 0;
}
