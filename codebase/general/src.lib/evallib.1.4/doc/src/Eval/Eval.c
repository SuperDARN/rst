/* Eval.c
   ====== 
   Author: R.J.Barnes
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "reval.h"

char *sum="cos(2.5)*3.2+x/2.0";
double x=1.0;

int decode_value(char *ptr,double *value,void *data) {
   if ((isdigit(ptr[0])) || (ptr[0]=='.')) {
     *value=atof(ptr);
     return 0;
   }

   if (strcmp(ptr,"x")==0) *value=x;
   else return -1;
   return 0;
}

int decode_function(char *ptr,int argnum,double *argptr,
                    double *value,void *data) {
  if (strcmp(ptr,"cos")==0) {
    *value=cos(argptr[0]);
  }
  return 0;
}

int main(int argc,char *argv[]) {
  int s;
  double value;

  s=Eval(sum,&value,decode_value,NULL,decode_function,NULL);
  if (s == -1)
  {
      fprintf(stderr, "Warning: Eval returned a negative, may be an error\n");
  }
  fprintf(stdout,"%s=%g\n",sum,value);

  return 0;

}

