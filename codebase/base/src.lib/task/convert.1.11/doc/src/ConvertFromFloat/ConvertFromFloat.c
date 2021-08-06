/* ConvertFromFloat.c
   ==================
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
#include "rtypes.h"
#include "rconvert.h"

int main(int argc,char *argv[]) {
  int i;
  float val;
  unsigned char buf[4];
  
  val=0.3;
  ConvertFromFloat(val,buf);
  
  fprintf(stdout,"val=%g\n",val);
  fprintf(stdout,"buf=");
  for (i=0;i<4;i++) fprintf(stdout,"%.2x",buf[i]);
  fprintf(stdout,"\n");
  return 0;
}
   

