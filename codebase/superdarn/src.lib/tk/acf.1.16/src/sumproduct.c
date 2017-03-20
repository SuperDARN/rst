/* sumproduct.c
   =============
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "rtypes.h"



int ACFSumProduct(int16 *buffer,float *avepower,
		   int numsamples,float *mxpwr,float *dco) {
  int sample;
  float sumpower;
  float power;
  float maxpower;
  float ltemp;

  float dcor1=0,dcoi1=0;

  sumpower = 0;
  maxpower = 0;

  if (dco !=NULL) {
    dcor1=dco[0];
    dcoi1=dco[1]; 
  }

  for (sample= 0;sample < numsamples; sample++) {
    ltemp = *buffer-dcor1;
    power = ltemp * ltemp;  /* compute the real */
    buffer++;
    ltemp = *buffer-dcoi1;
    power = power + ltemp * ltemp;    /* compute the imag */
    buffer++;

    /* compute max power */
    if (maxpower < power) maxpower = power;
    sumpower = sumpower + power;
  }

  *mxpwr = maxpower;
  *avepower = sumpower / numsamples;
  return 0;
}


