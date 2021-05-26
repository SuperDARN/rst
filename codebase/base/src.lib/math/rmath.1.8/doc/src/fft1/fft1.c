/* fft1.c
   ======
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




#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rmath.h"
#include "fft.h"

float data[512];

int main(int argc,char *argv[]) {

  int n;

  for (n=0;n<256;n++) data[n]=cos(8*PI*n/256.0);

  fft1(data,256,1); 
  for (n=0;n<256;n++) fprintf(stdout,"%d:%g\n",n,data[n]);

  return 0;
 
}
