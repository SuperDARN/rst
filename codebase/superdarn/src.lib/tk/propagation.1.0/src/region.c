/* region.c
   ========
   Author: Angeline G. Burrell - NRL - 2021
   This is a U.S. government work and not under copyright protection in the U.S.

   This file is part of the Radar Software Toolkit (RST).

   Disclaimer: RST is licensed under GPL v3.0. Please visit 
               <https://www.gnu.org/licenses/> to see the full license

   Modifications:

*/

#include <stdio.h>

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
