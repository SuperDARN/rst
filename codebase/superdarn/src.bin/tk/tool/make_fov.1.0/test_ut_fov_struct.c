/* test_ut_fov_struct.c
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
#include <string.h>

#include "rtypes.h"
#include "rtime.h"
#include "multbsid.h"
#include "scan_utils.h"

/**
 * @brief Examine the FoV values assigned for scans as a function of time
 *
 * @params[in] vb          - Verbosity flag
 *             vbuf        - Verbosity buffer
 *             min_pnts    - Minimum number of points necessary to perfrom
 *                           certain range gate or beam specific evaluations (3)
 *             min_frac    - Minimum fraction of possible backscatter points
 *                           needed in the RG/UT box to test the FoV (0.1)
 *             max_hop     - maximum number of hops to consider (3.0)
 *             ut_box_sec  - UT box size in decimal seconds (1200.0)
 *             D_nrg       - Number of range gates for D-region box (2)
 *             E_nrg       - Number of range gates for E-region box (5)
 *             F_nrg       - Number of range gates for near F-region box (10)
 *             far_nrg     - Number of range gates for far F-region box (20)
 *             D_rgmax     - Maximum range gate for D-region box width (5)
 *             E_rgmax     - Maximum range gate for E-region box width (25)
 *             F_rgmax     - Maximum range gate for near F-region box width (40)
 *             D_hmin      - Minimum h' for D-region in km (75)
 *             D_hmax      - Maximum h' for D-region in km (115)
 *             E_hmax      - Maximum h' for E-region in km (150)
 *             F_hmax      - Minimum h' for F-region in km (450)
 *             D_vh_box    - h' range for D-region box in km (40)
 *             E_vh_box    - h' range for E-region box in km (35)
 *             F_vh_box    - h' range for near F-region box in km (50)
 *             far_vh_box  - h' range for far F-region box in km (150)
 *
 * @params[out] mult_bsid - Ouptut data structure
 **/

void test_ut_fov_struct(unsigned char vb, char *vbuf, float min_frac,
			double ut_box_sec, int D_nrg, int E_nrg, int F_nrg,
			int far_nrg, int D_rgmax, int E_rgmax, int F_rgmax,
			float D_hmin, float D_hmax, float E_hmax, float F_hmax,
			struct MultFitBSID *mult_bsid)
{
  int iscan, ibm, irg, ireg, ihop, ibox, iwin, istime, ietime, ifov, bind;
  int max_pnts, num_bms, max_rg, max_reg, cpid, bmnum, num_rg;
  int good_fov, bad_fov, rmin, rmax, fnum[2], bnum[2];
  int ***fovflg, ***region, ***hop, **fov_in, **fov_out, **fov_mix, **opp_in;

  float fov_frac, frac_num, past_frac;

  double *stime;

  struct CellBSIDLoc loc, opp;
  struct FitBSIDBeam *bm;
  struct FitBSIDScan *scan;

  /* Test the input structure length */
  if(mult_bsid->ed_time - mult_bsid->st_time < ut_box_sec)
    {
      if(vb) sprintf(vbuf,
		     "too few scans to perform a temporal FoV evaluation\n");
      return;
    }

  /* Initialize the local variables */
  fov_frac = 2.0 / 3.0;
  max_reg  = 3;

  opp_in  = (int **)NULL;
  fov_in  = (int **)NULL;
  fov_out = (int **)NULL;
  fov_mix = (int **)NULL;
  fovflg  = (int ***)NULL;
  region  = (int ***)NULL;
  hop     = (int ***)NULL;
  stime   = (double *)NULL;

  /* Load the first scan */
  scan    = mult_bsid->scan_ptr;
  num_bms = scan->num_bms;
  max_rg  = -1;

  /* Cycle through all of the scans for each beam */
  for(ibm = 0; ibm < num_bms; ibm++)
    {
      /* Set the beam number and CPID for this beam eval */
      cpid   = scan->bm[ibm].cpid;
      bmnum  = scan->bm[ibm].bm;
      num_rg = scan->bm[ibm].nrang;

      /* Initialize the FoV flag tracking pointers */
      if(fovflg == NULL)
	{
	  fovflg  = (int ***)malloc(sizeof(int **) * 2);
	  region  = (int ***)malloc(sizeof(int **) * 2);
	  hop     = (int ***)malloc(sizeof(int **) * 2);
	  fov_in  = (int **)malloc(sizeof(int *) * mult_bsid->num_scans);
	  fov_out = (int **)malloc(sizeof(int *) * mult_bsid->num_scans);
	  fov_mix = (int **)malloc(sizeof(int *) * mult_bsid->num_scans);
	  opp_in  = (int **)malloc(sizeof(int *) * mult_bsid->num_scans);
	  stime   = (double *)calloc(mult_bsid->num_scans, sizeof(double));

	  for(ifov = 0; ifov < 2; ifov++)
	    {
	      fovflg[ifov] = (int **)malloc(sizeof(int *)
					    * mult_bsid->num_scans);
	      region[ifov] = (int **)malloc(sizeof(int *)
					    * mult_bsid->num_scans);
	      hop[ifov]    = (int **)malloc(sizeof(int *)
					    * mult_bsid->num_scans);
	    }
	}

      if(max_rg < num_rg)
	{
	  if(max_rg == -1)
	    {
	      for(iscan = 0; iscan < mult_bsid->num_scans; iscan++)
		{
		  for(ifov = 0; ifov < 2; ifov++)
		    {
		      fovflg[ifov][iscan] = (int *)malloc(sizeof(int) * num_rg);
		      region[ifov][iscan] = (int *)malloc(sizeof(int) * num_rg);
		      hop[ifov][iscan]    = (int *)malloc(sizeof(int) * num_rg);
		    }

		  fov_in[iscan]  = (int *)malloc(sizeof(int) * num_rg);
		  fov_out[iscan] = (int *)malloc(sizeof(int) * num_rg);
		  fov_mix[iscan] = (int *)malloc(sizeof(int) * num_rg);
		  opp_in[iscan]  = (int *)malloc(sizeof(int) * num_rg);
		}
	    }
	  else
	    {
	      for(iscan = 0; iscan < mult_bsid->num_scans; iscan++)
		{
		  for(ifov = 0; ifov < 2; ifov++)
		    {
		      fovflg[ifov][iscan]  = (int *)
			realloc(fovflg[iscan], num_rg * sizeof(int));
		      region[ifov][iscan]  = (int *)
			realloc(region[iscan], num_rg * sizeof(int));
		      hop[ifov][iscan]     = (int *)
			realloc(hop[iscan], num_rg * sizeof(int));
		    }

		  fov_in[iscan]  = (int *)
		    realloc(fov_in[iscan], num_rg * sizeof(int));
		  fov_out[iscan] = (int *)
		    realloc(fov_out[iscan], num_rg * sizeof(int));
		  fov_mix[iscan] = (int *)
		    realloc(fov_mix[iscan], num_rg * sizeof(int));
		  opp_in[iscan]  = (int *)
		    realloc(fov_in[iscan], num_rg * sizeof(int));
		}
	    }

	  max_rg = num_rg;
	}

      /* Cycle through each scan, loading the desired beam data */
      for(iscan = 0; iscan < mult_bsid->num_scans; iscan++)
	{
	  /* Load the desired beam for this scan */
	  if(iscan == 0) bind = ibm;
	  else           bind = get_bm_by_bmnum(ibm, scan);

	  bm = &scan->bm[bind];

	  if(ibm == 0) stime[iscan] = scan->st_time;

	  /* Ensure a consistent CPID and number of range gates */
	  if(bm->cpid == cpid && bm->nrang == num_rg)
	    {
	      /* Load the FoV data for this beam, sorting by path and RG bin */
	      for(irg = 0; irg < num_rg; irg++)
		{
		  /* Initialize each scan/RG pointer to zero before setting */
		  for(ifov = 0; ifov < 2; ifov++)
		    {
		      fovflg[ifov][iscan][irg] = 0;
		      region[ifov][iscan][irg] = 0;
		      hop[ifov][iscan][irg]    = 0;
		    }

		  fov_in[iscan][irg]  = 0;
		  fov_out[iscan][irg] = 0;
		  fov_mix[iscan][irg] = 0;
		  opp_in[iscan][irg]  = 0;

		  /* Only consider data with a FoV flag */
		  if(bm->sct[irg] == 1 && bm->rng_flgs[irg].fov != 0)
		    {
		      if(bm->rng_flgs[irg].fov == 1)
			{
			  loc = bm->front_loc[irg];
			  opp = bm->back_loc[irg];
			}
		      else
			{
			  loc = bm->back_loc[irg];
			  opp = bm->front_loc[irg];
			}

		      /* Determine the path by region and hop */
		      if(strstr(loc.region, "D") != NULL)      ireg = 0;
		      else if(strstr(loc.region, "E") != NULL) ireg = 1;
		      else if(strstr(loc.region, "F") != NULL) ireg = 2;
		      else                                     ireg = -1;
		      ihop = (int)(2.0 * loc.hop);

		      if(ireg >= 0 && ihop > 0)
			{
			  fovflg[0][iscan][irg] = bm->rng_flgs[irg].fov;
			  region[0][iscan][irg] = ireg;
			  hop[0][iscan][irg]    = ihop;
			}

		      if(bm->rng_flgs[irg].fov_past != 0)
			{
			  /* Determine the path by region and hop */
			  if(strstr(opp.region, "D") != NULL)      ireg = 0;
			  else if(strstr(opp.region, "E") != NULL) ireg = 1;
			  else if(strstr(opp.region, "F") != NULL) ireg = 2;
			  else                                     ireg = -1;
			  ihop = (int)(2.0 * opp.hop);

			  if(ireg >= 0 && ihop > 0)
			    {
			      fovflg[1][iscan][irg] = bm->rng_flgs[irg].fov_past;
			      region[1][iscan][irg] = ireg;
			      hop[1][iscan][irg]    = ihop;
			    }
			}

		    }
		}
	    }
	  else
	    {
	      /* Set each scan/RG pointer to zero, ensuring no bad access */
	      for(irg = 0; irg < max_rg; irg++)
		{
		  for(ifov = 0; ifov < 2; ifov++)
		    {
		      fovflg[ifov][iscan][irg]  = 0;
		      region[ifov][iscan][irg]  = 0;
		      hop[ifov][iscan][irg]     = 0;
		    }

		  fov_in[iscan][irg]  = 0;
		  fov_out[iscan][irg] = 0;
		  fov_mix[iscan][irg] = 0;
		  opp_in[iscan][irg]  = 0;
		}
	    }

	  /* Cycle to the next scan */
	  scan = scan->next_scan;
	}

      /* For this beam, sum the number of front/back FoV detections in */
      /* each UT/RG/Region/Hop bin.                                    */
      istime = 0;
      ietime = 0;
      for(iscan = 0; iscan < mult_bsid->num_scans; iscan++)
	{
	  /* Cycle the start time index */
	  while(stime[iscan] - stime[istime] > 0.5 * ut_box_sec
		&& istime < iscan)
	    istime++;
	  
	  /* Cycle the end time index */
	  while(stime[ietime] - stime[iscan] < 0.5 * ut_box_sec
		&& ietime < mult_bsid->num_scans - 1)
	    ietime++;

	  for(irg = 0; irg < num_rg; irg++)
	    {
	      if(fovflg[0][iscan][irg] != 0)
		{
		  /* Determine the range gate bin size */
		  get_rg_box_limits(irg, num_rg, D_rgmax, E_rgmax, F_rgmax,
				    D_nrg, E_nrg, F_nrg, far_nrg, &rmin, &rmax);

		  /* Determine the maximum number of points in this bin */
		  max_pnts = (ietime - istime) * (rmax - rmin);

		  /* Cycle through all points in the UT/RG bin.  Also test    */
		  /* to see if flipping this point agrees with all the other  */
		  /* (unflipped) points around it better than the current FoV */
		  for(ifov = 0; ifov < 2; ifov++)
		    {
		      fnum[ifov] = 0;
		      bnum[ifov] = 0;
		      for(iwin = istime; iwin < ietime; iwin++)
			{
			  for(ibox = rmin; ibox < rmax; ibox++)
			    {
			      if(iwin == iscan && ibox == irg)
				{
				  if(fovflg[ifov][iwin][ibox] == 1)
				    fnum[ifov]++;
				  else if(fovflg[ifov][iwin][ibox] == -1)
				    bnum[ifov]++;
				}
			      else if((hop[0][iwin][ibox]
				       == hop[ifov][iscan][irg])
				      && (region[0][iwin][ibox]
					  == region[ifov][iscan][irg]))
				{
				  /* Sum, since this point has the same    */
				  /* propagation paths the central point. */
				  if(fovflg[0][iwin][ibox] == 1)
				    fnum[ifov]++;
				  else if(fovflg[0][iwin][ibox] == -1)
				    bnum[ifov]++;

				}
			    }
			}

		      /* See if there are enough points for an evaluation */
		      frac_num = (float)(fnum[ifov] + bnum[ifov]);
		      if(frac_num < (float)max_pnts * min_frac)
			{
			  if(vb)
			    sprintf(vbuf, "insufficent data at bm %d/RG %d for UT test\n", bm->bm, irg);
			}
		      else if(fnum[ifov] + bnum[ifov] > max_pnts)
			{
			  fprintf(stderr, "too many points at bm %d, ", bm->bm);
			  fprintf(stderr, "range gate %d (%d > %d)\n", irg,
				  fnum[ifov] + bnum[ifov], max_pnts);
			  exit(1);
			}
		      else
			{
			  /* Evaluate the fraction of points in each FoV */
			  frac_num = (float)fnum[ifov] / frac_num;

			  if(ifov == 0)
			    {
			      if(frac_num >= fov_frac)
				{
				  good_fov = 1;
				  bad_fov  = -1;
				}
			      else if(1.0 - frac_num >= fov_frac)
				{
				  good_fov = 1;
				  bad_fov  = 1;
				}
			      else
				{
				  good_fov = 0;
				  bad_fov  = 0;
				}

			      /* Update the inlier/outlier/mixed sum for each */
			      /* point that went into this evaluation.        */
			      for(iwin = istime; iwin < ietime; iwin++)
				{
				  for(ibox = rmin; ibox < rmax; ibox++)
				    {
				      if((hop[ifov][iwin][ibox]
					  == hop[ifov][iscan][irg])
					 && (region[ifov][iwin][ibox]
					     == region[ifov][iscan][irg]))
					{
					  if(good_fov == 1)
					    {
					      if(fovflg[ifov][iwin][ibox]
						 == bad_fov)
						fov_out[iwin][ibox]++;
					      else fov_in[iwin][ibox]++;
					    }
					  else if(fovflg[ifov][iwin][ibox] != 0)
					    fov_mix[iwin][ibox]++;
					}
				    }
				}
			    }
			  else
			    {
			      /* Determine if swapping the FoV makes this */
			      /* point an inlier.                         */
			      past_frac = (float)fnum[0] / (float)(fnum[0]
								   + bnum[0]);
			      if(frac_num >= fov_frac
				 && fovflg[ifov][iscan][irg] == 1)
				{
				  opp_in[iscan][irg] = 1;

				  if(frac_num > past_frac)
				    opp_in[iscan][irg]++;
				}
			      else if(1.0 - frac_num >= fov_frac
				      && fovflg[ifov][iscan][irg] == -1)
			      {
				opp_in[iscan][irg] = 1;

				if(frac_num < past_frac)
				    opp_in[iscan][irg]++;
				}
			      else opp_in[iscan][irg] = 0;
			    }
			}
		    }
		}
	    }
	}

      /* Set the scan pointer back to the first scan */
      scan = mult_bsid->scan_ptr;

      /* Update the FoV flags for this beam */
      for(iscan = 0; iscan < mult_bsid->num_scans; iscan++)
	{
	  /* Load the desired beam for this scan */
	  if(iscan == 0) bind = ibm;
	  else           bind = get_bm_by_bmnum(ibm, scan);

	  bm = &scan->bm[bind];

	  /* Ensure a consistent CPID and number of range gates */
	  if(bm->cpid == cpid && bm->nrang == num_rg)
	    {
	      /* Load the FoV data for this beam, sorting by path and RG bin */
	      for(irg = 0; irg < num_rg; irg++)
		{
		  /* Only consider data with a FoV flag */
		  if(bm->sct[irg] == 1 && fovflg[0][iscan][irg] != 0)
		    {
		      /* This point was included in the UT/RG          */
		      /* evaluation and can be updated if it is found  */
		      /* to be an outlier, not clearly in either FoV,  */
		      /* or a better inlier in the opposite FoV.       */     
		      if(fov_in[iscan][irg] < (fov_out[iscan][irg]
					       + fov_mix[iscan][irg])
			 && fov_out[iscan][irg] > 0)
			{
			  /* This point is not an inlier */
			  if(fov_out[iscan][irg] > fov_mix[iscan][irg]
			     && fov_out[iscan][irg] > fov_in[iscan][irg])
			    /* This point is an outlier */
			    bm->rng_flgs[irg].fov = bm->rng_flgs[irg].fov_past;

			  else
			    {
			      if(opp_in[iscan][irg] > 0)
				/* This point is an inlier when swapped */
				bm->rng_flgs[irg].fov = bm->rng_flgs[irg].fov_past;
			      else
				/* This point is complicated */
				bm->rng_flgs[irg].fov = 0;
			    }

			  bm->rng_flgs[irg].fov_past = fovflg[0][iscan][irg];
			}
		      else if(opp_in[iscan][irg] == 2)
			{
			  /* This point is a better inlier when swapped */
			  bm->rng_flgs[irg].fov = bm->rng_flgs[irg].fov_past;
			  bm->rng_flgs[irg].fov_past = fovflg[0][iscan][irg];
			}
		    }
		}
	    }

	  /* Cycle to the next scan */
	  scan = scan->next_scan;
	}

      /* Set the scan pointer back to the first scan */
      scan = mult_bsid->scan_ptr;
    }

  /* Free the local pointers */
  if(fovflg != NULL)
    {
      for(iscan = 0; iscan < mult_bsid->num_scans; iscan++)
	{
	  for(ifov = 0; ifov < 2; ifov++)
	    {
	      free(fovflg[ifov][iscan]);
	      free(region[ifov][iscan]);
	      free(hop[ifov][iscan]);
	    }

	  free(fov_in[iscan]);
	  free(fov_out[iscan]);
	  free(fov_mix[iscan]);
	  free(opp_in[iscan]);
	}

      for(ifov = 0; ifov < 2; ifov++)
	{
	  free(fovflg[ifov]);
	  free(region[ifov]);
	  free(hop[ifov]);
	}

      free(fovflg);
      free(region);
      free(hop);
      free(fov_in);
      free(fov_out);
      free(fov_mix);
      free(opp_in);
    }

  return;
}
