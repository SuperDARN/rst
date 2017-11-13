/* calc_bmag.c
   ===========
   Author: E.D.P.Cousins and others
*/

/*
   See license.txt
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "rmath.h"
#include "aacgm.h"
#include "mlt.h"
#include "aacgmlib_v2.h"
#include "mlt_v2.h"
#include "shfconst.h"
#include "igrflib.h"

double calc_bmag(float mlat, float mlon, float date, int old_aacgm)
{
  double rtp[3], brtp[3], bxyz[3];
  double bmag;
  double glat, glon, r;

  if (old_aacgm) AACGMConvert((double)mlat,(double)mlon,1.,&glat,&glon,&r,1);
  else       AACGM_v2_Convert((double)mlat,(double)mlon,1.,&glat,&glon,&r,1);

  rtp[0] = (Re + Altitude)/Re;        /* unitless */
  rtp[1] = (90.-glat)*PI/180.;
  rtp[2] = glon*PI/180.;
  IGRF_compute(rtp, brtp);            /* compute the IGRF field here */
  bspcar(rtp[1],rtp[2], brtp, bxyz);  /* convert field to Cartesian */
  bmag = sqrt(bxyz[0]*bxyz[0] + bxyz[1]*bxyz[1] + bxyz[2]*bxyz[2]);
  bmag *= 1e-9;   /* SGS: not sure of units here, but this seems to work... */

  return bmag;
}
