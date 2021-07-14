/* eval_scan_fov.c
   ===============
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
#include <string.h>
#include <math.h>

#include "multbsid.h"

#define MAX_SCAN 1440
#define MAX_RG 225

/**
 * @brief Evaluate the elevation angles in a scan to determine the FoV
 *
 * @param[in] min_pnts - Minimum number of points needed for an evaluation
 *            struct RadarBSIDScan *scan
 **/

void EvalScanFoV(int min_pnts, struct RadarBSIDScan *scan)
{
  int i, j, p, ibm, irg, nrg, min_rg, max_rg, max_box, rmin, rmax, pnum;
  int fnum[MAX_RG], bnum[MAX_RG], floc[MAX_RG], bloc[MAX_RG];
  int num[2][MAX_RG], bm_ind[2][MAX_RG][MAX_SCAN * MAX_RG];
  int rg_ind[2][MAX_RG][MAX_SCAN * MAX_RG];

  float max_inc;
  float fvh[MAX_RG][MAX_SCAN * MAX_RG];
  float fhop[MAX_RG][MAX_SCAN * MAX_RG];
  float bvh[MAX_RG][MAX_SCAN * MAX_RG];
  float bhop[MAX_RG][MAX_SCAN * MAX_RG];
  float vh[2][MAX_RG][MAX_SCAN * MAX_RG];
  float elv[2][MAX_RG][MAX_SCAN * MAX_RG];

  double width;
  double felv[MAX_RG][MAX_SCAN * MAX_RG];
  double belv[MAX_RG][MAX_SCAN * MAX_RG];

  char freg[MAX_RG][MAX_SCAN * MAX_RG][1];
  char breg[MAX_RG][MAX_SCAN * MAX_RG][1];
  char unique_prop[MAX_SCAN * MAX_RG][5];
  char reg[2][MAX_RG][MAX_SCAN * MAX_RG][5];

  struct RadarBSIDBeam *beams[MAX_SCAN];

  /* Hard code the range gate and virtual height box limits */
  int rg_box[4]   = {2, 5, 10, 20};
  int rg_max[4]   = {5, 25, 40, 76};
  float vh_box[4] = {50.0, 50.0, 50.0, 150.0};

  for(i = 0; i < 4; i++)
    {
      if(i == 0)
	{
	  width = (double)rg_box[i] / 2.0;
	  max_inc = (float)rg_max[i];
	}
      else
	{
	  if(width > (double)rg_box[i] / 2.0) width  = (double)rg_box[i] / 2.0;
	  if(max_inc < (float)rg_max[i])     max_inc = (float)rg_max[i];
	}
    }
  min_rg = (int)width;
  max_rg = (int)ceil((double)max_inc - width);

  /* Verify scan dimensions fit within expected limits */
  if(scan->num >= MAX_SCAN)
    {
      printf("EvalScanFoV ERROR: too many beams in scan [%d > %d]\n",
	     scan->num, MAX_SCAN);
      exit(1);
    }

  beams[0] = scan->bm;
  nrg      = beams[0]->nrang;
  if(nrg >= MAX_RG)
    {
      printf("EvalScanFoV ERROR: too many range gates per beam [%d > %d]\n",
	     nrg, MAX_RG);
      exit(1);
    }

  /* Initialize the beam/scan group arrays */
  for(irg = 0; irg < MAX_RG; irg++)
    {
      fnum[irg] = 0;
      floc[irg] = 0;
      bnum[irg] = 0;
      bloc[irg] = 0;
 
      for(ibm = 0; ibm < MAX_SCAN; ibm++)
	{
	  fvh[irg][irg * MAX_SCAN + ibm]  = 0.0;
	  fhop[irg][irg * MAX_SCAN + ibm] = 0.0;
	  felv[irg][irg * MAX_SCAN + ibm] = 0.0;
	  sprintf(freg[irg][irg * MAX_SCAN + ibm], "U");

	  bvh[irg][irg * MAX_SCAN + ibm]  = 0.0;
	  bhop[irg][irg * MAX_SCAN + ibm] = 0.0;
	  belv[irg][irg * MAX_SCAN + ibm] = 0.0;
	  sprintf(breg[irg][irg * MAX_SCAN + ibm], "U");

	  /* Cycle to the next beam and verify the maximum range gate value */
	  if(irg == 0 && ibm + 1 < scan->num)
	    {
	      beams[ibm + 1] = scan->bm;
	      if(beams[ibm + 1]->nrang != nrg)
		{
		  printf("EvalScanFoV ERROR: change in range gates mid-scan");
		  exit(1);
		}
	    }
	}
    }

  /* Update the beam/scan group arrays, evaluating the elevation variations */
  /* across all beams for a single propagation path (region and hop) in a   */
  /* range gate and virtual height box.                                     */
  for(i = 0, irg = min_rg; irg <= max_rg; irg++)
    {
      /* Get the box limit index for this range gate */
      while(i < 3 && irg >= rg_max[i]) i++;

      /* Determine the size of this range gate box */
      width = floor((double)rg_box[i] / 2.0);
      rmin = irg - (int)width;
      rmin = (rmin < 0) ? 0 : rmin;
      rmax = irg + (int)width + rg_box[i] % 2;
      rmax = (rmax <= nrg) ? rmax : ((nrg < max_rg) ? nrg : max_rg);

      /* For each beam, load the data for this range gate window */
      for(ibm = 0; ibm < scan->num; ibm++)
	{
	  for(j = rmin; j < rmax; j++)
	    {
	      if(beams[ibm]->sct[j] == 1)
		{
		  /* Only load data if an elevation has been calculated for */
		  /* at least one field of view.                            */
		  if(beams[ibm]->front_elv[j].normal > 0.0)
		    {
		      bm_ind[0][irg][num[0][irg]] = ibm;
		      rg_ind[0][irg][num[0][irg]] = irg;
		      vh[0][irg][num[0][irg]] = beams[ibm]->front_loc[j].vh;
		      elv[0][irg][num[0][irg]] = beams[ibm]->front_elv[j].normal;		      sprintf(reg[0][irg][num[0][irg]], "%.1f%s",
			      beams[ibm]->front_loc[j].hop,
			      beams[ibm]->front_loc[j].region);
		      num[0][irg]++;
		    }
		  if(beams[ibm]->back_elv[j].normal > 0.0)
		    {
		      bm_ind[1][irg][num[1][irg]] = ibm;
		      rg_ind[1][irg][num[1][irg]] = irg;
		      vh[1][irg][num[1][irg]] = beams[ibm]->back_loc[j].vh;
		      elv[1][irg][num[1][irg]] = beams[ibm]->back_elv[j].normal;
		      sprintf(reg[1][irg][num[1][irg]], "%.1f%s",
			      beams[ibm]->back_loc[j].hop,
			      beams[ibm]->back_loc[j].region);
		      num[1][irg]++;
		    }
		}
	    }
	}

      /* Cycle through the fields of view */
      for(j = 0; j < 2; j++)
	{
	  /* Evaluate if there is enough data centred at this range gate */
	  if(num[j][irg] >= min_pnts)
	    {
	      /* Separate the data by propagation path */
	      pnum = get_unique_str(num[j][irg], reg[j][irg], unique_prop);

	      /* For each propagation path, group by virtual height */
	      for(p = 0; p < pnum; p++)
		{
		  /* Get the virtual height groups for this prop path */
		  // HERE
		}
	    }
	}
    }

  
  return;
}
