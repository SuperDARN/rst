/* SZAEqOfTime.c
   =============
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include "sza.h"


int main(int argc,char *argv[]) {

  int yr,mo,dy,hr,mt,sc;
 
  double dec,eqt;

  yr=2002;
  mo=8;
  dy=30;
  hr=10;
  mt=0;
  sc=0;


  dec=SZASolarDec(yr,mo,dy,hr,mt,sc);
  eqt=SZAEqOfTime(yr,mo,dy,hr,mt,sc);



  fprintf(stdout,"SZAEqOfTime\n");
  fprintf(stdout,"Time: %.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",yr,mo,dy,hr,mt,sc);

  fprintf(stdout,"Output: Equation of Time=%g\n",eqt);
  return 0;
}
