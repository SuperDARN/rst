/* sumproduct.c
   =============
   Author: R.J.Barnes
*/

/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
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


