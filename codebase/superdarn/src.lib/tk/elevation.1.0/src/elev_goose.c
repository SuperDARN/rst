/* elev_goose.c
   ============
   Author: R.J.Barnes & K.Baker

 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
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
  E.G.Thomas 2021-08: added support for bmoff parameter
*/ 



#include <math.h>
#include <stdio.h>

#include "rmath.h"
#include "fitblk.h"

double elev_goose(struct FitPrm *prm, double phi0) {
  int n;
  int cnt=0;
  double k, psi, dchi_cable, temp, sn2_alpha_min, cs_alpha_min;
  double sin_psi2, sin_psi_xi, sin_psi2_xi2, sn2_alpha_old;
  double dchi_sep_max, dchi_max, dchi, dchi_old, cs_alpha, sn2_eps=0, sn_eps;
  double elev;
  double offset=7.5;
  static double xi=0.0, sep= 0.0, elev_corr=0.0, cos_xi=0.0,
                 sin_xi=0.0, cos_xi2=0.0, sin_xi2;

  /* calculate the values that don't change if this hasn't already been done.

  The coordinate system is: +x is in front of the main array,
  +y is to the left of the center, and +z is upward.

  The angle xi is in the x-y plane.  The angle is measured counter-clockwise
  from the x axis.  If the y offset and x offset are both positive, the
  angle is positive.

  Sep is the distance between the centers of the two arrays

  If the z offset is not zero, then the elevation angle has to be corrected.
  An apparent elevation angle of 0 would actually being pointing slightly
  upward (downward) if the z offset is positive (negative).

  */

  if (sep == 0.0) {
    xi = atan2(prm->interfer[0],(prm->interfer[1]));
    sep = sqrt(prm->interfer[0]*prm->interfer[0] + 
               prm->interfer[1]*prm->interfer[1] +
               prm->interfer[2]*prm->interfer[2]);

    elev_corr = atan2(prm->interfer[2], 
                      sqrt(prm->interfer[0]*prm->interfer[0] +
                      prm->interfer[1]*prm->interfer[1]));
    cos_xi = cos(xi);
    sin_xi = sin(xi);

    cos_xi2 = cos(xi)*cos(xi);
    sin_xi2 = sin(xi)*sin(xi);
  }

  /* compute phasing matrix cone angle */
  offset=prm->maxbeam/2.0-0.5;
  psi = (prm->bmoff+prm->bmsep*(prm->bmnum-offset))*PI/180.;

  /* compute wavenumber */
  k = 2 * PI * prm->tfreq * 1000.0/C;

  dchi_cable = - 2* PI * prm->tfreq * 1000.0 * prm->tdiff * 1.0e-6;

  /* compute the minimum cone angle (alpha) */

  temp = sin(psi) + sin_xi;
  sin_psi_xi = sin(psi)*sin_xi;

  sn2_alpha_min = (temp*temp)/(1.0 + sin_psi_xi);
  cs_alpha_min = sqrt(1.0 - sn2_alpha_min);

  /* now iterate sn2_alpha_min, cs_alpha_min to improve value of alpha_min */

  sin_psi2 = sin(psi)*sin(psi);
  sin_psi2_xi2 = sin_psi2 + sin_xi2;

  sn2_alpha_old = sn2_alpha_min;

  while( fabs((sn2_alpha_min = sin_psi2_xi2 + 2.0*cs_alpha_min*sin_psi_xi)
                - sn2_alpha_old) > 0.005*sn2_alpha_old) {
    cs_alpha_min = sqrt(1.0 - sn2_alpha_min);
    sn2_alpha_old = sn2_alpha_min;
  }
  cs_alpha_min = sqrt(1.0 - sn2_alpha_min);

  /* we've now got the sin & cos of alpha_min */
  /*  compute the total phase difference */

  dchi_sep_max = k * sep / cos_xi * cs_alpha_min;
  dchi_max = dchi_cable + dchi_sep_max;
  n = 0.5 - dchi_max/(2*PI);

  dchi = phi0 - n*(2*PI);   /* this should be the true phase difference */

  if (dchi > dchi_max) dchi = dchi - (2*PI);
  if (dchi < (dchi_max - (2*PI))) dchi = dchi + (2*PI);

  /* compute the cone angle (alpha) */

  dchi_old = 0.0;
  while (fabs(dchi_old - dchi) > PI && cnt < 10) {
    cs_alpha = (dchi - dchi_cable)/(k*sep)*cos_xi;
    sn2_eps = 1.0 - (cs_alpha*cs_alpha)/(cos_xi2) - (sin_psi2/cos_xi2)
             - 2.0*cs_alpha*sin_psi_xi/cos_xi2;
    dchi_old = dchi;

    if ((fabs(sn2_eps) > 1.0) || (sn2_eps < 0.0)) {
      dchi = dchi - (2*PI);
      fprintf(stderr,"changing dchi by -2pi. %f -> %f\n",dchi_old,dchi);
    }

    /* counter to make sure this loop doesn't run to infinity - EGT 20190618 */
    cnt++;
  }

  sn_eps = sqrt(sn2_eps);
  elev = asin(sn_eps);

  /* The calculated elevation angle is actually with respect to the plane
   that includes the two antenna arrays.  This has to be corrected for the
   difference in elevation between the front array and the back array.
  */

  elev = elev + elev_corr;
  return 180.0*elev/PI;

}

