/* fft1.c
   ======
   Author: R.J.Barnes
*/


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
