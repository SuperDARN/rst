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
#include <math.h>

#include "rmath.h"
#include "radar.h"
#include "multscan.h"
#include "multbsid.h"

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
 *             F_rgmax    - Maximum range gate for near F-region box width (40)
 *             D_hmin     - Minimum h' for D-region in km (75)
 *             D_hmax     - Maximum h' for D-region in km (115)
 *             E_hmax     - Maximum h' for E-region in km (150)
 *             F_hmax     - Minimum h' for F-region in km (450)
 *             D_vh_box   - h' range for D-region box in km (40)
 *             E_vh_box   - h' range for E-region box in km (35)
 *             F_vh_box   - h' range for near F-region box in km (50)
 *             far_vh_box - h' range for far F-region box in km (150)
 *             max_hop    - maximum number of hops to consider (3.0)
 *             scan       - Input radar scan
 *             hard       - Radar site information from hardware file
 *
 * @params[out] mult_bsid - Output scans with location and scan-assingned FoVs
 **/

void UpdateScanBSFoV(short int strict_gs, int freq_min, int freq_max,
		     int min_pnts, int D_nrg, int E_nrg, int F_nrg, int far_nrg,
		     int D_rgmax, int E_rgmax, int F_rgmax, float D_hmin,
		     float D_hmax, float E_hmax, float F_hmax, float D_vh_box,
		     float E_vh_box, float F_vh_box, float far_vh_box,
		     float max_hop, struct RadarScan *scan,
		     struct RadarSite *hard, struct MultFitBSID *mult_bsid)
{
  int iscan, ibm, irg, ifov, ipath, ireg, ivh, igbm, cpid, scan_chan;
  int max_rg, max_path, out_num, group_num, bm_num, bmwidth, igood_num;
  int igood[MAX_BMS], group_bm[MAX_BMS * MAX_RGS], group_rg[MAX_BMS * MAX_RGS];
  int fovflg[MAX_BMS][MAX_RGS], fovpast[MAX_BMS][MAX_RGS];
  int front_num[MAX_BMS][MAX_RGS], back_num[MAX_BMS][MAX_RGS];
  int fovbelong[MAX_BMS][MAX_RGS][3];
  int ***scan_num, ****scan_bm, ****scan_rg;

  float hmin, hmax, hbox, *vmins, *vmaxs;
  float group_elv[MAX_BMS * MAX_RGS], group_vh[MAX_BMS * MAX_RGS];
  float ****scan_vh, ****scan_elv;

  struct CellBSIDFlgs *rng_flgs;
  struct CellBSIDLoc loc;
  struct FitElv elv;
  struct RadarBeam bm_old;
  struct FitBSIDBeam bm_new;
  struct FitBSIDScan *scan_new, *prev_new;

  void UpdateBeamFit(short int strict_gs, float max_hop, float D_hmin,
		     float D_hmax, float E_hmax, float F_hmax,
		     struct RadarSite *site, struct FitBSIDBeam *beam);
  int select_alt_groups(int num, int *rg, float *vh, float vh_min, float vh_max,
			float vh_box, int min_pnts, float *vh_mins,
			float *vh_maxs);
  int num_unique_int_vals(int num, int array[]);
  void eval_az_var_in_elv(int num, int fov, int scan_bm[], int scan_rg[],
			  int fovflg[MAX_BMS][MAX_RGS],
			  int fovpast[MAX_BMS][MAX_RGS], float scan_vh[],
			  float scan_elv[]);
  void eval_fov_flag_consistency(int max_rg, int bmwidth, int D_rgmax,
				 int D_nrg, int E_rgmax, int E_nrg,
				 int F_rgmax, int F_nrg, int far_nrg,
				 int fovflg[MAX_BMS][MAX_RGS],
				 int fovpast[MAX_BMS][MAX_RGS],
				 int fovbelong[MAX_BMS][MAX_RGS][3],
				 struct FitBSIDScan *scan);

  /* Initialize the local pointers and variables */
  bmwidth = (int)((float)min_pnts * 0.75);

  rng_flgs = (struct CellBSIDFlgs *)(malloc(sizeof(struct CellBSIDFlgs)));
  memset(rng_flgs, 0, sizeof(struct CellBSIDFlgs));

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
  scan_num = (int ***)(NULL);
  scan_bm  = (int ****)(NULL);
  scan_rg  = (int ****)(NULL);
  scan_vh  = (float ****)(NULL);
  scan_elv = (float ****)(NULL);

  out_num = (F_rgmax > E_rgmax) ? F_rgmax : E_rgmax;
  if(D_rgmax > out_num) out_num = D_rgmax;
  vmins = (float *)calloc(out_num, sizeof(float));
  vmaxs = (float *)calloc(out_num, sizeof(float));

  /* Inititalize the output scan */
  scan_new = (struct FitBSIDScan *)malloc(sizeof(struct FitBSIDScan));

  if(mult_bsid->num_scans == 0)
    {
      mult_bsid->scan_ptr      = scan_new;
      mult_bsid->stid          = scan->stid;
      mult_bsid->st_time       = scan->st_time;
      mult_bsid->ed_time       = scan->ed_time;
      mult_bsid->version.major = scan->version.major;
      mult_bsid->version.minor = scan->version.minor;
      prev_new                 = (struct FitBSIDScan *)(NULL);
    }
  else
    {
      prev_new            = mult_bsid->last_ptr;
      prev_new->next_scan = scan_new;
      scan_new->prev_scan = prev_new;
      scan_new->next_scan = (struct FitBSIDScan *)(NULL);
      mult_bsid->ed_time  = scan->ed_time;
    }
  mult_bsid->last_ptr = scan_new;

  /* Before initializing this new scan data, make sure the frequency for the */
  /* beams are correct. Also test to ensure constant CPID and channel.       */
  for(igood_num = 0, ibm = 0; ibm < scan->num; ibm++)
    {
      bm_old = scan->bm[ibm];

      if(ibm == 0)
	{
	  cpid      = bm_old.cpid;
	  scan_chan = bm_old.channel;
	}

      if(bm_old.freq >= freq_min && bm_old.freq < freq_max)
	{
	  /* Only verify CPID and channel for requested frequency range */
	  if(cpid != bm_old.cpid)
	    {
	      fprintf(stderr,
		      "CP changes in scan, try limiting channels [%d != %d]\n",
		      cpid, bm_old.cpid);
	      exit(1);
	    }
	  if(scan_chan != bm_old.channel)
	    {
	      fprintf(stderr,
		      "channel changes in scan, try -cn option [%d != %d]\n",
		      scan_chan, bm_old.channel);
	      exit(1);
	    }

	  igood[igood_num] = ibm;
	  igood_num++;
	}
    }

  /* If there are good beams in this scan, add it to the output structure */
  if(igood_num > 0)
    {
      scan_new->st_time = scan->st_time;
      scan_new->ed_time = scan->ed_time;
      scan_new->num_bms = scan->num;
      scan_new->bm = (struct FitBSIDBeam *)
	malloc(sizeof(struct FitBSIDBeam) * scan_new->num_bms);
      memset(scan_new->bm, 0, sizeof(struct FitBSIDBeam) * scan_new->num_bms);

      max_rg = 0;

      /* Cycle through each beam in the scan, but only update the beams */
      /* with appropriate frequencies.                                  */
      for(igbm = 0, ibm = 0; ibm < scan_new->num_bms; ibm++)
	{
	  if(igood[igbm] == ibm)
	    {
	      igbm++;
	      bm_old = scan->bm[ibm];
	      bm_new = scan_new->bm[ibm];

	      /* This corresponds to 
	       * davitpy.pydarn.proc.fov.update_backscatter.update_beam_fit
	       *
	       * Start by initializing new beams in the new scan */
	      bm_new.cpid    = bm_old.cpid;
	      bm_new.bm      = bm_old.bm;
	      bm_new.bmazm   = bm_old.bmazm;
	      bm_new.time    = bm_old.time;
	      bm_new.intt.sc = bm_old.intt.sc;
	      bm_new.intt.us = bm_old.intt.us;

	      /* Set the beam parameter values */
	      bm_new.nave    = bm_old.nave;
	      bm_new.frang   = bm_old.frang;
	      bm_new.rsep    = bm_old.rsep;
	      bm_new.rxrise  = bm_old.rxrise;
	      bm_new.freq    = bm_old.freq;
	      bm_new.noise   = bm_old.noise;
	      bm_new.atten   = bm_old.atten;
	      bm_new.channel = bm_old.channel;

	      /* Initialize the range-dependent variables */
	      bm_new.nrang     = bm_old.nrang;
	      bm_new.rng       = (struct RadarCell *)
		calloc(bm_new.nrang, sizeof(struct RadarCell));
	      bm_new.med_rng   = (struct RadarCell *)
		calloc(bm_new.nrang, sizeof(struct RadarCell));
	      bm_new.rng_flgs  = (struct CellBSIDFlgs *)
		malloc(bm_new.nrang * sizeof(struct CellBSIDFlgs));
	      bm_new.front_loc = (struct CellBSIDLoc *)
		malloc(bm_new.nrang * sizeof(struct CellBSIDLoc));
	      bm_new.back_loc  = (struct CellBSIDLoc *)
		malloc(bm_new.nrang * sizeof(struct CellBSIDLoc));
	      bm_new.front_elv = (struct FitElv *)
		calloc(bm_new.nrang, sizeof(struct FitElv));
	      bm_new.back_elv  = (struct FitElv *)
		calloc(bm_new.nrang, sizeof(struct FitElv));
	      if(bm_old.sct == NULL) bm_new.sct = (unsigned char *)(NULL);
	      else bm_new.sct = (unsigned char *)
		     calloc(bm_new.nrang, sizeof(char));

	      if(bm_new.nrang > max_rg) max_rg = bm_new.nrang;

	      for(irg = 0; irg < bm_new.nrang; irg++)
		{
		  bm_new.sct[irg] = bm_old.sct[irg];

		  /* Load only for range gates with data */
		  if(bm_old.sct[irg] == 1)
		    {
		      bm_new.rng[irg].gsct   = bm_old.rng[irg].gsct;
		      bm_new.rng[irg].p_0    = bm_old.rng[irg].p_0;
		      bm_new.rng[irg].p_0_e  = bm_old.rng[irg].p_0_e;
		      bm_new.rng[irg].v      = bm_old.rng[irg].v;
		      bm_new.rng[irg].v_e    = bm_old.rng[irg].v_e;
		      bm_new.rng[irg].w_l    = bm_old.rng[irg].w_l;
		      bm_new.rng[irg].w_l_e  = bm_old.rng[irg].w_l_e;
		      bm_new.rng[irg].p_l    = bm_old.rng[irg].p_l;
		      bm_new.rng[irg].p_l_e  = bm_old.rng[irg].p_l_e;
		      bm_new.rng[irg].phi0   = bm_old.rng[irg].phi0;
		      bm_new.rng[irg].phi0_e = bm_old.rng[irg].phi0_e;
		      bm_new.rng[irg].elv    = bm_old.rng[irg].elv;
		    }
		}

	      /* Update the front and back FoVs */
	      UpdateBeamFit(strict_gs, max_hop, D_hmin, D_hmax, E_hmax,
			    F_hmax, hard, &bm_new);

	      /* Find the scan's altitude bins by FoV, region, and path */
	      if(scan_num == NULL)
		{
		  scan_num = (int ***)malloc(3 * sizeof(int **));
		  scan_bm  = (int ****)malloc(3 * sizeof(int ***));
		  scan_rg  = (int ****)malloc(3 * sizeof(int ***));
		  scan_vh  = (float ****)malloc(3 * sizeof(float ***));
		  scan_elv = (float ****)malloc(3 * sizeof(float ***));

		  for(ireg = 0; ireg < 3; ireg++)
		    {
		      scan_num[ireg] = (int **)malloc(2 * sizeof(int *));
		      scan_bm[ireg]  = (int ***)malloc(2 * sizeof(int **));
		      scan_rg[ireg]  = (int ***)malloc(2 * sizeof(int **));
		      scan_vh[ireg]  = (float ***)malloc(2 * sizeof(float **));
		      scan_elv[ireg] = (float ***)malloc(2 * sizeof(float **));

		      if(ireg == 0) out_num = D_rgmax;
		      else out_num = (ireg == 1) ? E_rgmax : F_rgmax;

		      for(ifov = 0; ifov < 2; ifov++)
			{
			  scan_num[ireg][ifov] = (int *)
			    malloc(max_path * sizeof(int));
			  scan_bm[ireg][ifov]  = (int **)
			    malloc(max_path * sizeof(int *));
			  scan_rg[ireg][ifov]  = (int **)
			    malloc(max_path * sizeof(int *));
			  scan_vh[ireg][ifov]  = (float **)
			    malloc(max_path * sizeof(float *));
			  scan_elv[ireg][ifov] = (float **)
			    malloc(max_path * sizeof(float *));

			  for(ipath = 0; ipath < max_path; ipath++)
			    {
			      scan_num[ireg][ifov][ipath] = 0;
			      scan_bm[ireg][ifov][ipath] = (int *)
				malloc(out_num * sizeof(int));
			      scan_rg[ireg][ifov][ipath] = (int *)
				malloc(out_num * sizeof(int));
			      scan_vh[ireg][ifov][ipath] = (float *)
				malloc(out_num * sizeof(float));
			      scan_elv[ireg][ifov][ipath] = (float *)
				malloc(out_num * sizeof(float));
			    }
			}
		    }
		}
	      else
		{
		  for(ifov = 0; ifov < 2; ifov++)
		    {
		      for(ipath = 0; ipath < max_path; ipath++)
			{
			  for(ireg = 0; ireg < 3; ireg++)
			    scan_num[ireg][ifov][ipath] = 0;
			}
		    }
		}

	      for(irg = 0; irg < bm_new.nrang; irg++)
		{
		  if(bm_new.sct[irg] == 1)
		    {
		      /* Assign this data to the correct region list for each */
		      /* field of view (ifov is 0 for back and 1 for front)   */
		      for(ifov = 0; ifov < 2; ifov++)
			{
			  if(ifov == 0)
			    {
			      loc = bm_new.back_loc[irg];
			      elv = bm_new.back_elv[irg];
			    }
			  else
			    {
			      loc = bm_new.front_loc[irg];
			      elv = bm_new.front_elv[irg];
			    }

			  ipath = (int)(loc.hop * 2.0);
			  if(strstr(loc.region, "D") != NULL) ireg = 0;
			  else if(strstr(loc.region, "E") != NULL) ireg = 1;
			  else if(strstr(loc.region, "F") != NULL) ireg = 2;
			  else ireg = -1;

			  if(ireg >= 0)
			    {
			      scan_bm[ireg][ifov][ipath][scan_num[ireg][ifov][ipath]] = bm_new.bm;
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
	      hmin = E_hmax;
	      hmax = E_hmax;
	      hbox = E_vh_box;
	    }
	  else if(ireg == 2)
	    {
	      hmin = E_hmax;
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

		      /* For each virtual height bin, determine if this */
		      /* FoV has a realistic azimuth variation.         */
		      for(ivh = 0; ivh < out_num; ivh++)
			{
			  for(group_num = 0, irg = 0;
			      irg < scan_num[ireg][ifov][ipath]; irg++)
			    {
			      if(scan_vh[ireg][ifov][ipath][irg] >= vmins[ivh]
				 && scan_vh[ireg][ifov][ipath][irg]
				 < vmaxs[ivh])
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
				eval_az_var_in_elv(group_num, ifov, group_bm,
						   group_rg, fovflg, fovpast,
						   group_vh, group_elv);
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
      for(ibm = 0; ibm < scan_new->num_bms; ibm++)
	{
	  bm_new = scan_new->bm[ibm];

	  for(irg = 0; irg < bm_new.nrang; irg++)
	    {
	      /* Remove or change the FoV of any points flagged as outliers. */
	      /* Recall that in the last dimension of fovbelong the 0 index  */
	      /* records the times flagged as an inlier, the 1 index records */
	      /* the times flagged as an outlier, and the index 2 records    */
	      /* the times with mixed FoVs.                                  */
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
	      rng_flgs           = bm_new.rng_flgs;
	      rng_flgs->fov      = fovflg[ibm][irg];
	      rng_flgs->fov_past = fovpast[ibm][irg];
	    }

	  /* Cycle to the next scan */
	  scan_new->next_scan = (struct FitBSIDScan *)
	    malloc(sizeof(struct FitBSIDScan));
	  prev_new            = scan_new;
	  scan_new            = scan_new->next_scan;
	  scan_new->prev_scan = prev_new;
	  mult_bsid->last_ptr = prev_new;
	  scan_new->next_scan = (struct FitBSIDScan *)(NULL);
	  mult_bsid->num_scans++;
	}

      /* Close the scan structure */
      prev_new->next_scan = (struct FitBSIDScan *)(NULL);
      scan_new            = prev_new->next_scan;
    }

  /* Free the assigned memory */
  free(vmins);
  free(vmaxs);
  free(rng_flgs);
  free(scan_new);
  if(prev_new != (struct FitBSIDScan *)(NULL)) free(prev_new);

  if(scan_num != (int ***)(NULL))
    {
      for(ireg = 0; ireg < 3; ireg++)
	{
	  for(ifov = 0; ifov < 2; ifov++)
	    {
	      for(ipath = 0; ipath < max_path; ipath++)
		{
		  free(scan_bm[ireg][ifov][ipath]);
		  free(scan_rg[ireg][ifov][ipath]);
		  free(scan_vh[ireg][ifov][ipath]);
		  free(scan_elv[ireg][ifov][ipath]);
		}
	    }
	}

      for(ireg = 0; ireg < 3; ireg++)
	{
	  for(ifov = 0; ifov < 2; ifov++)
	    {
	      free(scan_num[ireg][ifov]);
	      free(scan_bm[ireg][ifov]);
	      free(scan_rg[ireg][ifov]);
	      free(scan_vh[ireg][ifov]);
	      free(scan_elv[ireg][ifov]);
	    }
	}

      for(ireg = 0; ireg < 3; ireg++)
	{
	  free(scan_num[ireg]);
	  free(scan_bm[ireg]);
	  free(scan_rg[ireg]);
	  free(scan_vh[ireg]);
	  free(scan_elv[ireg]);
	}

      free(scan_num);
      free(scan_bm);
      free(scan_rg);
      free(scan_vh);
      free(scan_elv);
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
 **/

void eval_az_var_in_elv(int num, int fov, int scan_bm[], int scan_rg[],
			int fovflg[MAX_BMS][MAX_RGS],
			int fovpast[MAX_BMS][MAX_RGS], float scan_vh[],
			float scan_elv[])
{
  int i, irg, ibm, reg_stat;
  int get_fov[2] = {-1, 1};

  float max_std, max_score, intercept, sig_intercept, slope, chi2, q, lstd;
  float sig_slope, line_std;
  float fovstd[MAX_BMS][MAX_RGS], fovscore[MAX_BMS][MAX_RGS];
  float *lval, *ldev, *abs_ldev, *lscore, *line_dev, *sig;

  int linear_regression(float x[], float y[], float sig[], int num, int mwt,
			float *intercept, float *sig_intercept, float *slope,
			float *sig_slope, float *chi2, float *q);
  void zscore(int num, float array[], float *zscore);
  float stdev_float(int num, float array[]);

  /* Initalize the maximum statistics and reference variables */
  max_std   = 3.0;
  max_score = 3.0;

  for(ibm = 0; ibm < MAX_BMS; ibm++)
    {
      for(irg = 0; irg < MAX_RGS; irg++)
	{
	  fovstd[ibm][irg] = 0.0;
	  fovscore[ibm][irg] = 0.0;
	}
    }
  
  /* Initialize the evalutaion statistics to default values */
  line_std = max_std + 100.0;
  line_dev = (float *)calloc(num, sizeof(float));
  sig      = (float *)calloc(num, sizeof(float));
  lval     = (float *)calloc(num, sizeof(float));
  ldev     = (float *)calloc(num, sizeof(float));
  abs_ldev = (float *)calloc(num, sizeof(float));
  lscore   = (float *)calloc(num, sizeof(float));
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
			       int far_nrg, int fovflg[MAX_BMS][MAX_RGS],
			       int fovpast[MAX_BMS][MAX_RGS],
			       int fovbelong[MAX_BMS][MAX_RGS][3],
			       struct FitBSIDScan *scan)
{
  int irg, ibm, irsel, ibsel, rmin, rmax, bmin, bmax, width, fnum, bnum;
  int bad_fov;

  float ffrac, fov_frac, near_rg;

  struct FitBSIDBeam bm, bm_ref;
  struct CellBSIDLoc loc, ref_loc;

  /* Initialize the variables */
  fov_frac = 2.0 / 3.0;
  near_rg  = -1.0;

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
 
	      if(bm.sct[irg] == 1)
		{
		  if(fovflg[ibm][irg] == -1) ref_loc = bm.back_loc[irg];
		  else                       ref_loc = bm.front_loc[irg];

		  /* Get the beam limits for the azimuthal box */
		  bmin = ibm - bmwidth;
		  bmax = ibm + bmwidth;
		  if(bmin < 0) bmin = 0;
		  if(bmax > scan->num_bms) bmax = scan->num_bms;

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
				loc = bm_ref.back_loc[irsel];
			      else loc = bm_ref.front_loc[irsel];

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
		      else if((1.0 - ffrac) >= fov_frac && fnum > 0)
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
				loc = bm_ref.back_loc[irsel];
			      else loc = bm_ref.front_loc[irsel];

			      if(loc.hop == ref_loc.hop)
				{
				  if(bad_fov == 0)
				    fovbelong[ibsel][irsel][2] += 1;
				  else if(fovflg[ibsel][irsel] == bad_fov)
				    {
				      /* If this point is not associated with */
				      /* a structure that is dominated by     */
				      /* points with the same FoV, and this   */
				      /* is not the only FoV capapble of      */
				      /* producing arealistic elevation       */
				      /* angle, flag this point as an outlier */
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
    }
  return;
}
