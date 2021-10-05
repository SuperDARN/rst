/* update_beam.c
   =============
   Author: Angeline G. Burrell - NRL - 2021
   This is a U.S. government work and not under copyright protection in the U.S.

   This file is part of the Radar Software Toolkit (RST).

   Disclaimer: RST is licensed under GPL v3.0. Please visit 
               <https://www.gnu.org/licenses/> to see the full license

   Modifications:

*/

#include <stdio.h>
#include <stdlib.h>

#include "radar.h"
#include "fitmultbsid.h"
#include "rpos.h"
#include "shfconst.h"
#include "elevation.h"
#include "propagation.h"

#ifndef RE_KM
#define RE_KM Re / 1000.0
#endif

/**
 * @brief Update the RadarBSIDBeam.
 *
 * @param[in] strict_gs - (1/0) Remove indeterminate backscatter (1)
 *            max_hop   - maximum number of hops to consider (3.0)
 *            D_hmin    - Minimum h' for D-region in km (75)
 *            D_hmax    - Maximum h' for D-region in km (115)
 *            E_hmax    - Maximum h' for E-region in km (150)
 *            F_hmax    - Minimum h' for F-region in km (450)
 *            site      - Radar hardware site structure
 *
 * @param[in/out] beam - Radar backscatter ID beam structure with data to update
 **/

void UpdateBeamFit(short int strict_gs, float max_hop, float D_hmin,
		   float D_hmax, float E_hmax, float F_hmax,
		   struct RadarSite *site, struct FitBSIDBeam *beam)
{
  int irg;

  float vh_low, vh_high;

  double range_edge;

  void EvalGroundScatter(struct FitBSIDBeam *beam);

  /* Calculate the 1/2 hop distance and initialize the hop values */
  for(irg=0; irg<beam->nrang; irg++)
    {
      if(beam->sct[irg] == 1)
	{
	  range_edge = -0.5 * beam->rsep * 20.0 / 3.0;
	  beam->front_loc[irg].dist = slant_range_no_edge(beam->frang,
							  beam->rsep,
							  (double)beam->rxrise,
							  irg);
	  beam->back_loc[irg].dist = beam->front_loc[irg].dist;
	}
    }

  /* Update the groundscatter flag */
  EvalGroundScatter(beam);

  /* Use the front and back lobe values */
  for(irg = 0; irg < beam->nrang; irg++)
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

	  if(beam->sct[irg] == 1 && beam->rng[irg].phi0 == 0.0)
	    beam->sct[irg] = 0.0;

	  else if(beam->sct[irg] == 1)
	    {
	      /* Calculate the elevation. Front must be assigned here */
	      /* as well, since TDIFF may have been updated.          */
	      beam->front_elv[irg].normal = elevation_v2_lobe(1, beam->bm,
							      beam->freq, site,
							      beam->rng[irg].phi0);
	      beam->back_elv[irg].normal = elevation_v2_lobe(-1, beam->bm,
							     beam->freq, site,
							     beam->rng[irg].phi0);

	      /* Calculate the virtual heights */
	      beam->front_loc[irg].vh = calc_elv_vheight(beam->front_loc[irg].dist, beam->front_loc[irg].hop, RE_KM, beam->front_elv[irg].normal);
	      beam->back_loc[irg].vh = calc_elv_vheight(beam->back_loc[irg].dist, beam->back_loc[irg].hop, RE_KM, beam->back_elv[irg].normal);
	      sprintf(beam->front_loc[irg].vh_m, "E");
	      sprintf(beam->back_loc[irg].vh_m, "E");

	      /* Test and adjust the virtual height and propagation path */
	      AdjustPropagation(1, RE_KM, D_hmin, D_hmax, E_hmax, F_hmax,
				max_hop, beam->bm, beam->freq, site,
				beam->rng[irg].phi0, &beam->front_loc[irg].hop,
				&beam->front_loc[irg].vh,
				&beam->front_elv[irg].normal,
				&beam->front_loc[irg].dist);
	      AdjustPropagation(-1, RE_KM, D_hmin, D_hmax, E_hmax, F_hmax,
				max_hop, beam->bm, beam->freq, site,
				beam->rng[irg].phi0, &beam->back_loc[irg].hop,
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
		  beam->front_elv[irg].low = elevation_v2_lobe(1, beam->bm,
							       beam->freq, site,
							       beam->rng[irg].phi0 - beam->rng[irg].phi0_e);
		  beam->front_elv[irg].high = elevation_v2_lobe(1, beam->bm,
								beam->freq,
								site, beam->rng[irg].phi0 + beam->rng[irg].phi0_e);
		  beam->back_elv[irg].low = elevation_v2_lobe(-1, beam->bm,
							      beam->freq, site,
							      beam->rng[irg].phi0 - beam->rng[irg].phi0_e);
		  beam->back_elv[irg].high = elevation_v2_lobe(-1, beam->bm,
							       beam->freq, site,
							       beam->rng[irg].phi0 + beam->rng[irg].phi0_e);

		  /* Add the virtual height errors */
		  vh_low = beam->front_loc[irg].vh - calc_elv_vheight(beam->front_loc[irg].dist, beam->front_loc[irg].hop, RE_KM, beam->front_elv[irg].low);
		  vh_high = calc_elv_vheight(beam->back_loc[irg].dist, beam->back_loc[irg].hop, RE_KM, beam->front_elv[irg].high) - beam->front_loc[irg].vh;
		  beam->front_loc[irg].vh_e = (vh_low > vh_high) ? vh_low : vh_high;

		  vh_low = beam->back_loc[irg].vh - calc_elv_vheight(beam->back_loc[irg].dist, beam->back_loc[irg].hop, RE_KM, beam->back_elv[irg].low);
		  vh_high = calc_elv_vheight(beam->back_loc[irg].dist, beam->back_loc[irg].hop, RE_KM, beam->back_elv[irg].high) - beam->back_loc[irg].vh;
		  beam->back_loc[irg].vh_e = (vh_low > vh_high) ? vh_low : vh_high;

		  /* Add the region ID */
		  SetRegion(beam->front_loc[irg].hop, D_hmin, D_hmax, E_hmax,
			    F_hmax, beam->front_loc[irg].vh,
			    beam->front_loc[irg].region);
		  SetRegion(beam->back_loc[irg].hop, D_hmin, D_hmax, E_hmax,
			    F_hmax, beam->back_loc[irg].vh,
			    beam->back_loc[irg].region);
		}
	    }
	}
      else
	{
	  /* Set defaults for the empty points */
	  beam->front_loc[irg].hop    = 0.0;
	  beam->front_loc[irg].dist   = 0.0;
	  beam->front_elv[irg].normal = 0.0;
	  beam->front_elv[irg].low    = 0.0;
	  beam->front_elv[irg].high   = 0.0;
	  beam->back_loc[irg].hop     = 0.0;
	  beam->back_loc[irg].dist    = 0.0;
	  beam->back_elv[irg].normal  = 0.0;
	  beam->back_elv[irg].low     = 0.0;
	  beam->back_elv[irg].high    = 0.0;
	}
    }

  return;
}