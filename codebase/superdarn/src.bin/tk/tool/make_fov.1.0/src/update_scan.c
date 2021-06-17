/* update_scan.c
   =============
   Author: Angeline G. Burrell - NRL - 2020
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

#define MAX_BMS 25  /* Current maximum in hdw files */
#define MAX_RG 225  /* Current maximum in hdw files */
#define MIN_BMS 3  /* Minimum number of beams to find azimuthal variations */
#define RG_INC 3

/**
 * @brief Update scan backscatter propagation path, elevation angle,
 *        backscatter type, structure flag, and origin field of view for all
 *        backscatter observatoins in each bean for a scan of data.  A full
 *        scan is not necessary, but if the number of beams is less than the
 *        specified minimum, a less rigerous evaluation method will be used.
 *
 * Input: strict_gs (1/0) Remove indeterminate backscatter (rec=1)
 *        step (1-6) Number of processing steps to perform: (rec=6)
 *              1, 2: Examine the elevation structure across each scan
 *              3: Add assignments for points with realistic heights in one FoV
 *              4: Add assignments using single-beam elevation angle variations
 *              5, 6: Test assignmnets for consistency along the scan
 *        min_pnts (+) Minimum number of points necessary to perfrom certain
 *                      range gate or beam specific evaluations (rec=3)
 *        dreg_nrg (+) Number of range gates for D-region box (rec=2)
 *        ereg_nrg (+) Number of range gates for E-region box (rec=5)
 *        freg_nrg (+) Number of range gates for near F-region box (rec=10)
 *        far_nrg (+) Number of range gates for far F-region box (rec=20)
 *        dreg_hmin (+) Minimum h' for D-region in km (rec=75)
 *        ereg_hmin (+) Minimum h' for E-region in km (rec=115)
 *        freg_hmin (+) Minimum h' for F-region in km (rec=150)
 *        freg_hmax (+) Maximum h' for F-region in km (rec=450)
 *        dreg_vh_box (+) h' range for D-region box in km (rec=40)
 *        ereg_vh_box (+) h' range for E-region box in km (rec=35)
 *        freg_vh_box (+) h' range for near F-region box in km (rec=50)
 *        far_vh_box (+) h' range for far F-region box in km (rec=150)
 *        max_hop (N.0, N.5) maximum number of hops to consider (rec=3.0)
 *        mult_scan (*) Input scans
 *        mult_bsid (*) Output scans with additional data
 **/

int UpdateScanBSFoV(short int strict_gs, short int step, int min_pnts,
		    int D_nrg, int E_nrg, int F_nrg, int far_nrg,
		    int D_rgmax, int E_rgmax, int F_rgmax, int far_rgmax,
		    float D_hmin, float D_hmax, float E_hmax, float F_hmax,
		    float D_vh_box, float E_vh_box, float F_vh_box,
		    float far_vh_box, float max_hop,
		    struct MultRadarScan *mult_scan,
		    struct MultFoVScan *mult_bsid)
{
  int iscan, ibm, irg, ifov, ipath, ireg, ivh;
  int rmin, rmax, max_path, out_num, group_num, bm_num;
  int group_bm[MAX_BMS * MAX_RG], group_rg[MAX_BMS * MAX_RG];
  int group_num[MAX_BMS * MAX_RG];
  int fovflg[MAX_BMS][MAX_RG], fovpast[MAX_BMS][MAX_RG];
  int *scan_num[3][2], **scan_bm[3][2], **scan_rg[3][2];

  float near_rg, hmin, hmax, hbox;
  float group_elv[MAX_BMS * MAX_RG], group_vh[MAX_BMS * MAX_RG], *vmins, *vmaxs;
  float fovstd[MAX_BMS][MAX_RG], fovscore[MAX_BMS][MAX_RG];
  float **scan_vh[3][2], **scan_elv[3][2];

  struct CellBSIDLoc loc;
  struct FitElv elv;
  struct RadarCell rng;
  struct RadarParm *prm;
  struct RadarSite *site=NULL;
  struct RadarBeam *bm_old;
  struct RadarBSIDBeam *bm_new;
  struct RadarScanCycl *scan_old;
  struct RadarBSIDCycl *scan_new, *prev_new;

  void UpdateBeamFit(short int strict_gs, float max_hop, float D_hmin,
		     float D_hmax, float E_hmax, float F_hmax,
		     struct FitPrm *prm, struct RadarBSIDBeam *beam);
  int select_alt_groups(int num, int *rg, float *vh, float vh_min, float vh_max,
			float vh_box, int min_pnts, float *vh_mins,
			float *vh_maxs);
  int num_unique_int_vals(int num, int array[]);
  void eval_az_var_in_elv(int num, int fov, int scan_bm[], int scan_rg[],
			  int fovflg[][], int fovpast[][], float scan_vh[],
			  float scan_elv[], float fovscore[][],
			  float fovstd[][]);
 
  /* Initialize the local pointers */
  scan_old = mult_scan->scan_ptr;
  bm_new = (struct RadarBSIDBeam *)(malloc(sizeof(struct RadarBSIDBeam)));
  memset(bm_new, 0, sizeof(struct RadarBSIDBeam));

  for(ibm = 0; ibm < MAX_BMS; ibm++)
    {
      for(irg = 0; irg < MAX_RG; irg++)
	{
	  fovflg[ibm][irg] = 0;
	  fovpast[ibm][irg] = 0;
	}
    }

  max_path = (int)(max_hop * 2.0);
  for(ireg = 0; ireg < 3; ireg++)
    {
      out_num = (ireg == 0) ? D_rgmax : ((ireg == 1) ? E_rgmax : F_rgmax);

      for(ifov = 0; ifov < 2; ifov++)
	{
	  scan_num[ireg][ifov] = (int *)calloc(sizeof(int) * max_path);
	  scan_bm[ireg][ifov] = (int **)calloc(sizeof(int) * max_path);
	  scan_rg[ireg][ifov] = (int **)calloc(sizeof(int) * max_path);
	  scan_vh[ireg][ifov] = (float **)calloc(sizeof(float) * max_path);
	  scan_elv[ireg][ifov] = (float **)calloc(sizeof(float) * max_path);

	  for(ipath = 0; ipath < max_path; ipath++)
	    {
	      scan_bm[ireg][ifov][ipath] = (int *)calloc(sizeof(int) * out_num);
	      scan_rg[ireg][ifov][ipath] = (int *)calloc(sizeof(int) * out_num);
	      scan_vh[ireg][ifov][ipath] = (float *)calloc(sizeof(float)
							   * out_num);
	      scan_elv[ireg][ifov][ipath] = (float *)calloc(sizeof(float)
							    * out_num);
	    }
	}
    }

  out_num = (F_rgmax > E_rgmax) ? F_rgmax : E_rgmax;
  if(D_rgmax > out_num) out_num = D_rgmax;
  vmins = (float *)calloc(sizeof(float) * out_num);
  vmaxs = (float *)calloc(sizeof(float) * out_num);

  /* Inititalize the output */
  if(mult_bsid->num_scans == 0)
    {
      mult_bsid->scan = *RadarBSIDCyclMake();
      scan_new        = mult_bsid->scan_ptr;

      mult_bsid->stid = mult_scan->bsid;
      mult_bsid->st_time = mult_scan->st_time;
      mult_bsid->ed_time = mult_scan->ed_time;
      mult_bsid->version.major = mult_scan->version.major;
      mult_bsid->version.minor = mult_scan->version.minor;
    }
  else
    {
      prev_new            = mult_bsid->last_ptr;
      prev_new->next_scan = *RadarBSIDCyclMake();
      scan_new            = prev_new->next_scan;
      scan_new->next_scan = prev_new;

      mult_bsid->ed_time = mult_scan->ed_time;
    }
  
  
  /* Cycle through the scans */
  for(iscan = 0; iscan < mult_scan->num_scans; iscan++)
    {
     scan_new->st_time = scan_old->st_time;
     scan_new->ed_time = scan_old->ed_time;
     scan_new->num = scan_old->num;
     scan_new->bm = bm_new;

      /* Cycle through each beam in the scan */
      for(ibm = 0; ibm < scan_old->num; ibm++)
	{
	  bm_old = scan_old->bm;

	  if(ibm == 0)
	    {
	      /* Calculate oncee for each scan */
	      near_rg = ((500.0 / (5.0e-10 * C) - bm_old->prm.lagfr)
			 / bm_old->prm.smsep);
	    }
 
	  /* This corresponds to 
	   * davitpy.pydarn.proc.fov.update_backscatter.update_beam_fit
	   *
	   * Start by initializing new beams in the new scan */
	  bm_new->scan    = bm_old->scan;
	  bm_new->bm      = bm_old->bm;
	  bm_new->time    = bm_old->time;
	  bm_new->cpid    = bm_old->cpid;
	  bm_new->intt.sc = bm_old->intt.sc;
	  bm_new->intt.us = bm_old->intt.us;
	  bm_new->nave    = bm_old->nave;
	  bm_new->frang   = bm_old->frang;
	  bm_new->rsep    = bm_old->rsep;
	  bm_new->rxrise  = bm_old->rxrise;
	  bm_new->freq    = bm_old->freq;
	  bm_new->noise   = bm_old->noise;
	  bm_new->atten   = bm_old->atten;
	  bm_new->channel = bm_old->channel;
	  bm_new->nrang   = bm_old->nrang;
	  bm_new->sct     = bm_old->sct;

	  bm_new->rng       = (struct RadarCell *)
	    (malloc(sizeof(struct RadarCell) * bm_new->nrang));
	  bm_new->med_rng   = (struct RadarCell *)
	    (malloc(sizeof(struct RadarCell) * bm_new->nrang));
	  bm_new->rng_flgs  = (struct CellBSIDFlgs *)
	    (malloc(sizeof(struct CellBSIDFlgs) * bm_new->nrang));
	  bm_new->front_rng = (struct CellBSIDLoc *)
	    (malloc(sizeof(struct CellBSIDLoc) * bm_new->nrang));
	  bm_new->back_rng  = (struct CellBSIDLoc *)
	    (malloc(sizeof(struct CellBSIDLoc) * bm_new->nrang));

	  memset(bm_new->rng, 0, sizeof(struct RadarCell) * bm_new->nrang);
	  memset(bm_new->med_rng, 0, sizeof(struct RadarCell) * bm_new->nrang);
	  memset(bm_new->rng_flgs, 0, sizeof(struct CellBSIDFlgs)
		 * bm_new->nrang);
	  memset(bm_new->front_rng, 0, sizeof(struct CellBSIDLoc)
		 * bm_new->nrang);
	  memset(bm_new->back_rng, 0, sizeof(struct CellBSIDLoc)
		 * bm_new->nrang);

	  for(irg = 0; irg < bm_new->nrang; irg++)
	    {
	      /* Load only for range gates with data */
	      if(bm->sct[irg] == 1)
		{
		  rng = bm_old->rng[irg];
		  bm_new->rng[irg].gsct = rng.gsct;
		  bm_new->rng[irg].p_0 = rng.p_0;
		  bm_new->rng[irg].p_0_e = rng.p_0_e;
		  bm_new->rng[irg].v = rng.v;
		  bm_new->rng[irg].v_e = rng.v_e;
		  bm_new->rng[irg].w_l = rng.w_l;
		  bm_new->rng[irg].w_l_e = rng.w_l_e;
		  bm_new->rng[irg].p_l = rng.p_l;
		  bm_new->rng[irg].p_l_e = rng.p_l_e;
		}
	    }
	  
	  /* Take the initialized beam and update the front and back FoVs */
	  UpdateBeamFit(strict_gs, max_hop, D_hmin, D_hmax, E_hmax, F_hmax,
			prm, bm_new);

	  /* Find the altitude bins for this scan by FoV, region, and path */
	  for(ifov = 0; ifov < 2; ifov++)
	    {
	      for(ipath = 0; ipath < max_path; ipath++)
		{
		  scan_num_D[ifov][ipath] = 0;
		  scan_num_E[ifov][ipath] = 0;
		  scan_num_F[ifov][ipath] = 0;
		  scan_num_far[ifov][ipath] = 0;
		}
	    }

	  for(irg = 0; irg < bm_new->nrang; irg++)
	    {
	      if(bm_new->sct[irg] == 1)
		{
		  /* Assign this data to the correct region list for each  */
		  /* field of view (ifov = 0 for back lobe and 1 for front */
		  for(ifov = 0; ifov < 2; ifov++)
		    {
		      loc = (ifov == 0) ? bm_new->back_loc : bm_new->front_loc;
		      elv = (ifov == 0) ? bm_new->back_elv : bm_new->front_elv;
		      ipath = (int)(loc->hop * 2.0);
		      if(strstr(loc->region, "D") != NULL)      ireg = 0;
		      else if(strstr(loc->region, "E") != NULL) ireg = 1;
		      else if(strstr(loc->region, "F") != NULL) ireg = 2;
		      else                                      ireg = -1;

		      if(ireg >= 0)
			{
			  scan_bm[ireg][ifov][ipath][scan_num[ireg][ifov][ipath]] = ibm;
			  scan_rg[ireg][ifov][ipath][scan_num[ireg][ifov][ipath]] = irg;
			  scan_vh[ireg][ifov][ipath][scan_num[ireg][ifov][ipath]] = (float)loc.vh;
			  scan_elv[ireg][ifov][ipath][scan_num[ireg][ifov][ipath]] = (float)elv.normal;
			  scan_num[ireg][ifov][ipath]++;
			}
		    }
		}
	    }			
	}

      /* To determine the FoV, evaluate the elevation variations across all */
      /* beams for a range gate and virtual height band, considering each   */
      /* propagation path (region and hop) seperately.                      */
      for(ireg = 0; ireg < 3; ireg++)
	{
	  if(ireg == 0)
	    {
	      hmin = D_hmin;
	      hmax = D_hmax;
	      hbox = D_vh_box;
	    }
	  else if(ireg == 1)
	    {
	      hmin = E_hmin;
	      hmax = E_hmax;
	      hbox = E_vh_box;
	    }
	  else if(ireg == 2)
	    {
	      hmin = F_hmin;
	      hmax = F_hmax;
	      hbox = F_vh_box;
	    }
	  
	  for(ifov = 0; ifov < 2; ifov++)
	    {
	      for(ipath = 0; ipath < max_path; ipath++)
		{
		  if(ireg == 2 && ipath >= 3) hbox = far_vh_box;

		  if(scan_num[ireg][ifov][ipath] >= min_pnts)
		    {
		      /* Use select_alt_groups */
		      out_num = select_alt_groups(scan_num[ireg][ifov][ipath],
						  scan_rg[ireg][ifov][ipath],
						  scan_vh[ireg][ifov][ipath],
						  hmin, hmax, hbox, min_pnts,
						  vmins, vmaxs);

		      /* For each virtual height bin, determine if this FoV */
		      /* has a realistic azimuth variation.                 */
		      for(ivh = 0; ivh < out_num; ivh++)
			{
			  for(group_num = 0, irg = 0;
			      irg < scan_num[ireg][ifov][ipath]; irg++)
			    {
			      if(scan_vh[ireg][ifov][ipath][irg] >= vmins[irg]
				 && scan_vh[ireg][ifov][ipath][irg] < vmaxs[irg])
				{
				  group_bm[group_num] = scan_bm[ireg][ifov][ipath][irg];
				  group_rg[group_num] = scan_rg[ireg][ifov][ipath][irg];
				  group_vh[group_num] = scan_vh[ireg][ifov][ipath][irg];
				  group_elv[group_num] = scan_elv[ireg][ifov][ipath][irg];
				  group_num++;
				}
			    }

			  if(group_num >= min_pnts)
			    {
			      /* Test to see if there are enough beams */
			      bm_num = num_unique_int_vals(group_num, scan_bm[ireg][ifov][ipath]);
			      if(bm_num >= MIN_BMS)
				{
				  eval_az_var_in_elv(group_num, ifov, scan_bm,
						     scan_rg, fovflg, fovpast,
						     scan_vh, scan_elv,
						     fovscore, fovstd);
				}
			    }
			}
		    }
		}
	    }
	}

      /* Evaluate the FoV flags, removing points that are surrounded by data */
      /* assigned to the opposite FoV.                                       */
      for(rmin = 0, rmax = 0, irg = 0; irg < max_rg; irg++)
	{
	  /* Get the half-width of the range gate box */
	  if(irg < D_rgmax)      out_num = (D_nrg + RG_INC) / 2;
	  else if(irg < E_rgmax) out_num = (E_nrg + RG_INC) / 2;
	  else if(irg < F_rgmax) out_num = (F_nrg + RG_INC) / 2;
	  else                   out_num = (far_nrg + RG_INC) / 2;

	  /* Set the upper and lower box limits. This changes the logic    */
	  /* from the davitpy implementation by allowing the limits of a   */
	  /* range gate box to extend into range gates beyond their limit. */
	  rmin = (irg - out_num < 0) ? 0 : irg - out_num;
	  rmax = rmin + 2 * out_num;
	  if(rmax > bm_new->nrang + 1) rmax = bm_new->nrang + 1;

	  /* For each bean in the maximum possible range gate window, gather */
	  /* the range gate, FoV flag, beam index, and range gate index by   */
	  /* propagation path.                                               */
	  for(ibm = 0; ibm < max_bm; ibm++)
	    {
	      // HERE; go define max_bm and max_rg
	    }
	}
    }

  return;
}


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
 *            fovscore - Beam x RG array holding previous z-score values
 *            fovstd   - Beam x RG array holding previous standard deviations
 **/

void eval_az_var_in_elv(int num, int fov, int scan_bm[], int scan_rg[],
			int fovflg[][], int fovpast[][], float scan_vh[],
			float scan_elv[], float fovscore[][], float fovstd[][])
{
  int i, irg, ibm, reg_stat;
  int get_fov[2] = {-1, 1};

  float max_std, max_score, fov_frac, intercept, sig_intercept, slope;
  float sig_slope, chi2, q, lstd, line_std;
  float *lval, *ldev, *abs_ldev, *lscore, *line_dev, *sig;

  int linear_regression(float x[], float y[], float sig[], int num, int mwt,
			float *intercept, float *sig_intercept, float *slope,
			float *sig_slope, float *chi2, float *q);
  void zscore(int num, float array[], float *zscore);
  float stdev_float(int num, float array[]);

  /* Initalize the maximum statistics and reference variables */
  max_std   = 3.0;
  max_score = 3.0;
  fov_frac  = 2.0 / 3.0;
  
  /* Initialize the evalutaion statistics to default values */
  line_std = max_std + 100.0;
  line_dev = (float *)calloc(sizeof(float) * num);
  sig      = (float *)calloc(sizeof(float) * num);
  lval     = (float *)calloc(sizeof(float) * num);
  ldev     = (float *)calloc(sizeof(float) * num);
  abs_ldev = (float *)calloc(sizeof(float) * num);
  lscore   = (float *)calloc(sizeof(float) * num);
  for(i = 0; i < num; i++)
    {
      line_dev[i] = line_std;
      sig[i] = 0.0;
    }

  /* Get the linear regression of the elevation angles as a function of    */
  /* range gate. The slope of this line must be flat or negative. Aliasing */
  /* will cause positive jumps, but these should not be present in all     */
  /* boxes, allowing data to be assigned at times when the aliasing jump   */
  /* isn't present. A more robust method (such as RANSAC or Theil-Sen) was */
  /* not used, since the number of points available are small.             */
  reg_stat = linear_regression((float *)scan_rg, scan_elv, sig, num, 0,
			       &intercept, &sig_intercept, &slope, &sig_slope,
			       &chi2, &q);

  if(reg_stat == 0)
    {
      /* If there were no calculation problems, calculate the linear values */
      for(i = 0; i < num; i++)
	{
	  lval[i] = slope * (float)scan_rg[i] + intercept;
	  ldev[i] = lval[i] - scan_elv[i];
	  abs_ldev[i] = fabs(ldev[i])
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

		  /* Also update if this is the first evaluation */
		  if(fovflg[ibm][irg] == 0 || (lscore[i] < fovscore[ibm][irg]
					       && lstd < fovstd[ibm][irg]))
		    {
		      /* If the FoV is changing, note that here */
		      if(fovflg[ibm][irg] != 0
			 && fovflg[ibm][irg] != get_fov[fov])
			fovpast[ibm][irg] = fovflg[ibm][irg];

		      /* Update with the new good FoV stats and flag */
		      fovflg[ibm][irg]   = get_fov[fov];
		      fovstd[ibm][irg]   = lstd;
		      fovscore[ibm][irg] = lscore[i];
		    }
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

  return;
}
