/* rang_badlags.c
   ==============
   Author: R.J.Barnes & K.Baker & P.Ponomarenko
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
#include <math.h>

#include "fitblk.h"
#include "badlags.h"

/* #define MIN_PWR_RATIO	.3 */

#define PULSE_SIZE 256

static int range_overlap[PULSE_SIZE][PULSE_SIZE];

/*  r_overlap sets up the table r_overlap which keeps track of the
 *  ranges which might cause interference.
 */ 

void r_overlap(struct FitPrm *fitted_prms) {
  int pulse_to_check;
  int pulse;
  int tau;

  int diff_pulse;

  /* define constants */
  /* Found a few cases where smsep isn't written or is zero.  So lets use txpl
     in its place */
  if (fitted_prms->smsep != 0) {
     tau = fitted_prms->mpinc / fitted_prms->smsep;
  } else {
     fprintf( stderr, "r_overlap: WARNING, using txpl instead of smsep...\n");
     tau = fitted_prms->mpinc / fitted_prms->txpl;
  }

  for (pulse_to_check = 0; pulse_to_check < fitted_prms->mppul; ++pulse_to_check) {
    for (pulse = 0; pulse < fitted_prms->mppul; ++pulse) {
      diff_pulse = fitted_prms->pulse[pulse_to_check] - 
                      fitted_prms->pulse[pulse];
      range_overlap[pulse_to_check][pulse] = diff_pulse * tau;
    }
  }
  return;
} 


/* lag_overlap marks the badlag array for bad lags */

void lag_overlap(int range,int *lag,struct FitPrm *fitted_prms) {
     
  int pulse_to_check;
  int pulse;
  int lag_number;
  int range_to_check;
  long min_pwr;
  long pwr_ratio;
  int bad_pulse[PULSE_SIZE];  /* 1 if there is a bad pulse */
  int i;
  double nave;
  double total_cri;
  --range;  /* compensate for the index which starts from 0 instead of 1 */

  nave = (double) (fitted_prms->nave);

  for (pulse = 0; pulse < fitted_prms->mppul; ++pulse)
      bad_pulse[pulse] = 0;

  for (pulse_to_check = 0;  pulse_to_check < fitted_prms->mppul; ++pulse_to_check) {
    total_cri = 0.0;
    for (pulse = 0; pulse < fitted_prms->mppul; ++pulse) {
      range_to_check = range_overlap[pulse_to_check][pulse] + range;

      if ((pulse != pulse_to_check) &&
          (0 <= range_to_check) && 
	        (range_to_check < fitted_prms->nrang)) {
        total_cri += fitted_prms->pwr0[range_to_check];
        pwr_ratio = (long) 1;  /*pwr_ratio = (long) (nave * MIN_PWR_RATIO);*/
        min_pwr =  pwr_ratio * fitted_prms->pwr0[range];

        if(min_pwr < total_cri){
          bad_pulse[pulse_to_check] = BAD;
        }
      }
    } 
  }           
  
  /* mark the bad lag */

  for (pulse = 0 ; pulse < fitted_prms->mppul; ++pulse) {
    if (bad_pulse[pulse] == 1) {
      for (i=0; i < 2 ; ++i) {
        for (lag_number = 0 ; lag_number < fitted_prms->mplgs ; ++lag_number) {
          if (fitted_prms->lag[i][lag_number] == fitted_prms->pulse[pulse])
            lag[lag_number] = BAD;  /* 1 for bad lag */
        }
      }
    }
  } 
  return;
} 

