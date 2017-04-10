/* remove_noise.c
   ==============
   Author: R.J.Barnes & K.Baker
*/

/*
   See license.txt
*/



#include <math.h>

#include "rmath.h"
#include "badsmp.h"
#include "fitblk.h"
#include "noise_stat.h"

void remove_noise(int mplgs,struct complex *acf,
				  struct complex *ncf) {
  int i;
  double pa, plim;

  plim = lag_power(&ncf[0]);
  pa = lag_power(&acf[0]);

  if( pa > plim ) for (i=0; i < mplgs; i++) {
      acf[i].x -= ncf[i].x;	
	  acf[i].y -= ncf[i].y;
    }
  else for (i=0; i < mplgs; i++) {
    acf[i].x = 0;
	acf[i].y = 0;
  }
  return;
}
