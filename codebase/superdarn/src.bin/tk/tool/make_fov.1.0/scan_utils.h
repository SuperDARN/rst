/* scan_utils.h
   ============
   Author: Angeline G. Burrell - NRL - 2021
   This is a U.S. government work and not under copyright protection in the U.S.

   This file is part of the Radar Software Toolkit (RST).

   Disclaimer: RST is licensed under GPL v3.0. Please visit 
               <https://www.gnu.org/licenses/> to see the full license

   Modifications:

*/

#ifndef _SCAN_UTILS_H
#define _SCAN_UTILS_H

#ifndef _RMATH_H
#include "rmath.h"
#endif

#ifndef _STAT_UTILS_H
#include "stats.h"
#endif

#ifndef _FITMULTBSID_H
#include "fitmultbsid.h"
#endif

#ifndef MAX_BMS
#define MAX_BMS 25  /* Current maximum in hdw files */
#endif

#ifndef MAX_RGS
#define MAX_RGS 225  /* Current maximum in hdw files */
#endif

void eval_az_var_in_elv(int num, int fov, int scan_bm[], int scan_rg[],
			int fovflg[MAX_BMS][MAX_RGS],
			int fovpast[MAX_BMS][MAX_RGS], float scan_vh[],
			float scan_elv[], float fovstd[MAX_BMS][MAX_RGS],
			float fovscore[MAX_BMS][MAX_RGS]);
void eval_fov_flag_consistency(int max_rg, int max_bm, int bmwidth, int D_rgmax,
			       int D_nrg, int E_rgmax, int E_nrg, int F_rgmax,
			       int F_nrg, int far_nrg,
			       int fovflg[MAX_BMS][MAX_RGS],
			       int fovpast[MAX_BMS][MAX_RGS],
			       int fovbelong[MAX_BMS][MAX_RGS][3],
			       int opp_in[MAX_BMS][MAX_RGS],
			       struct FitBSIDScan *scan);
void get_rg_box_limits(int rg, int max_rg, int D_rgmax, int E_rgmax,
		       int F_rgmax, int D_nrg, int E_nrg, int F_nrg,
		       int far_nrg, int *rg_min, int *rg_max);
#endif
