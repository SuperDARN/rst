/* scan_utils.h
   ============
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

#ifndef _RMATH_H
#include "rmath.h"
#endif

#ifndef _STAT_UTILS_H
#include "stat_utils.h"
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
