/* elvheight.c
   ===========
   Author: Angeline G. Burrell - NRL - 2021
   This is a U.S. government work and not under copyright protection in the U.S.

   This file is part of the Radar Software Toolkit (RST).

   Disclaimer: RST is licensed under GPL v3.0. Please visit 
               <https://www.gnu.org/licenses/> to see the full license

   Modifications:

*/

#include <math.h>
#include <stdio.h>

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
