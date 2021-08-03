/* ConvertBlock.c
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



#include <stdio.h>
#include <stdlib.h>
#include "rtypes.h"
#include "rconvert.h"

int main(int argc,char *argv[]) {
  int i;
  
  
  struct {
    double x,y,z;
    int16 flg;
    int32 data[4];
    float angle;
  } dat;
  
  int pattern[]={8,3,2,1,4,4,4,1,0,0};
  
  unsigned char *str;
  
  dat.x=0.5;
  dat.y=0.2;
  dat.z=0.3;
  dat.flg=1023;
  dat.data[0]=18000;
  dat.data[1]=32000;
  dat.data[2]=64000;
  dat.data[3]=12000;
  dat.angle=45.5;
   
  str=(unsigned char *) &dat;
 
 
  ConvertBlock(str,pattern);
  fprintf(stdout,"buf=");
  for (i=0;i<sizeof(dat);i++) fprintf(stdout,"%.2x",str[i]);
  fprintf(stdout,"\n");
  
  return 0;
}
   









