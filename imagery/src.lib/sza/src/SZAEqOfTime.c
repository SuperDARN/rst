/* SZAEqOfTime.c
   =============
   Author: R.J.Barnes
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:

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
