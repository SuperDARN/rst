/* location.c
   ===========
   Author: S.G. Shepherd, Angeline G. Burrell - NRL - 2021
*/

/*
 LICENSE AND DISCLAIMER

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

/* TODO: add to rst/codebase/superdarn/src.lib/tk/elevation.1.0 */

#include <math.h>
#include <stdio.h>

#include "radar.h"
#include "rmath.h"

/**
 * @brief Calculate elevation for the specified field of view
 *
 * @param[in] lobe    - Field-of-view specifier: 1=front, -1=back
 *            bmnum   - Beam number (zero offset)
 *            tfreq   - Transmission frequency in kHz
 *            site    - Radar hardware site structure
 *            psi_obs - Phase lag value
 *
 * @param[out] alpha - Elevation angle [deg]
 *
 * @notes SGS somehow need to pass in options for allowing negative elevation
 *        angles and residual phase
 **/

double elevation_v2_lobe(int lobe, int bmnum, int tfreq, struct RadarSite *site,
			 double psi_obs)
{
  static double X, Y, Z; /* interferometer offsets [m]                       */
  double k;              /* wavenumber [rad/m]                               */
  double boff;           /* offset in beam widths to edge of FOV             */
  double phi0_max;       /* beam direction off boresight [rad]               */
  double cp0, sp0;       /* cosine and sine of phi0                          */
  double ca0, sa0;       /* cosine and sine of a0                            */
  double psi_ele;        /* phase delay due to electrical path diff [rad]    */
  double psi_max;        /* max and min phase difference [rad]               */
  double a0;             /* angle where phase is maximum [rad]               */
  int    sgn;            /* sign of Y offset                                 */
  double dpsi;           /* delta phase [rad]                                */
  double n2pi;           /* number of 2PI factors                            */
  double d2pi;           /* correct multiple value of 2PIs                   */
  double E;              /* factor for simplifying expression                */
  double alpha;          /* elevation angle [degrees]                        */

  static double d = -9999.0; /* separation of antenna arrays [m]             */

  /* At first call, calculate the values that don't change. */
  if (d < -999.0)
    {
      X = site->interfer[0];
      Y = site->interfer[1];
      Z = site->interfer[2];

      d = sqrt(X*X + Y*Y + Z*Z);
  }

  /* SGS: 20180926
   *
   * There is still some question as to exactly what the phidiff parameter in
   * the hdw.dat files means. The note in the hdw.dat files, presumably written
   * by Ray is:
   * 12) Phase sign (Cabling errors can lead to a 180 degree shift of the
   *     interferometry phase measurement. +1 indicates that the sign is
   *     correct, -1 indicates that it must be flipped.)
   * The _only_ hdw.dat file that has this value set to -1 is GBR during the
   * time period: 19870508 - 19921203
   *
   * To my knowlege there is no data available prior to 1993, so dealing with
   * this parameter is no longer necessary. For this reason I am simply
   * removing it from this algorithm.
   */

  sgn = (Y < 0) ? -1 : 1;

  boff     = (double)site->maxbeam / 2.0 - 0.5;
  phi0_max = site->bmsep * ((double)bmnum - boff) * PI / 180.0;

  cp0 = cos(phi0_max);
  sp0 = sin(phi0_max);

  /* Determine the elevation angle (a0) where the phase difference (psi) is *
   * at its maximum.  This occurs when k and d are anti-parallel. Using     *
   * calculus of variations to compute the value: d(psi)/d(a) = 0           */
  a0 = asin(sgn * Z * cp0 / sqrt(Y*Y + Z*Z));

  /* Note: We are assuming that negative elevation angles are unphysical.   *
   *   The act of setting a0 = 0 _only_ has the effect to change psi_max    *
   *   (which is used to compute the correct number of 2pi factors and map  *
   *   the observed phase to the actual phase. The _only_ elevation angles  *
   *   that are affected are the small range from [-a0, 0]. Instead of      *
   *   these being mapped to negative elevation they are mapped to very     *
   *   small ranges just below the maximum.                                 */

  /* Note that it is possible in some cases with sloping ground that        *
   * extends far in front of the radar, that negative elevation angles      *
   * might exist. However, since elevation angles near the maximum "share"  *
   * this phase [-pi, pi] it is perhaps more likely that the higher         *
   * elevation angles are actually what is being observed.                  */

  /* In either case, one must decide which angle to chose (just as with all *
   * the aliased angles). Here we decide that negative elevation angles are */
  /* unphysical and map them to the upper end.                              */

  if (a0 < 0.0) a0 = 0.0;

  ca0 = cos(a0);
  sa0 = sin(a0);
  k   = 2.0 * PI * tfreq * 1000.0 / C;

  /* Phase delay [radians] due to electrical path difference.               *
   *   If the path length (cable and electronics) to the interferometer is  *
   *   shorter than that to the main antenna array, then the time for the   *
   *   to transit the interferometer electrical path is shorter: tdiff < 0  */
  psi_ele = -2.0 * PI * tfreq * site->tdiff * 1.0e-3;

  /* maximum phase = psi_ele + psi_geo(a0)                                  */
  psi_max = psi_ele + k * (X * sp0 + Y * sqrt(ca0 * ca0 - sp0 * sp0) + Z * sa0);

  /* Compute the number of 2pi factors necessary to map to correct region.  *
   * The lobe direction changes the sign of the observed phase difference,  *
   * phi_obs. (AGB)                                                         */
  psi_obs *= (double)lobe;
  dpsi = psi_max - psi_obs;
  n2pi = (Y > 0) ? floor(dpsi / (2.0 * PI)) : ceil(dpsi / (2.0 * PI));
  d2pi = n2pi * 2.0 * PI;
  psi_obs += d2pi;

  /* Evaluate the phase shift and solve for the elevation angle, alpha      */
  E = (psi_obs / (k * C) + site->tdiff * 1.0e-6) * C -  X * sp0;
  alpha = asin((E * Z + sqrt(E * E * Z * Z - (Y * Y + Z * Z)
			     * (E * E - Y * Y * cp0 * cp0)))
	       / (Y * Y + Z * Z));
  alpha *= 180.0 / PI; /* Convert from radians to degrees */

  return(alpha);
}


/**
 * @brief Calculate the virtual height using hop number and elevation angle
 *
 * @param[in] slant_dist - slant distance in km
 *            hop        - number of hops (0.5, 1.0, 1.5, etc)
 *            radius     - radius of the Earth in km
 *            elv        - elevation angle in degrees
 *
 * @param[out] vheight - virtual height above the surface of the Earth in km
 */

double calc_elv_vheight(double slant_dist, double hop, double radius,
			double elv)
{
  double vheight;

  /* Adjust the slant distance to find the length up to the ionosphere *
   * This assumes the propagation path can be evenly segmented, which  *
   * is mildly not correct, but very much not correct in some cases.   */
  slant_dist /= (2.0 * hop);

  vheight = sqrt(slant_dist * slant_dist + radius * radius
  		 + 2.0 * slant_dist * radius * sin(elv * PI / 180.0)) - radius;

  return(vheight);
}

/**
 * @brief Test the propagation path for realism using the basic properties of HF
 *         radars.
 *
 * @param[in] hop        - number of hops (0.5, 1.0, 1.5, etc)
 *            vheight    - virtual height in km
 *            slant_dist - slant distance in km
 *            D_hmin     - minimum height for D region in km
 *            D_hmax     - maximum height for D region in km
 *            E_hmax     - maximum height for E region in km
 *
 * @param[out] good_path - 1 if good, 0 if bad
 **/

int TestPropagation(float hop, float vheight, float slant_dist, float D_hmin,
		    float D_hmax, float E_hmax)
{
  int good_path=1;

  if(vheight < D_hmin) good_path = 0;
  else if(vheight <= D_hmax)
    {
      if(hop > 0.5 || slant_dist > 500.0) good_path = 0;
    }
  else if(vheight <= E_hmax)
    {
      if(hop < 1.5 && slant_dist > 900.0) good_path = 0;
    }

  return(good_path);
}


/**
 * @brief Test the propagation path for realism using the basic properties of HF
 *         radars.
 *
 * @param[in] lobe       - Field-of-view specifier: 1=front, -1=back
 *            radius     - Earth radius in km
 *            D_hmin     - Minimum height for D region in km
 *            D_hmax     - Maximum height for D region in km
 *            E_hmax     - Maximum height for E region in km
 *            F_hmax     - Maximum height for F region in km
 *            max_hop    - Maximum number of realistic hops allowed
 *            bmnum      - Beam number (zero offset)
 *            tfreq      - Transmission frequency in kHz
 *            site       - Radar hardware site structure
 *            psi_obs    - Phase lag value
 *            hop        - Number of hops (0.5, 1.0, 1.5, etc)
 *            vheight    - Virtual height in km
 *            elv        - Elevation angle in degrees
 *            slant_dist - Slant distance of entire path in km
 **/

void AdjustPropagation(int lobe, float radius, float D_hmin, float D_hmax,
		       float E_hmax, float F_hmax, float max_hop,
		       int bmnum, int tfreq, struct RadarSite *site,
		       double psi_obs, float *hop, float *vheight,
		       double *elv, float *slant_dist)
{
  int good_hop;

  float new_hop;

  double elevation_v2_lobe(int lobe, int bmnum, int tfreq,
			   struct RadarSite *site, double psi_obs);
  int TestPropagation(float hop, float vheight, float slant_dist, float D_hmin,
		      float D_hmax, float E_hmax);

  /* If the virtual height is realistic, assign the current propagation path. */
  new_hop  = *hop;
  good_hop = TestPropagation(*hop, *vheight, *slant_dist, D_hmin, D_hmax,
			     E_hmax);

  /* While the virtual height is above the maximum height of the refractable */
  /* F-region, adjust the propagation path                                   */
  while(*vheight > F_hmax && new_hop <= max_hop && good_hop == 1)
    {
      /* Add a hop and recalculate the slant distance and virtual height */
      new_hop += 1.0;
      *vheight = calc_elv_vheight(*slant_dist, new_hop, radius, *elv);
      good_hop = TestPropagation(new_hop, *vheight, *slant_dist, D_hmin, D_hmax,
				 E_hmax);
    }

  /* Evaluate the status of the current propagation path */
  if(new_hop <= max_hop && good_hop == 1)
    {
      /* This is a realistic propagation path.  The original Python */
      /* algorithm from Burrell et al. (2015) would assign the      */
      /* ionospheric region here as well.                           */
      *hop        = new_hop;
      *slant_dist = *slant_dist / (2.0 * new_hop);
    }
  else
    {
      /* This is an unrealistic propagation path, return unrealistic values */
      *hop        = 0.0;
      *slant_dist = 0.0;
      *elv        = -1.0;
      *vheight    = 0.0;
    }

  return;
}

/**
 * @brief determine the ionospheric region
 *
 * @param[in] hop     - number of hops (e.g., 0.5, 1.0, 1.5, etc.)
 *            D_hmin  - minimum height for D region in km
 *            D_hmax  - maximum height for D region in km
 *            E_hmax  - maximum height for E region in km
 *            F_hmax  - maximum height for E region in km
 *            vheight - virtual height in km
 *
 * @param[out] region - "D", "E", "F", or "U" (unknown)
 *
 * @notes Each region has a maximum number of hops:
 *        D - 0.5
 *        E - 1.5
 *        F - Specified outside of this routine
 *        Mixed propagation paths are not identified here (yet)
 **/

void SetRegion(float hop, float D_hmin, float D_hmax, float E_hmax,
	       float F_hmax, float vheight, char *region)
{
  
  if(vheight < D_hmin || vheight > F_hmax) sprintf(region, "U");
  else if(vheight <= D_hmax)
    {
      if(hop > 0.5) sprintf(region, "U");
      else          sprintf(region, "D");
    }
  else if(vheight <= E_hmax)
    {
      if(hop > 1.5) sprintf(region, "U");
      else          sprintf(region, "E");
    }
  else sprintf(region, "F");
  return;
}

/**
 * @brief Calculates the slant range to a range gate.
 *
 * @param[in] frang - distance to first range gate in km
 *            rsep  - range gate size in km
 *            rxris - receiver rise time in microseconds
 *            irg   - zero offset range gate index
 *
 * @param[out] sdist - slant distance in km
 **/
double slant_range(int frang, int rsep, double rxris, int irg)
{

    int lagfr, smsep;
    double sdist;

    /* Calculate the lag to first range gate in microseconds */
    lagfr = frang * 20 / 3;

    /* Calculate the sample separation in microseconds */
    smsep = rsep * 20 / 3;

    /* Return the calculated slant range distance [km] */
    sdist = C * 5.0e-10 * (lagfr - rxris + irg * smsep);

    return sdist;
}
