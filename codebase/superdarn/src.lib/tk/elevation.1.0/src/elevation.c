/* elevation.c
   ===========
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

double elevation(struct FitPrm *prm, double phi0) {
 
  double k;          /* wave number; 1/m */
  double phi;        /* beam direction off boresight; rad */
  double c_phi;      /* cosine of phi                     */
  double dchi_cable; /* phase shift caused by cables; rad */
  double chi_max;    /* maximum phase shift possible; rad */
  double phi_temp;   /* actual phase angle + cable;   rad */
  double psi;        /* actual phase angle - cable;   rad */
  double theta;      /* angle of arrival for horizontal antennas; rad */
  double offset=7.5; /* offset in beam widths to the edge of the array */
  static double antenna_separation= 0.0; /* m */
  static double elev_corr= 0.0;
  /* elevation angle correction, if antennas are at different heights; rad */
  static double phi_sign= 0;
  /* +1 if interferometer antenna is in front of main antenna, -1 otherwise*/

  /* calculate the values that don't change if this hasn't already been done. */

  if (antenna_separation == 0.0) {
    antenna_separation= sqrt(prm->interfer[1]*prm->interfer[1] + 
			                 prm->interfer[0]*prm->interfer[0] +
	                         prm->interfer[2]*prm->interfer[2]);
    elev_corr= prm->phidiff* asin( prm->interfer[2]/ antenna_separation);
    if (prm->interfer[1] > 0.0) /* interferometer in front of main antenna */
      phi_sign= 1.0;
    else {                           /* interferometer behind main antenna */
      phi_sign= -1.0;
      elev_corr= -elev_corr;
    }
  }
  offset=prm->maxbeam/2.0-0.5;
  phi= (prm->bmoff + prm->bmsep*(prm->bmnum - offset))* PI/ 180.0;
  c_phi= cos( phi);
  k= 2 * PI * prm->tfreq * 1000.0/C;

  /* the phase difference phi0 is between -pi and +pi and gets positive,  */
  /* if the signal from the interferometer antenna arrives earlier at the */
  /* receiver than the signal from the main antenna. */
  /* If the cable to the interferometer is shorter than the one to */
  /* the main antenna, than the signal from the interferometer     */
  /* antenna arrives earlier. tdiff < 0  --> dchi_cable > 0        */

  dchi_cable= - 2* PI * prm->tfreq * 1000.0 * prm->tdiff * 1.0e-6;

  /* If the interferometer antenna is in front of the main antenna */
  /* then lower elevation angles correspond to earlier arrival     */
  /* and greater phase difference. */    
  /* If the interferometer antenna is behind of the main antenna   */
  /* then lower elevation angles correspond to later arrival       */
  /* and smaller phase difference */    

  chi_max= phi_sign* k* antenna_separation* c_phi + dchi_cable;

  /* change phi0 by multiples of twopi, until it is in the range   */
  /* (chi_max - twopi) to chi_max (interferometer in front)        */
  /* or chi_max to (chi_max + twopi) (interferometer in the back)  */

  phi_temp= phi0 + 2*PI* floor( (chi_max - phi0)/ (2*PI));
  if (phi_sign < 0.0) phi_temp= phi_temp + (2*PI);

  /* subtract the cable effect */
  psi= phi_temp - dchi_cable;
  theta= psi/ (k* antenna_separation);
  theta= (c_phi* c_phi - theta* theta);
  /* set elevation angle to 0 for out of range values */

  if ( (theta < 0.0) || (fabs( theta) > 1.0) ) theta= - elev_corr;
  else theta= asin( sqrt( theta));

  return 180.0* (theta + elev_corr)/ PI; /* in degree */
}

