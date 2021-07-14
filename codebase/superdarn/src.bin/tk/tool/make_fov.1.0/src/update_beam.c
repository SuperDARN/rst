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

#include "shfconst.h"

#include "multbsid.h"

/**
 * @brief Update the RadarBSIDBeam.
 *
 * @param[in] strict_gs - (1/0) Remove indeterminate backscatter (1)
 *            max_hop   - maximum number of hops to consider (3.0)
 *            D_hmin    - Minimum h' for D-region in km (75)
 *            D_hmax    - Maximum h' for D-region in km (115)
 *            E_hmax    - Maximum h' for E-region in km (150)
 *            F_hmax    - Minimum h' for F-region in km (450)
 *
 * @param[in/out] beam - Radar backscatter ID beam structure with data to update
 **/

void UpdateBeamFit(short int strict_gs, float max_hop, float D_hmin,
		   float D_hmax, float E_hmax, float F_hmax,
		   struct RadarBSIDBeam *beam)
{
  short int falias, balias;
  
  int irg;

  float vh_low, vh_high;

  void EvalGroundScatter(struct RadarBSIDBeam *beam);
  double elevation_v2_lobe(int lobe, struct FitPrm *prm, double psi_obs,
			   double alias);
  float calc_elv_vheight(float slant_dist, float hop, float radius, float elv);
  short int AdjustPropagation(int lobe, float radius, float D_hmin,
			      float D_hmax, float E_hmax, float F_hmax,
			      float max_hop, struct FitPrm *prm, double psi_obs,
			      float *hop, float *vheight, double *elv,
			      float *slant_dist);
  void SetRegion(float D_hmin, float D_hmax, float E_hmax, float F_hmax,
		 float vheight, char *region);
  float slant_range(float smsep, float lagfr, int irg);

  /* Calculate the 1/2 hop distance and initialize the hop values */
  for(irg=0; irg<beam->nrang; irg++)
    {
      if(beam->sct[irg] == 1)
	{
	  beam->front_loc[irg].dist = slant_range(beam->prm.smsep,
						   beam->prm.lagfr, irg);
	  beam->back_loc[irg].dist = beam->front_loc[irg].dist;
	}
    }

  /* Update the groundscatter flag */
  EvalGroundScatter(beam);

  /* Use the front and back lobe values */
  for(irg=0; irg<beam->nrang; irg++)
    {
      if(beam->sct[irg] == 1)
	{
	  if(beam->rng[irg].gsct == 1)
	    {
	      /* Update groundscatter hop and distance */
	      beam->front_loc[irg].hop = 1.0;
	      beam->front_loc[irg].dist *= 0.5;
	      beam->back_loc[irg].hop = 1.0;
	      beam->back_loc[irg].dist *= 0.5;
	    }
	  else
	    {
	      if((strict_gs == 1) && (beam->rng[irg].gsct == -1))
		{
		  /* Remove bad groundscattter hop and distance by assigning */
		  /* negative values (since NaN is more complicated)         */
		  beam->front_loc[irg].hop = 0.0;
		  beam->front_loc[irg].dist = 0.0;
		  beam->back_loc[irg].hop = 0.0;
		  beam->back_loc[irg].dist = 0.0;
		  beam->sct[irg] = 0;
		}
	      else
		{
		  beam->front_loc[irg].hop = 0.5;
		  beam->back_loc[irg].hop = 0.5;

		  if(beam->rng[irg].p_l < 0.0)
		    {
		      /* Remove bad ionospheric hop and distance by assigning */
		      /* unrealistic values (since NaN is more complicated)   */
		      beam->front_loc[irg].hop = 0.0;
		      beam->front_loc[irg].dist = 0.0;
		      beam->back_loc[irg].hop = 0.0;
		      beam->back_loc[irg].dist = 0.0;
		      beam->sct[irg] = 0;
		    }
		}
	    }

	  if(beam->sct[irg] == 1)
	    {
	      /* Calculate the back lobe elevation, as front was assigned */
	      /* during the beam initialization.                          */
	      beam->back_elv[irg].normal = elevation_v2_lobe(-1, &beam->prm, beam->rng[irg].phi0, 0.0);

	      /* Calculate the virtual heights */
	      beam->front_loc[irg].vh = calc_elv_vheight(beam->front_loc[irg].dist, beam->front_loc[irg].hop, Re, beam->front_elv[irg].normal);
	      beam->back_loc[irg].vh = calc_elv_vheight(beam->back_loc[irg].dist, beam->back_loc[irg].hop, Re, beam->back_elv[irg].normal);
	      sprintf(beam->front_loc[irg].vh_m, "E");
	      sprintf(beam->back_loc[irg].vh_m, "E");

	      /* Test and adjust the virtual height and propagation path */
	      falias = AdjustPropagation(1, Re, D_hmin, D_hmax, E_hmax,
					 F_hmax, max_hop, &beam->prm,
					 beam->rng[irg].phi0,
					 &beam->front_loc[irg].hop,
					 &beam->front_loc[irg].vh,
					 &beam->front_elv[irg].normal,
					 &beam->front_loc[irg].dist);
	      balias = AdjustPropagation(-1, Re, D_hmin, D_hmax, E_hmax,
					 F_hmax, max_hop, &beam->prm,
					 beam->rng[irg].phi0,
					 &beam->back_loc[irg].hop,
					 &beam->back_loc[irg].vh,
					 &beam->back_elv[irg].normal,
					 &beam->back_loc[irg].dist);

	      /* If a realistic propagtion path could not be found in  */
	      /* either field of view, remove this positive scatter ID */
	      if(beam->front_loc[irg].hop == 0.0
		 && beam->back_loc[irg].hop == 0.0)
		beam->sct[irg] = 0;
	      else
		{
		  /* Add the elevation angle errors */
		  if(falias)
		    {
		      /* This step isn't necesssary if no changes made */
		      beam->front_elv[irg].low = elevation_v2_lobe(-1, &beam->prm, beam->rng[irg].phi0 - beam->rng[irg].phi0_err, 1.0);
		      beam->front_elv[irg].high = elevation_v2_lobe(-1, &beam->prm, beam->rng[irg].phi0 + beam->rng[irg].phi0_err, 1.0);
		    }
		  beam->back_elv[irg].low = elevation_v2_lobe(-1, &beam->prm, beam->rng[irg].phi0 - beam->rng[irg].phi0_err, (float)balias);
		  beam->back_elv[irg].high = elevation_v2_lobe(-1, &beam->prm, beam->rng[irg].phi0 + beam->rng[irg].phi0_err, (float)balias);

		  /* Add the virtual height errors */
		  vh_low = beam->front_loc[irg].vh - calc_elv_vheight(beam->front_loc[irg].dist, beam->front_loc[irg].hop, Re, beam->front_elv[irg].low);
		  vh_high = calc_elv_vheight(beam->back_loc[irg].dist, beam->back_loc[irg].hop, Re, beam->front_elv[irg].high) - beam->front_loc[irg].vh;
		  beam->front_loc[irg].vh_e = (vh_low > vh_high) ? vh_low : vh_high;

		  vh_low = beam->back_loc[irg].vh - calc_elv_vheight(beam->back_loc[irg].dist, beam->back_loc[irg].hop, Re, beam->back_elv[irg].low);
		  vh_high = calc_elv_vheight(beam->back_loc[irg].dist, beam->back_loc[irg].hop, Re, beam->back_elv[irg].high) - beam->back_loc[irg].vh;
		  beam->back_loc[irg].vh_e = (vh_low > vh_high) ? vh_low : vh_high;

		  /* Add the region ID */
		  SetRegion(D_hmin, D_hmax, E_hmax, F_hmax,
			    beam->front_loc[irg].vh,
			    beam->back_loc[irg].region);
		  SetRegion(D_hmin, D_hmax, E_hmax, F_hmax,
			    beam->back_loc[irg].vh,
			    beam->back_loc[irg].region);
		}
	    }
	}
    }

  return;
}
