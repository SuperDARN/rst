/* calc_bmag.c
   ============
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "shfconst.h"
#include "igrfcall.h"
#include "aacgm.h"

double calc_bmag(float mlat, float mlon, float date) {
  double bmag;
  double glat, glon, r;
  double x,y,z;

  
  AACGMConvert((double)mlat,(double)mlon,1.,&glat,&glon,&r,1);
  IGRFCall(date,glat,glon,Altitude/1000.,&x,&y,&z);
  bmag = 1e-9*sqrt(x*x + y*y + z*z);

  return bmag;
}
