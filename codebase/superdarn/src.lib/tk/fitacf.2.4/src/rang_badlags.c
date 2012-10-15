/* rang_badlags.c
   ==============
   Author: R.J.Barnes & K.Baker & P.Ponomarenko
*/

/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.2-beta-4-g32f7302.txt for more information.
 
 
  
*/



#include <stdio.h>
#include <math.h>

#include "fitblk.h"

/* #define MIN_PWR_RATIO	.3 */

#define PULSE_SIZE 256

static int range_overlap[PULSE_SIZE][PULSE_SIZE];

/*  r_overlap sets up the table r_overlap which keeps track of the
 *  ranges which might cause interference.
 */ 

void r_overlap(struct FitPrm *ptr) {
  int ck_pulse;
  int pulse;
  int tau;

  int diff_pulse;

  /* define constants */
  tau = ptr->mpinc / ptr->smsep;
 
  for (ck_pulse = 0; ck_pulse < ptr->mppul; ++ck_pulse) {
    for (pulse = 0; pulse < ptr->mppul; ++pulse) {
      diff_pulse = ptr->pulse[ck_pulse] - 
                      ptr->pulse[pulse];
      range_overlap[ck_pulse][pulse] = diff_pulse * tau;
    }
  }
  return;
} 


/* lag_overlap marks the badlag array for bad lags */

void lag_overlap(int range,int *badlag,struct FitPrm *ptr) {
     
  int ck_pulse;
  int pulse;
  int lag;
  int ck_range;
  long min_pwr;
  long pwr_ratio;
  int bad_pulse[PULSE_SIZE];  /* 1 if there is a bad pulse */
  int i;
  double nave;
   
  --range;  /* compensate for the index which starts from 0 instead of 1 */

  nave = (double) (ptr->nave);

  for (pulse = 0; pulse < ptr->mppul; ++pulse)
      bad_pulse[pulse] = 0;

  for (ck_pulse = 0;  ck_pulse < ptr->mppul; ++ck_pulse)
	{
    for (pulse = 0; pulse < ptr->mppul; ++pulse)
		{
      ck_range = range_overlap[ck_pulse][pulse] + range;
      if ((pulse != ck_pulse) && (0 <= ck_range) && 
	      (ck_range < ptr->nrang))
			{
        pwr_ratio = (long) 1;  /*pwr_ratio = (long) (nave * MIN_PWR_RATIO);*/
        min_pwr =  pwr_ratio * ptr->pwr0[range];
        if(min_pwr < ptr->pwr0[ck_range])
					bad_pulse[ck_pulse] = 1;
      }
    }
  }

  /* mark the bad lag */

  for(pulse = 0 ; pulse < ptr->mppul; ++pulse)
	{
    if(bad_pulse[pulse] == 1)
		{
      for(i=0; i < 2 ; ++i)
			{
        for(lag = 0 ; lag < ptr->mplgs ; ++lag)
				{
          if((ptr->lag[i][lag] == ptr->pulse[pulse]) && badlag[lag] == 0)
            badlag[lag] = 11;  /* 1 for bad lag */
        }
      }
    }
  } 
  return;
} 

