/* propagation.c
   =============
   Author: Angeline G. Burrell - NRL - 2021
   This is a U.S. government work and not under copyright protection in the U.S.

   This file is part of the Radar Software Toolkit (RST).

   Disclaimer: RST is licensed under GPL v3.0. Please visit 
               <https://www.gnu.org/licenses/> to see the full license

   Modifications:

*/

#include <math.h>
#include <stdio.h>

#include "radar.h"
#include "rmath.h"
#include "elevation.h"
#include "propagation.h"

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
