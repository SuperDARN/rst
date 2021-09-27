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
