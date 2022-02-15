/* RPosCalcVector
   ==============
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

#include "calcvector.h"


int main(int argc,char *argv[]) {
 
  double alat=58.0;
  double alon=140.0;
  double mag=1500.0;
  double azm=18.0;

  double blat,blon;

  RPosCalcVector(alat,alon,mag,azm,&blat,&blon);


  fprintf(stdout,"RPosCalcVector\n");
  fprintf(stdout,"alat=%g, alon=%g, mag=%g, azm=%g\n",alat,alon,mag,azm);
  fprintf(stdout,"blat=%g blon=%g\n",blat,blon);

  return 0;
}
