/* update_scan.c
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
#include <string.h>
#include <math.h>

#include "scan_utils.h"

/**
 * @brief Evaluate the azimuthal variations in a group of elevation angles
 *
 * @param[in] num - Number of points in the `scan_*` arrays
 *            fov - Integer denoting field of view (0=back, 1=front)
 *            scan_bm - Array holding the group's beam numbers
 *            scan_rg - Array holding the group's range gates
 *            fovflg  - Beam x RG array holding assigned FoVs (-1=back, 1=front)
 *            fovpast - Beam x RG array holding previously assigned FoV flags
 *            scan_vh - Array holding the group's virtual heights in km
 *            scan_elv - Array holding the group's elevation angles in deg
 **/

void eval_az_var_in_elv(int num, int fov, int scan_bm[], int scan_rg[],
			int fovflg[MAX_BMS][MAX_RGS],
			int fovpast[MAX_BMS][MAX_RGS], float scan_vh[],
			float scan_elv[], float fovstd[MAX_BMS][MAX_RGS],
			float fovscore[MAX_BMS][MAX_RGS])
{
  int i, irg, ibm, reg_stat;
  int get_fov[2] = {-1, 1};

  float max_std, max_score, intercept, sig_intercept, slope, chi2, q, lstd;
  float sig_slope, line_std, *scan_x;
  float *lval, *ldev, *abs_ldev, *lscore, *line_dev, *sig;

  int linear_regression(float x[], float y[], float sig[], int num, int mwt,
			float *intercept, float *sig_intercept, float *slope,
			float *sig_slope, float *chi2, float *q);

  /* Initalize the maximum statistics and reference variables */
  max_std   = 5.0;  /* Used to be 3.0, changed after INV tests */
  max_score = 3.0;
  
  /* Initialize the evalutaion statistics to default values */
  line_std = max_std + 100.0;
  line_dev = (float *)calloc(num, sizeof(float));
  sig      = (float *)calloc(num, sizeof(float));
  lval     = (float *)calloc(num, sizeof(float));
  ldev     = (float *)calloc(num, sizeof(float));
  abs_ldev = (float *)calloc(num, sizeof(float));
  lscore   = (float *)calloc(num, sizeof(float));
  scan_x   = (float *)calloc(num, sizeof(float));
  for(i = 0; i < num; i++)
    {
      scan_x[i]   = (float)scan_rg[i];
      line_dev[i] = line_std;
      sig[i]      = 0.0;
    }

  /* Get the linear regression of the elevation angles as a function of    */
  /* range gate. The slope of this line must be flat or negative. Aliasing */
  /* will cause positive jumps, but these should not be present in all     */
  /* boxes, allowing data to be assigned at times when the aliasing jump   */
  /* isn't present. A more robust method (such as RANSAC or Theil-Sen) was */
  /* not used, since the number of points available are small.             */
  reg_stat = linear_regression(scan_x, scan_elv, sig, num, 0, &intercept,
			       &sig_intercept, &slope, &sig_slope, &chi2, &q);

  /* Don't check for slope being negative or flat, because flat can be     */
  /* slightly positive and it's been too finicky finding a good threshold. */
  if(reg_stat == 0)
    {
      /* If there were no calculation problems, and the slope is flat or */
      /* decreasing, calculate the linear values                         */
      for(i = 0; i < num; i++)
	{
	  lval[i] = slope * scan_x[i] + intercept;
	  ldev[i] = lval[i] - scan_elv[i];
	  abs_ldev[i] = fabs(ldev[i]);
	}

      lstd = stdev_float(num, ldev);
      zscore(num, abs_ldev, lscore);

      /* Use current and past z-scores to determine whether or not each */
      /* point is well-characterized by the linear regression.          */
      if(lstd <= max_std)
	{
	  for(i = 0; i < num; i++)
	    {
	      /* If this backscatter point's score is below the maximum and */
	      /* lower than previous estimate's scores.                     */
	      if(lscore[i] <= max_score)
		{
		  irg = scan_rg[i];
		  ibm = scan_bm[i];

		  /* Also update if this is the first evaluation. Adapted the */
		  /* requirement for this line's standard to be less than the */
		  /* previous line's standard deviation as well as a lower    */
		  /* z-score.                                                 */
		  if(fovflg[ibm][irg] == 0
		     || (fabs(lscore[i]) < fovscore[ibm][irg]
			 && lstd <= fovstd[ibm][irg]))
		    {
		      /* If the FoV is changing, note that here */
		      if(fovflg[ibm][irg] != 0
			 && fovflg[ibm][irg] != get_fov[fov])
			fovpast[ibm][irg] = fovflg[ibm][irg];

		      /* Update with the new good FoV stats and flag */
		      fovflg[ibm][irg]   = get_fov[fov];
		      fovstd[ibm][irg]   = lstd;
		      fovscore[ibm][irg] = fabs(lscore[i]);
		    }
		  else if(fovpast[ibm][irg] == 0)
		    fovpast[ibm][irg] = get_fov[fov];  /* Other FoV is valid */
		}
	    }
	}
    }

  /* Free the pointers */
  free(line_dev);
  free(sig);
  free(lval);
  free(ldev);
  free(abs_ldev);
  free(lscore);
  free(scan_x);

  return;
}


/**
 * @brief: Evaluate the FoV flags, removing and identifying outliers
 *
 * @params[in] max_rg    - Maximum number of range gates
 *             max_bm    - Maximum number of beams/maximum beam index + 1
 *             bmwidth   - Beam half-width in beam numbers (75% of min_pnts)
 *             D_rgmax   - Maximum range gate for the D-region box width (5)
 *             D_nrg     - Number of range gates for D-region box (2)
 *             E_rgmax   - Maximum range gate for the E-region box width (25)
 *             E_nrg     - Number of range gates for E-region box (5)
 *             F_rgmax   - Max range gate for the near F-region box width (40)
 *             F_nrg     - Number of range gates for near F-region box (10)
 *             far_nrg   - Number of range gates for far F-region box (20)
 *             fovflg    - Beam x RG array holding assigned FoVs
 *             fovpast   - Beam x RG array holding previously assigned FoV flags
 *             fovbelong - Beam x RG x 3 array holding True/False assignments
 *                         indicating the point is an inlier (0), outlier (1),
 *                         or mixed-ID (2). Must be initialized outside of this
 *                         routine.
 *             opp_in    - Beam x RG array holding True/False assignments
 *                         indicating the point is a better inlier if the FoV
 *                         for the central point is swapped. Must be
 *                         initialized outside of this routine.
 *             scan      - Scan structure holding data by beam and range gate
 *
 * @note `*_rgmax` parameters are equivalent to `rg_max` in davitpy
 **/

void eval_fov_flag_consistency(int max_rg, int max_bm, int bmwidth, int D_rgmax,
			       int D_nrg, int E_rgmax, int E_nrg, int F_rgmax,
			       int F_nrg, int far_nrg,
			       int fovflg[MAX_BMS][MAX_RGS],
			       int fovpast[MAX_BMS][MAX_RGS],
			       int fovbelong[MAX_BMS][MAX_RGS][3],
			       int opp_in[MAX_BMS][MAX_RGS],
			       struct FitBSIDScan *scan)
{
  int irg, ibm, ifov, irsel, ibsel, bind, rmin, rmax, bmin, bmax, bad_fov;
  int fnum[2], bnum[2];
  
  float ffrac, fov_frac, ffrac_opp, near_rg;

  struct FitBSIDBeam bm, bm_ref;
  struct CellBSIDLoc loc, ref_loc[2];

  /* Initialize the variables */
  fov_frac = 2.0 / 3.0;
  near_rg  = -1.0;

  /* Cycle through all of the range gates, looking for consistency in the */
  /* neighboring backscatter fields of view.                              */
  for(irg = 0; irg < max_rg; irg++)
    {
      /* Get the range gate box limits */
      get_rg_box_limits(irg, max_rg, D_rgmax, E_rgmax, F_rgmax, D_nrg,
			E_nrg, F_nrg, far_nrg, &rmin, &rmax);

      /* For each beam in the maximum possible range gate window, gather */
      /* the range gate, FoV flag, beam index, and range gate index by   */
      /* propagation path.                                               */
      for(ibm = 0; ibm < scan->num_bms; ibm++)
	{
	  /* Cycle the reference beam and evaluation beam to the */
	  /* correct range gate                                  */
	  bm = scan->bm[ibm];

	  /* Ensure only good beams with data are used */
	  if(bm.frang > 0 && bm.rsep > 0)
	    {
	      if(near_rg < 0.0)
		{
		  /* Calculate once in the routine */
		  near_rg = ((500.0 / (5.0e-10 * C)
			      - ((float)bm.frang * 20.0 / 3.0))
			     / ((float)bm.rsep * 20.0 / 3.0));
		}
 
	      if(bm.sct[irg] == 1 && fovflg[bm.bm][irg] != 0)
		{
		  if(fovflg[bm.bm][irg] == -1)
		    {
		      ref_loc[0] = bm.back_loc[irg];
		      ref_loc[1] = bm.front_loc[irg];
		    }
		  else
		    {
		      ref_loc[0] = bm.front_loc[irg];
		      ref_loc[1] = bm.back_loc[irg];
		    }

		  /* Get the beam limits for the azimuthal box */
		  bmin = bm.bm - bmwidth;
		  bmax = bm.bm + bmwidth;
		  if(bmin < 0)      bmin = 0;
		  if(bmax > max_bm) bmax = max_bm;

		  /* Cycle through each range gate and beam in the box, */
		  /* getting the number of points for the same hop in   */
		  /* each field-of-view                                 */
		  for(ifov = 0; ifov < 2; ifov++)
		    {
		      fnum[ifov] = 0;
		      bnum[ifov] = 0;
		    }
		  
		  for(ibsel = bmin; ibsel < bmax; ibsel++)
		    {
		      for(irsel = rmin; irsel < rmax; irsel++)
			{
			  if(fovflg[ibsel][irsel] != 0)
			    {
			      bind   = get_bm_by_bmnum(ibsel, scan);
			      bm_ref = scan->bm[bind];
			      if(fovflg[ibsel][irsel] == -1)
				loc = bm_ref.back_loc[irsel];
			      else loc = bm_ref.front_loc[irsel];

			      if(loc.hop == ref_loc[0].hop)
				{
				  if(fovflg[ibsel][irsel] == 1) fnum[0]++;
				  else                          bnum[0]++;
				}
			      
			      if(fovpast[bind][irg] != 0)
				{
				  if(irg == irsel && bm.bm == ibsel)
				    {
				      if(fovpast[ibsel][irsel] == 1) fnum[1]++;
				      else                           bnum[1]++;
				    }
				  else if(loc.hop == ref_loc[1].hop)
				    {
				      if(fovflg[ibsel][irsel] == 1) fnum[1]++;
				      else                          bnum[1]++;
				    }
				}
			    }
			}
		    }

		  /* Sum up the number of points in this range gate/beam box */
		  /* and see if there is an overwhelming number of points in */
		  /* either field-of-view.                                   */
		  if(fnum[0] + bnum[0] > 0)
		    {
		      /* Get the bad field of view and test the opposite FoV */
		      ffrac     = (float)fnum[0] / (float)(fnum[0] + bnum[0]);
		      ffrac_opp = (float)fnum[1] / (float)(fnum[1] + bnum[1]);
		      if(ffrac >= fov_frac)
			{
			  bad_fov = -1;

			  if(ffrac_opp > ffrac) opp_in[bm.bm][irg] = 1;
			}
		      else if((1.0 - ffrac) >= fov_frac)
			{
			  bad_fov = 1;

			  if(ffrac_opp < ffrac) opp_in[bm.bm][irg] = 1;
			}
		      else
			{
			  bad_fov = 0;

			  if((ffrac_opp >= fov_frac)
			     || ((1.0 - ffrac_opp) >= fov_frac))
			    opp_in[bm.bm][irg] = 1;
			  else opp_in[bm.bm][irg] = 0;
			}

		      if(opp_in[bm.bm][irg] == 1
			 && fovpast[bm.bm][irg] == bad_fov)
			opp_in[bm.bm][irg] = 0;

		      /* Tag all points whose FoV are or are not */
		      /* consistent with the observed structure  */
		      /* at this propagation path                */
		      for(ibsel = bmin; ibsel < bmax; ibsel++)
			{
			  for(irsel = rmin; irsel < rmax; irsel++)
			    {
			      if(fovflg[ibsel][irsel] != 0)
				{
				  bind   = get_bm_by_bmnum(ibsel, scan);
				  bm_ref = scan->bm[bind];
				  if(fovflg[ibsel][irsel] == -1)
				    loc = bm_ref.back_loc[irsel];
				  else loc = bm_ref.front_loc[irsel];

				  if(ifov == 0 && loc.hop == ref_loc[0].hop)
				    {
				      if(bad_fov == 0)
					fovbelong[ibsel][irsel][2]++;
				      else if(fovflg[ibsel][irsel] == bad_fov)
					{
					  /* If this point is not          */
					  /* associated with a structure   */
					  /* dominated by points with the  */
					  /* same FoV, and this is not     */
					  /* the only FoV able to produce  */
					  /* a realistic elevation angle,  */
					  /* flag this point as an outlier */
					  if(irsel < near_rg)
					    fovbelong[ibsel][irsel][1]++;
					}
				      else fovbelong[ibsel][irsel][0]++;
				    }
				}
			    }
			}
		    }
		}
	    }	  
	}
    }

  return;
}

/**
 * @brief Cycle through scan, returning beam by beam number
 *
 * @param[in] ibm  - Zero-index beam number
 *            scan - Scan data structure
 *
 * @param[out] bm - Pointer to the desired beam structure
 **/

int get_bm_by_bmnum(int ibm, struct FitBSIDScan *scan)
{
  int i;

  struct FitBSIDBeam bm;

  if(ibm < scan->num_bms)
    {
      /* This scan may be ordered by beam number */
      i  = ibm;
      bm = scan->bm[i];

      /* This scan may be ordered by reverse beam order */
      if(bm.bm != ibm)
	{
	  i  = scan->num_bms - (ibm + 1);
	  bm = scan->bm[i];
	}

      if(bm.bm == ibm) return(i);
    }

  /* Not a clear relationship between beam index and beam number */
  i  = 0;
  bm = scan->bm[i];

  while(bm.bm != ibm && i < scan->num_bms)
    bm = scan->bm[++i];

  if(i >= scan->num_bms)
    {
      fprintf(stderr, "can't find beam number [%d] in scan with time [%f]\n",
	      ibm, scan->st_time);
      exit(1);
    }

  return(i);
}


/**
 * @brief Get range gate limits for an input point and set regional guidelines
 *
 * @params[in] rg      - Zero-starting range gate index
 *             max_rg  - Maximum transmission frequency limit in kH
 *             D_rgmax - Maximum range gate for D-region box width (5)
 *             E_rgmax - Maximum range gate for E-region box width (25)
 *             F_rgmax - Maximum range gate for near F-region box width (40)z
 *             D_nrg   - Number of range gates for D-region box (2)
 *             E_nrg   - Number of range gates for E-region box (5)
 *             F_nrg   - Number of range gates for near F-region box (10)
 *             far_nrg - Number of range gates for far F-region box (20)
 *
 * @params[out] rg_min - minimum range gate in box
 *              rg_max - maximum range gate in box
 **/

void get_rg_box_limits(int rg, int max_rg, int D_rgmax, int E_rgmax,
		       int F_rgmax, int D_nrg, int E_nrg, int F_nrg,
		       int far_nrg, int *rg_min, int *rg_max)
{
  int width;

  /* Get the half-width of the range gate box */
  if(rg < D_rgmax)      width = D_nrg / 2;   /* (D_nrg   + inc) / 2; */
  else if(rg < E_rgmax) width = E_nrg / 2;   /* (E_nrg   + inc) / 2; */
  else if(rg < F_rgmax) width = F_nrg / 2;   /* (F_nrg   + inc) / 2; */
  else                  width = far_nrg / 2; /* (far_nrg + inc) / 2; */

  /* Set the upper and lower box limits, limiting them  */
  /* to the possible upper and lower range gate limits. */
  *rg_min = (rg - width < 0) ? 0 : rg - width;
  *rg_max = (rg + width > max_rg) ? max_rg : rg + width;

  return;
}
