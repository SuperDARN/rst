/* propagation.h
   =============
   Author: Angeline G. Burrell - NRL - 2021
   This is a U.S. government work and not under copyright protection in the U.S.

   This file is part of the Radar Software Toolkit (RST).

   Disclaimer: RST is licensed under GPL v3.0. Please visit 
               <https://www.gnu.org/licenses/> to see the full license

   Modifications:

*/


#ifndef _PROPAGATION_H
#define _PROPAGATION_H

int TestPropagation(float hop, float vheight, float slant_dist, float D_hmin,
		    float D_hmax, float E_hmax);


void AdjustPropagation(int lobe, float radius, float D_hmin, float D_hmax,
		       float E_hmax, float F_hmax, float max_hop,
		       int bmnum, int tfreq, struct RadarSite *site,
		       double psi_obs, float *hop, float *vheight,
		       double *elv, float *slant_dist);


void SetRegion(float hop, float D_hmin, float D_hmax, float E_hmax,
	       float F_hmax, float vheight, char *region);

#endif
