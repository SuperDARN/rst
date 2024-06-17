/* elevation_v2.c
   ==============
   Author: S.G. Shepherd


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
  E.G.Thomas 2021-08: added support for bmoff parameter
*/

#include <math.h>
#include <stdio.h>
#include "rmath.h"
#include "fitblk.h"

/* SGS somehow need to pass in options for allowing:                         *
 *     negative elevation angles and residual phase                          */
double elevation_v2(struct FitPrm *prm, double psi_obs)
{
  static double X,Y,Z;      /* interferometer offsets [m]                    */
  double boff;              /* offset in beam widths to edge of FOV          */
  double phi0;              /* beam direction off boresight [rad]            */
  double cp0,sp0;           /* cosine and sine of phi0                       */
  double ca0,sa0;           /* cosine and sine of a0                         */
  double psi_ele;           /* phase delay due to electrical path diff [rad] */
  double psi_max;           /* maximum phase difference [rad]                */
  double a0;                /* angle where phase is maximum [rad]            */
  int    sgn;               /* sign of Y offset                              */
  double dpsi;              /* delta phase [rad]                             */
  double n2pi;              /* # of 2PI factors for correct mapping          */
  double d2pi;              /* correct multiple value of 2PIs                */
  double E;                 /* factor for simplifying expression             */
  double alpha;             /* elevation angle [degrees]                     */

  static double d = -9999.; /* separation of antenna arrays [m]              */

  /* calculate the values that don't change if this hasn't already been done. */

  if (d < -999.) {  /* SGS check this; does this mean a change won't register */
    X   = prm->interfer[0];
    Y   = prm->interfer[1];
    Z   = prm->interfer[2];

    d   = sqrt(X*X + Y*Y + Z*Z);
  }

  sgn = (Y < 0) ? -1 : 1;

  boff   = prm->maxbeam/2. - 0.5;
  phi0   = (prm->bmoff + prm->bmsep*(prm->bmnum - boff))* PI/ 180.;
  cp0    = cos(phi0);
  sp0    = sin(phi0);

  /*k      = 2 * PI * prm->tfreq * 1000./C;*/

  /* Phase delay [radians] due to electrical path difference.                *
   *   If the path length (cable and electronics) to the interferometer is   *
   *   shorter than that to the main antenna array, then the time for the    *
   *   to transit the interferometer electrical path is shorter: tdiff < 0   */
  psi_ele = -2 * PI * prm->tfreq * prm->tdiff * 1e-3;

  /* Determine elevation angle (a0) where psi (phase difference) is maximum; *
   *   which occurs when k and d are anti-parallel. Using calculus of        *
   *   variations to compute the value: d(psi)/d(a) = 0                      */
  a0    = asin(sgn*Z*cp0/sqrt(Y*Y + Z*Z));

  /* Note: we are assuming that negative elevation angles are unphysical.    *
   *   The act of setting a0 = 0 _only_ has the effect to change psi_max     *
   *   (which is used to compute the correct number of 2pi factors and map   *
   *   the observed phase to the actual phase. The _only_ elevation angles   *
   *   that are affected are the small range from [-a0, 0]. Instead of these *
   *   being mapped to negative elevation they are mapped to very small      *
   *   range just below the maximum.                                         */

  /* Note that it is possible in some cases with sloping ground that extends *
   *   far in front of the radar, that negative elevation angles might exist.*
   *   However, since elevation angles near the maximum "share" this phase   *
   *   [-pi,pi] it is perhaps more likely that the higher elevation angles   *
   *   are actually what is being observed.                                  */

  /* In either case, one must decide which angle to chose (just as with all  *
   *   the aliased angles). Here we decide (unless the keyword 'negative' is *
   *   set) that negative elevation angles are unphysical and map them to    *
   *   the upper end.                                                        */

  if (a0 < 0) a0 = 0.;  /* SGS and ~keyword_set(negative) */

  ca0     = cos(a0);
  sa0     = sin(a0);
  /* maximum phase = psi_ele + psi_geo(a0)                                   */
  psi_max = psi_ele + 2 * PI * prm->tfreq * 1e3/C *
                     (X*sp0 + Y*sqrt(ca0*ca0 - sp0*sp0) + Z*sa0);

  /* compute the number of 2pi factors necessary to map to correct region    */
  dpsi = psi_max - psi_obs;   /* psi_obs is observed phase difference        */
  n2pi = (Y > 0) ? floor(dpsi/(2*PI)) : ceil(dpsi/(2*PI));
  d2pi = n2pi * 2*PI;

  /* map observed phase to correct extended phase                           */
  psi_obs += d2pi;
  /* SGS: if not keyword_set(actual) then psi_obs += d2pi                   */

  /* now solve for the elevation angle: alpha                               */
  E = (psi_obs/(2*PI*prm->tfreq*1e3) + prm->tdiff*1e-6)*C - X*sp0;
  alpha = asin((E*Z + sqrt(E*E * Z*Z - (Y*Y + Z*Z)*(E*E - Y*Y*cp0*cp0)))/
               (Y*Y + Z*Z));

  return (180.*alpha/PI);
}
