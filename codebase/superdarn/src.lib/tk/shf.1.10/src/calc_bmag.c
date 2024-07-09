/* calc_bmag.c
   ===========
   Author: E.D.P.Cousins and others

Copyright (C) <year>  <name of author>

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
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

double calc_bmag(float mlat, float mlon, float date, int magflg)
{
  double rtp[3], brtp[3], bxyz[3], out[3];
  double bmag;
  double glat, glon, r;

  if (magflg == 2) {
    ecdip2geod((double)mlat,(double)mlon,1.,out);
    glat = out[0];
    glon = out[1];
    r = out[2];
  } else if (magflg == 1) {
    AACGMConvert((double)mlat,(double)mlon,1.,&glat,&glon,&r,1);
  } else {
    AACGM_v2_Convert((double)mlat,(double)mlon,1.,&glat,&glon,&r,1);
  }

  rtp[0] = (Re + Altitude)/Re;        /* unitless */
  rtp[1] = (90.-glat)*PI/180.;
  rtp[2] = glon*PI/180.;
  IGRF_compute(rtp, brtp);            /* compute the IGRF field here */
  bspcar(rtp[1],rtp[2], brtp, bxyz);  /* convert field to Cartesian */
  bmag = sqrt(bxyz[0]*bxyz[0] + bxyz[1]*bxyz[1] + bxyz[2]*bxyz[2]);
  bmag *= 1e-9;   /* SGS: not sure of units here, but this seems to work... */

  return bmag;
}
