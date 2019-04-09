/* TimeReadClock.c
   ===============
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include "rtime.h"


int main(int argc,char *argv[]) {

  int yr,mo,dy,hr,mt,sc,usc;

  TimeReadClock(&yr,&mo,&dy,&hr,&mt,&sc,&usc);

  fprintf(stdout,"TimeReadClock\n");
  fprintf(stdout,"Time: %.4d-%.2d-%.2d %.2d:%.2d:%.2d:%.2d\n",
          yr,mo,dy,hr,mt,sc,usc);

  return 0;
}
