/* update_beam.c
   =============
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

/**
 * UpdateBeamFit: Update the groundscatter flag 'gflg' for points already
 *                    flagged as groundscatter.
 *
 * Criteria
 * --------
 * - power above 0
 * - gflg of 1
 * - range gate above 10 for any positive power
 * - range gate below 10 and between 0.0-5.0 dB power
 * - at least half of backscatter in a 10 range gate box is groundscatter
 * - at least 3 groundscatter points are found in a 10 range gate box
 *
 * Input: beam - radar beam structure
 **/

void UpdateBeamFit(int ptest, int strict_gs, float D_hmin, float D_hmax,
		   float E_hmin, float E_hmax, float F_hmin, float F_hmax,
		   struct FitPrm *prm, struct RadarBSIDBeam *beam)
{
  float reg_hmin, reg_hmax;

  void EvalGroundScatter(struct FitRange *beam);
  double elevation_v2_lobe(int lobe, struct FitPrm *prm, double psi_obs);


  return;
}
