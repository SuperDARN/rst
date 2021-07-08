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
#define MAX_RGS 225  /* Current maximum in hdw files */
#define MIN_BMS 3  /* Minimum number of beams to find azimuthal variations */
#define RG_INC 3

/**
 * @brief Update scan backscatter propagation path, elevation angle,
 *        backscatter type, structure flag, and origin field of view for all
 *        backscatter observatoins in each bean for a scan of data.  A full
 *        scan is not necessary, but if the number of beams is less than the
 *        specified minimum, a less rigerous evaluation method will be used.
 *
 * @params[in] strict_gs  - (1/0) Remove indeterminate backscatter (1)
 *             freq_min   - Minimum allowed transmission frequency in kHz
 *             freq_max   - Maximum transmission frequency limit in kHz
 *             min_pnts   - Minimum number of points necessary to perfrom
 *                          certain range gate or beam specific evaluations (3)
 *             D_nrg      - Number of range gates for D-region box (2)
 *             E_nrg      - Number of range gates for E-region box (5)
 *             F_nrg      - Number of range gates for near F-region box (10)
 *             far_nrg    - Number of range gates for far F-region box (20)
 *             D_rgmax    - Maximum range gate for D-region box width (5)
 *             E_rgmax    - Maximum range gate for E-region box width (25)
 *             F_rgmax    - Maximum  range gate for near F-region box width (40)
 *             D_hmin     - Minimum h' for D-region in km (75)
 *             D_hmax     - Maximum h' for D-region in km (115)
 *             E_hmax     - Maximum h' for E-region in km (150)
 *             F_hmax     - Minimum h' for F-region in km (450)
 *             D_vh_box   - h' range for D-region box in km (40)
 *             E_vh_box   - h' range for E-region box in km (35)
 *             F_vh_box   - h' range for near F-region box in km (50)
 *             far_vh_box - h' range for far F-region box in km (150)
 *             max_hop    - maximum number of hops to consider (3.0)
 *             mult_scan  - Input scans
 *             hard       - Radar site information from hardware file
 *
 * @params[out] mult_bsid - Output scans with location and scan-assingned FoVs
 **/

void UpdateScanBSFoV(short int strict_gs, int freq_min, int freq_max,
		     int min_pnts, int D_nrg, int E_nrg, int F_nrg, int far_nrg,
		     int D_rgmax, int E_rgmax, int F_rgmax, float D_hmin,
		     float D_hmax, float E_hmax, float F_hmax, float D_vh_box,
		     float E_vh_box, float F_vh_box, float far_vh_box,
		     float max_hop, struct MultRadarScan *mult_scan,
		     struct RadarSite *hard, struct MultFoVScan *mult_bsid)
{
  int iscan, ibm, irg, ifov, ipath, ireg, ivh, igbm;
  int max_rg, max_path, out_num, group_num, bm_num, bmwidth, igood_num;
  int igood[MAX_BMS], group_bm[MAX_BMS * MAX_RGS], group_rg[MAX_BMS * MAX_RGS];
  int group_num[MAX_BMS * MAX_RGS], fovflg[MAX_BMS][MAX_RGS];
  int fovpast[MAX_BMS][MAX_RGS], fovbelong[MAX_BMS][MAX_RGS][3];
  int front_num[MAX_BMS][MAX_RGS], back_num[MAX_BMS][MAX_RGS];
  int *scan_num[3][2], **scan_bm[3][2], **scan_rg[3][2];

  float hmin, hmax, hbox, *vmins, *vmaxs;
  float group_elv[MAX_BMS * MAX_RGS], group_vh[MAX_BMS * MAX_RGS];
  float fovstd[MAX_BMS][MAX_RGS], fovscore[MAX_BMS][MAX_RGS];
  float **scan_vh[3][2], **scan_elv[3][2];

  struct CellBSIDFlgs *rng_flgs;
  struct CellBSIDLoc loc;
  struct FitElv elv;
  struct FitNoise noise;
  struct RadarCell rng;
  struct RadarParm *prm;
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
  void eval_fov_flag_consistency(int max_rg, int bmwidth, int D_rgmax,
				 int D_nrg, int E_rgmax, int E_nrg,
				 int F_rgmax, int F_nrg, int far_nrg,
				 int fovflg[][], int fovpast[][],
				 int fovbelong[][][],
				 struct RadarBSIDCycl *scan);

  /* Initialize the local pointers and variables */
  bmwidth = (int)((float)min_pnts * 0.75);

  scan_old = mult_scan->scan_ptr;
  bm_old = (struct RadarBeam *)(malloc(sizeof(struct RadarBeam)));
  memset(bm_old, 0, sizeof(struct RadarBeam));
  bm_new = (struct RadarBSIDBeam *)(malloc(sizeof(struct RadarBSIDBeam)));
  memset(bm_new, 0, sizeof(struct RadarBSIDBeam));
  rng_flgs = (struct CellBSIDFlgs *)(malloc(sizeof(struct CellBSIDFlgs)));
  memset(rng_flgs, 0, sizeof(struct CellBSIDFlgs));
  prm = (struct struct RadarParm *)(malloc(sizeof(struct RadarParm)));
  memset(prm, 0, sizeof(struct RadarParam));

  for(ibm = 0; ibm < MAX_BMS; ibm++)
    {
      igood[ibm] = -1;

      for(irg = 0; irg < MAX_RGS; irg++)
	{
	  fovflg[ibm][irg] = 0;
	  fovpast[ibm][irg] = 0;
	  front_num[ibm][irg] = 0;
	  back_num[ibm][irg] = 0;

	  for(iscan = 0; iscan < 3; iscan++)
	    fovbelong[ibm][irg][iscan] = 0;
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

      mult_bsid->stid          = mult_scan->stid;
      mult_bsid->st_time       = mult_scan->st_time;
      mult_bsid->ed_time       = mult_scan->ed_time;
      mult_bsid->version.major = mult_scan->version.major;
      mult_bsid->version.minor = mult_scan->version.minor;
    }
  else
    {
      prev_new            = mult_bsid->last_ptr;
      prev_new->next_scan = *RadarBSIDCyclMake();
      scan_new            = prev_new->next_scan;
      scan_new->next_scan = prev_new;
      mult_bsid->ed_time  = mult_scan->ed_time;
    }
  
  
  /* Cycle through the scans */
  for(iscan = 0; iscan < mult_scan->num_scans; iscan++)
    {
      /* Before initializing this new scan data, make sure the frequency */
      /* for the beams are correct.                                      */
      for(igood_num = 0; ibm = 0; ibm < scan_old->num; ibm++)
	{
	  bm_old = scan_old->bm[ibm];
	  prm    = bm_old->prm;

	  if(prm.tfreq <= min_freq && prm.tfreq > max_freq)
	    {
	      igood[igood_num] = ibm;
	      igood_num++;
	    }
	}

      /* If there are good beams in this scan, add it to the output structure */
      if(igood_num > 0)
	{
	  scan_new->st_time = scan_old->st_time;
	  scan_new->ed_time = scan_old->ed_time;
	  scan_new->num = scan_old->num;
	  scan_new->bm = (struct RadarBSIDBeam *)
	    malloc(sizeof(struct RadarBSIDBeam) * scan_new->num);
	  memset(scan_new->bm, 0, sizeof(struct RadarBSIDBeam) * scan_new->num);

	  max_rg = 0;

	  /* Cycle through each beam in the scan, but only update the beams */
	  /* with appropriate frequencies.                                  */
	  for(igbm = 0; ibm = 0; ibm < scan_new->num; ibm++)
	    {
	      if(igood_num[igbm] == ibm)
		{
		  bm_old = scan_old->bm[ibm];
		  bm_new = scan_new->bm[ibm];
		  prm    = bm_old->prm;

		  /* This corresponds to 
		   * davitpy.pydarn.proc.fov.update_backscatter.update_beam_fit
		   *
		   * Start by initializing new beams in the new scan */
		  bm_new->time  = bm_old->time;
		  bm_new->nrang = prm->nrang;
		  strcpy(bm_new->sct, bm_old->sct);

		  /* Set the RadarParam values */
		  bm_new->prm.channel     = prm->channel;
		  bm_new->prm.offset      = prm->offset;
		  bm_new->prm.cp          = prm->cp;
		  bm_new->prm.xcf         = prm->xcf;
		  bm_new->prm.tfreq       = prm->tfreq;
		  bm_new->prm.nrange      = prm->nrang;
		  bm_new->prm.smsep       = prm->smsep;
		  bm_new->prm.rsep        = prm->rsep;
		  bm_new->prm.nave        = prm->nave;
		  bm_new->prm.mplgs       = prm->mplgs;
		  bm_new->prm.mpinc       = prm->mpinc;
		  bm_new->prm.txpl        = prm->txpl;
		  bm_new->prm.lagfr       = prm->lagfr;
		  bm_new->prm.mppul       = prm->mppul;
		  bm_new->prm.bmnum       = prm->bmnum;
		  bm_new->prm.old         = 0;
		  RadarParamSetLag(prm, prm->mplgs, (int16_t *)bm_new->prm.lag);
		  RadarParamSetPulse(prm, prm.mppul,
				     (int16_t *)bm_new->prm.pulse);

		  /* Add the radar hardware information */
		  bm_new->prm.maxbeam     = hard->maxbeam;
		  bm_new->prm.interfer[0] = hard->interfer[0];
		  bm_new->prm.interfer[1] = hard->interfer[1];
		  bm_new->prm.interfer[2] = hard->interfer[2];
		  bm_new->prm.bmsep       = hard->bmsep;
		  bm_new->prm.phidiff     = hard->phidiff;
		  bm_new->prm.tdiff       = hard->tdiff;
		  bm_new->prm.vdir        = hard->vdir;

		  /* Set the FitNoise values */
		  noise                  = bm_old->noise;
		  bm_new->noise.vel      = noise.vel;
		  bm_new->noise.skynoise = noise.skynoise;
		  bm_new->noise.lag0     = noise.lag0;

		  /* Initialize the range-dependent variables */
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

		  memset(bm_new->rng, 0,
			 sizeof(struct RadarCell) * bm_new->nrang);
		  memset(bm_new->med_rng, 0,
			 sizeof(struct RadarCell) * bm_new->nrang);
		  memset(bm_new->rng_flgs, 0, sizeof(struct CellBSIDFlgs)
			 * bm_new->nrang);
		  memset(bm_new->front_rng, 0, sizeof(struct CellBSIDLoc)
			 * bm_new->nrang);
		  memset(bm_new->back_rng, 0, sizeof(struct CellBSIDLoc)
			 * bm_new->nrang);

		  if(bm_new->nrange > max_rg) max_rg = bm_new->nrange;

		  for(irg = 0; irg < bm_new->nrang; irg++)
		    {
		      /* Load only for range gates with data */
		      if(bm_old->sct[irg] == 1)
			{
			  rng = bm_old->rng[irg];
			  bm_new->rng[irg].v        = rng.v;
			  bm_new->rng[irg].v_err    = rng.v_err;
			  bm_new->rng[irg].p_0      = rng.p_0;
			  bm_new->rng[irg].p_l      = rng.p_l;
			  bm_new->rng[irg].p_l_err  = rng.p_l_err;
			  bm_new->rng[irg].p_s      = rng.p_s;
			  bm_new->rng[irg].p_s_err  = rng.p_s_err;
			  bm_new->rng[irg].w_l      = rng.w_l;
			  bm_new->rng[irg].w_l_err  = rng.w_l_err;
			  bm_new->rng[irg].w_s      = rng.w_s;
			  bm_new->rng[irg].w_s_err  = rng.w_s_err;
			  bm_new->rng[irg].phi0     = rng.phi0;
			  bm_new->rng[irg].phi0_err = rng.phi0_err;
			  bm_new->rng[irg].sdev_l   = rng.sdev_l;
			  bm_new->rng[irg].sdev_s   = rng.sdev_s;
			  bm_new->rng[irg].sdev_phi = rng.sdev_phi;
			  bm_new->rng[irg].qflg     = rng.qflg;
			  bm_new->rng[irg].gsct     = rng.gsct;
			  strcpy(bm_new->rng[irg].nump, rng.nump);
			}
		    }

		  /* Update the front and back FoVs */
		  UpdateBeamFit(strict_gs, max_hop, D_hmin, D_hmax, E_hmax,
				F_hmax, bmprm, bm_new);

		  /* Find the altitude bins for this scan by FoV, region, and
		   * path                                                     */
		  for(ifov = 0; ifov < 2; ifov++)
		    {
		      for(ipath = 0; ipath < max_path; ipath++)
			{
			  scan_num_D[ifov][ipath]   = 0;
			  scan_num_E[ifov][ipath]   = 0;
			  scan_num_F[ifov][ipath]   = 0;
			  scan_num_far[ifov][ipath] = 0;
			}
		    }

		  for(irg = 0; irg < bm_new->nrang; irg++)
		    {
		      if(bm_new->sct[irg] == 1)
			{
			  /* Assign this data to the correct region list for */
			  /* each field of view (ifov = 0 for back lobe and  */
			  /* 1 for front)                                    */
			  for(ifov = 0; ifov < 2; ifov++)
			    {
			      if(ifov == 0)
				{
				  loc = bm_new->back_loc[irg];
				  elv = bm_new->back_elv[irg];
				}
			      else
				{
				  loc = bm_new->front_loc[irg];
				  elv = bm_new->front_elv[irg];
				}

			      ipath = (int)(loc.hop * 2.0);
			      if(strstr(loc.region, "D") != NULL) ireg = 0;
			      else if(strstr(loc.region, "E") != NULL) ireg = 1;
			      else if(strstr(loc.region, "F") != NULL) ireg = 2;
			      else ireg = -1;

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
	    }

	  /* To determine the FoV, evaluate the elevation variations across  */
	  /* all beams for a range gate and virtual height band, considering */
	  /* each propagation path (region and hop) seperately.              */
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
			  out_num = select_alt_groups(scan_num[ireg][ifov][ipath], scan_rg[ireg][ifov][ipath], scan_vh[ireg][ifov][ipath], hmin, hmax, hbox, min_pnts, vmins, vmaxs);

			  /* For each virtual height bin, determine if this */
			  /* FoV has a realistic azimuth variation.         */
			  for(ivh = 0; ivh < out_num; ivh++)
			    {
			      for(group_num = 0, irg = 0;
				  irg < scan_num[ireg][ifov][ipath]; irg++)
				{
				  if(scan_vh[ireg][ifov][ipath][irg]
				     >= vmins[irg]
				     && scan_vh[ireg][ifov][ipath][irg]
				     < vmaxs[irg])
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

	  /* Evaluate the FoV flags, removing points that are surrounded by */
	  /* data assigned to the opposite FoV.                             */
	  eval_fov_flag_consistency(max_rg, bmwidth, D_rgmax, D_nrg, E_rgmax,
				    E_nrg, F_rgmax, F_nrg, far_nrg, fovflg,
				    fovpast, fovbelong, scan_new);

	  /* Assign the appropriate virtual heights, regions, and elevation */
	  /* angles to each point based on their FoV.                       */
	  for(ibm = 0; ibm < scan_new->num; ibm++)
	    {
	      bm_new = scan_new->bm[ibm];

	      for(irg = 0; irg < bm_new->num; irg++)
		{
		  /* Remove or change the FoV of any points flagged as        */
		  /* outliers. Recall that in the last dimension of fovbelong */
		  /* the 0 index records the times flagged as an inlier, the  */
		  /* 1 index records the times flagged as an outlier, and the */
		  /* index 2 records the times with mixed FoVs.               */
		  if((fovbelong[ibm][irg][0] < fovbelong[ibm][irg][1]
		      + fovbelong[ibm][irg][2]) && fovbelong[ibm][irg][1] > 0)
		    {
		      /* This point is an outlier in a structure with the */
		      /* opposite FoV. If this point fit the criteria for */
		      /* a different FoV in the past, assign that FoV.    */
		      /* Otherwise remove any FoV assignment.             */
		      if((fovbelong[ibm][irg][1] > fovbelong[ibm][irg][2])
			 && (fovbelong[ibm][irg][1] > fovbelong[ibm][irg][0]))
			fovflg[ibm][irg] = fovpast[ibm][irg];
		      else fovflg[ibm][irg] = 0;

		      fovpast[ibm][irg] = 0;
		    }

		  /* Update the location values for this beam and range gate */
		  rng_flgs           = bm_new->rng_flgs;
		  rng_flgs->fov      = fovflg[ibm][irg];
		  rng_flgs->fov_past = fovpast[ibm][irg];
		}
	    }

	  /* Cycle to the next scan */
	  scan_new->next_scan = (struct RadarBSIDCycl *)
	    malloc(sizeof(struct RadarBSIDCycl));
	  prev_new            = new_scan;
	  scan_new            = scan_new->next_scan;
	  scan_new->prev_scan = prev_new;
	  scan_new->next_scan = (struct RadarBSIDCycl *)(NULL);
	  mult_bsid->num_scans++;
	}
    }

  /* Free the assigned memory */
  free(vmins);
  free(vmaxs);
  free(rng_flgs);
  free(prm);
  free(bm_old);
  free(bm_new);
  free(scan_old);
  free(scan_new);
  free(prev_new);

  for(ireg = 0; ireg < 3; ireg++)
    {
      out_num = (ireg == 0) ? D_rgmax : ((ireg == 1) ? E_rgmax : F_rgmax);

      for(ifov = 0; ifov < 2; ifov++)
	{
	  for(ipath = 0; ipath < max_path; ipath++)
	    {
	      free(scan_bm[ireg][ifov][ipath]);
	      free(scan_rg[ireg][ifov][ipath]);
	      free(scan_vh[ireg][ifov][ipath]);
	      free(scan_elv[ireg][ifov][ipath]);
	    }
	      
	  free(scan_num[ireg][ifov]);
	  free(scan_bm[ireg][ifov]);
	  free(scan_rg[ireg][ifov]);
	  free(scan_vh[ireg][ifov]);
	  free(scan_elv[ireg][ifov]);
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

  float max_std, max_score, intercept, sig_intercept, slope;
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


/**
 * @brief: Evaluate the FoV flags, removing and identifying outliers
 *
 * @params[in] max_rg    - Maximum number of range gates
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
 *             scan      - Scan structure holding data by beam and range gate
 *
 * @note `*_rgmax` parameters are equivalent to `rg_max` in davitpy
 **/

void eval_fov_flag_consistency(int max_rg, int bmwidth, int D_rgmax, int D_nrg,
			       int E_rgmax, int E_nrg, int F_rgmax, int F_nrg,
			       int far_nrg, int fovflg[][], int fovpast[][],
			       int fovbelong[][][], struct RadarBSIDCycl *scan)
{
  int irg, ibm, irsel, ibsel, rmin, rmax, bmin, bmax, width, fnum, bnum;
  int bad_fov;

  float ffrac, fov_frac, near_rg;

  struct RadarBSIDBeam *bm, *bm_ref;
  struct CellBSIDLoc *loc, *ref_loc;

  /* Initialize the variables */
  fov_frac = 2.0 / 3.0;
  near_rg  = -1.0;

  bm = (struct RadarBSIDBeam *)(malloc(sizeof(struct RadarBSIDBeam)));
  memset(bm, 0, sizeof(struct RadarBSIDBeam));
  bm_ref = (struct RadarBSIDBeam *)(malloc(sizeof(struct RadarBSIDBeam)));
  memset(bm_ref, 0, sizeof(struct RadarBSIDBeam));
  loc = (struct CellBSIDLoc *)(malloc(sizeof(struct CellBSIDLoc)));
  memset(loc, 0, sizeof(struct CellBSIDLoc));
  loc_ref = (struct CellBSIDLoc *)(malloc(sizeof(struct CellBSIDLoc)));
  memset(loc_ref, 0, sizeof(struct CellBSIDLoc));

  /* Cycle through all of the range gates, looking for consistency in the */
  /* neighboring backscatter fields of view.                              */
  for(rmin = 0, rmax = 0, irg = 0; irg < max_rg; irg++)
    {
      /* Get the half-width of the range gate box */
      if(irg < D_rgmax)      width = (D_nrg + RG_INC) / 2;
      else if(irg < E_rgmax) width = (E_nrg + RG_INC) / 2;
      else if(irg < F_rgmax) width = (F_nrg + RG_INC) / 2;
      else                   width = (far_nrg + RG_INC) / 2;

      /* Set the upper and lower box limits. This changes the logic    */
      /* from the davitpy implementation by allowing the limits of a   */
      /* range gate box to extend into range gates beyond their limit. */
      rmin = (irg - width < 0) ? 0 : irg - width;
      rmax = rmin + 2 * width;
      if(rmax > max_rg + 1) rmax = max_rg + 1;

      /* For each beam in the maximum possible range gate window, gather */
      /* the range gate, FoV flag, beam index, and range gate index by   */
      /* propagation path.                                               */
      for(ibm = 0; ibm < scan->num; ibm++)
	{
	  /* Cycle the reference beam and evaluation beam to the */
	  /* correct range gate                                  */
	  bm = scan->bm[ibm];

	  if(near_rg < 0.0)
	    {
	      /* Calculate once in the routine */
	      near_rg = ((500.0 / (5.0e-10 * C) - bm->prm.lagfr)
			 / bm->prm.smsep);
	    }
 
	  if(bm->sct[irg] == 1)
	    {
	      if(fovflg[ibm][irg] == -1) ref_loc = bm->back_loc[irg];
	      else                       ref_loc = bm->front_loc[irg];

	      /* Get the beam limits for the azimuthal box */
	      bmin = ibm - bmwidth;
	      bmax = ibm + bmwidth;
	      if(bmin < 0) bmin = 0;
	      if(bmax > scan->num) bmax = scan->num;

	      /* Cycle through each range gate and beam in the box, */
	      /* getting the number of points for the same hop in   */
	      /* each field-of-view                                 */
	      for(fnum = 0, bnum = 0, ibsel = bmin; ibsel < bmax; ibsel++)
		{
		  for(irsel = rmin; irsel < rmax; irsel++)
		    {
		      if(fovflg[ibsel][irsel] != 0)
			{
			  bm_ref = scan->bm[ibsel];
			  if(fovflg[ibsel][irsel] == -1)
			    loc = bm_ref->back_loc[irsel];
			  else bm_ref->front_loc[irsel];

			  if(loc.hop == ref_loc.hop)
			    {
			      if(fovflg[ibsel][irsel] == 1) fnum++;
			      else                          bnum++;
			    }
			}
		    }
		}

	      /* Sum up the number of points in this range gate/beam box */
	      /* and see if there is an overwhelming number of points in */
	      /* either field-of-view.                                   */
	      if(fnum + bnum > 0)
		{
		  ffrac = (float)fnum / (float)(fnum + bnum);
		  if(ffrac >= fov_frac && bnum > 0)
		    bad_fov = -1;
		  else if((1.0 - ffrac) >= fov_frac && fnum > 0):
		    bad_fov = 1;
		  else
		    bad_fov = 0;
		}

	      /* Tag all points whose FoV are or are not consistent with */
	      /* the observed structure at this propagation path.        */
	      for(ibsel = bmin; ibsel < bmax; ibsel++)
		{
		  for(irsel = rmin; irsel < rmax; irsel++)
		    {
		      if(fovflg[ibsel][irsel] != 0)
			{
			  bm_ref = scan->bm[ibsel];
			  if(fovflg[ibsel][irsel] == -1)
			    loc = bm_ref->back_loc[irsel];
			  else bm_ref->front_loc[irsel];

			  if(loc.hop == ref_loc.hop)
			    {
			      if(bad_fov == 0) fovbelong[ibsel][irsel][2] += 1;
			      else if(fovflg[ibsel][irsel] == bad_fov)
				{
				  /* If this point is not associated with a */
				  /* structure that is dominated by points  */
				  /* with the same FoV, and this is not the */
				  /* only FoV capapble of producing a       */
				  /* realistic elevation angle, flag this   */
				  /* point as an outlier.                   */
				  if(irsel < near_rg)
				    fovbelong[ibsel][irsel][1] += 1;

				}
			      else fovbelong[ibsel][irsel][0] += 1;
			    }
			}
		    }
		}
	    }
	}
    }

  /* Free the local pointers */
  free(bm);
  free(bm_ref);
  free(loc);
  free(loc_ref);

  return;
}
