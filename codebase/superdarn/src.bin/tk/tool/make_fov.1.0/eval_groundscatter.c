/* eval_groundscatter.c
   ====================
   Author: Angeline G. Burrell - NRL - 2021
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

#include <stdio.h>
#include <stdlib.h>

#include "multbsid.h"

/**
 * @brief Update the groundscatter flag 'gflg' for points already flagged as GS
 *
 * @param[in] beam - radar beam structure
 *
 * Criteria
 * --------
 *  * power above 0
 *  * gflg of 1
 *  * range gate above 10 for any positive power
 *  * range gate below 10 and between 0.0-5.0 dB power
 *  * at least half of backscatter in a 10 range gate box is groundscatter
 *  * at least 3 groundscatter points are found in a 10 range gate box
 **/

void EvalGroundScatter(struct FitBSIDBeam *beam)
{
  int irg, gflg, min_rg, max_rg, box_width, npnts, nmin;

  float max_power, gflg_frac, gs_tol;

  int CheckGroundScatter(int rg, int gflg, float slant_dist, float power,
			 int min_rg, int max_rg, float max_power);
  float CalcFracGroundScatter(int box_width, int icenter,
			      struct FitBSIDBeam *beam, int *npnts);

  /* Set the evaluation defaults */
  min_rg    = 10;
  max_rg    = beam->nrang;
  max_power = 5.0;
  box_width = 5;
  gs_tol    = 0.5;
  nmin      = 3;

  /* Check each groundscatter flag, looking for bad values */
  for(irg = 0; irg < beam->nrang; irg++)
    {
      if(beam->sct[irg] == 1 && beam->rng[irg].gsct == 1)
	{
	  gflg = CheckGroundScatter(irg, beam->rng[irg].gsct,
				    beam->front_loc[irg].dist,
				    beam->rng[irg].p_l, min_rg, max_rg,
				    max_power);

	  /* Compare new flag with current flag, set groundscatter flag */
	  /* to -1 if the groundscatter check failed                    */
	  if(gflg != beam->rng[irg].gsct) beam->rng[irg].gsct = -1;
	}
    }

  /* After the single point check, remove any isolated groundscatter points */
  for(irg = 0; irg < beam->nrang; irg++)
    {
      if(beam->sct[irg] == 1 && beam->rng[irg].gsct == 1)
	{
	  npnts = 0;
	  gflg_frac = CalcFracGroundScatter(box_width, irg, beam, &npnts);

	  if(gflg_frac < gs_tol || npnts < nmin) beam->rng[irg].gsct = -1;
	}
    }

  return;
}

/**
 * @brief Single-point groundscatter quality evaluation
 *
 * @param[in] rg         - range gate index
 *            gflg       - GS flag (1=ground scatter, 0=ionospheric scatter)
 *            slant_dist - 1-hop slant distance in km
 *            power      - signal power in dB
 *            min_rg     - minimum range gate for D-region power considerations
 *            max_rg     - maximum range gate
 *            max_power  - maximum power for near-range gate groundscatter
 *
 * @param[out] out_flag - Returns 0 if GS check fails, 1 if it passes
 *
 * @notes Routine uses the following criteria to evaluate groundscatter status:
 *        * power above 0
 *        * gflg of 1
 *        * 1/2 hop distances closer than 160 km <- re-evaluate for h' 100 km?
 *        * range gate above min_rg for any positive power
 *        * range gate below min_rg and between 0.0 and max_power
 **/

int CheckGroundScatter(int rg, int gflg, float slant_dist, float power,
		       int min_rg, int max_rg, float max_power)
{
  int out_gflg = 0;

  float min_slant_dist = 78;  /* minimum slant distance to be ionospheric */

  /* Ensure the scatter fits the standard groundscatter definition (slow */
  /* moving), has a positive power (successful fit), and has a slant     */
  /* distance long enough to have refracted in the ionosphere.           */
  if(gflg == 1 && power >= 0.0 && slant_dist > min_slant_dist)
    {
      if(rg < min_rg)
	{
	  /* Ensure the power in the near-range gates */
	  /* is low enough to not be meteor ablation. */
	  if(power < max_power) out_gflg = 1;
	}
      else if(rg <= max_rg) out_gflg = 1;  /* Maximum range gate to consider */
    }

  return(out_gflg);
}


/**
 * @brief Proximity test for groundscatter along a beam
 *
 * @param[in] box_width - 1/2 size of box in range gates
 *            rg_center - range gate of central point
 *            beam      - FitRange structured radar beam
 *
 * @param[out] npnts - number of points with groundscatter in box
 *             frac  - fraction of points in range gate box that are GS
 **/

float CalcFracGroundScatter(int box_width, int rg_center,
			    struct FitBSIDBeam *beam, int *npnts)
{
  int irg, num, min_box, max_box;

  float frac;

  /* Set the upper and lower range gate limits for the current box */
  min_box = rg_center - box_width;
  max_box = rg_center + box_width;

  if(min_box < 0) min_box = 0;
  if(max_box > beam->nrang) max_box = beam->nrang;

  /* Initialize the output */
  num  = 0;
  frac = 0.0;

  /* Cycle through the range gates in the box, updating the total number of */
  /* points and total number of groundscatter points                        */
  for(irg = min_box; irg < max_box; irg++)
    {
      if(beam->sct[irg] == 1)
	{
	  num++;
	  if(beam->rng[irg].gsct == 1) frac += 1.0;
	}
    }

  /* Set the output */
  if(num > 0 && frac > 0.0) frac /= (float)num;
  *npnts = num;

  return(frac);
}
