/* Eval.c
   ====== 
   Author: R.J.Barnes
*/

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

