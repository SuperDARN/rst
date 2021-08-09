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

#include "rmath.h"
#include "radar.h"
#include "multbsid.h"
#include "scan_utils.h"

#ifndef MAX_BMS
#define MAX_BMS 25  /* Current maximum in hdw files */
#endif

#ifndef MAX_RGS
#define MAX_RGS 225  /* Current maximum in hdw files */
#endif

#ifndef MIN_BMS
#define MIN_BMS 3  /* Minimum number of beams to find azimuthal variations */
#endif

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
 *             D_hmax     - Maximum h' for D-region in km (100)
 *             E_hmax     - Maximum h' for E-region in km (120)
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
  int iscan, ibm, irg, ifov, ipath, ireg, ivh, igbm, cpid, scan_chan, max_vbin;
  int bind, max_rg, max_path, out_num, group_num, bm_num, bmwidth, igood_num;
  int igood[MAX_BMS], bgood[MAX_BMS];
  int group_bm[MAX_BMS * MAX_RGS], group_rg[MAX_BMS * MAX_RGS];
  int fovflg[MAX_BMS][MAX_RGS], fovpast[MAX_BMS][MAX_RGS];
  int front_num[MAX_BMS][MAX_RGS], back_num[MAX_BMS][MAX_RGS];
  int fovbelong[MAX_BMS][MAX_RGS][3], opp_in[MAX_BMS][MAX_RGS];
  int ***scan_num, ****scan_bm, ****scan_rg;

  float hmin, hmax, hbox, *vmins, *vmaxs;
  float group_elv[MAX_BMS * MAX_RGS], group_vh[MAX_BMS * MAX_RGS];
  float fovstd[MAX_BMS][MAX_RGS], fovscore[MAX_BMS][MAX_RGS];
  float ****scan_vh, ****scan_elv;

  struct CellBSIDLoc loc;
  struct FitElv elv;
  struct RadarBeam bm_old;
  struct FitBSIDBeam *bm_new;
  struct FitBSIDScan *scan_new, *prev_new;

  void UpdateBeamFit(short int strict_gs, float max_hop, float D_hmin,
		     float D_hmax, float E_hmax, float F_hmax,
		     struct RadarSite *site, struct FitBSIDBeam *beam);
  int select_alt_groups(int num, int *rg, float *vh, float vh_min, float vh_max,
			float vh_box, int min_pnts, int max_vbin,
			float *vh_mins, float *vh_maxs);

  /* Test to ensure the local definitions agree with the hardware file */
  if(MAX_BMS < hard->maxbeam || MAX_RGS < hard->maxrange)
    {
      fprintf(stderr, "local definitions of max range gates or beams is ");
      fprintf(stderr, "too small [%d > %d or %d > %d]\n", MAX_BMS,
	      hard->maxbeam, MAX_RGS, hard->maxrange);
      exit(1);
    }

  /* Initialize the local pointers and variables */
  bmwidth = (int)((float)min_pnts * 0.75);

  for(ibm = 0; ibm < MAX_BMS; ibm++)
    {
      igood[ibm] = -1;
      bgood[ibm] = -1;

      for(irg = 0; irg < MAX_RGS; irg++)
	{
	  fovflg[ibm][irg]    = 0;
	  fovpast[ibm][irg]   = 0;
	  fovstd[ibm][irg]    = 0.0;
	  fovscore[ibm][irg]  = 0.0;
	  front_num[ibm][irg] = 0;
	  back_num[ibm][irg]  = 0;
	  opp_in[ibm][irg]    = 0;

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

  max_vbin = (F_rgmax > E_rgmax) ? F_rgmax : E_rgmax;
  if(D_rgmax > max_vbin) max_vbin = D_rgmax;
  vmins = (float *)calloc(max_vbin, sizeof(float));
  vmaxs = (float *)calloc(max_vbin, sizeof(float));

  /* Before initializing this new scan data, make sure the frequency for the */
  /* beams are correct. Also test to ensure constant CPID and channel.       */
  for(igood_num = 0, ibm = 0; ibm < scan->num && igood_num >= 0; ibm++)
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

	  /* If there are too many good beams in this scan, exit */
	  if(igood_num >= hard->maxbeam)
	    {
	      fprintf(stderr, "too many beams in scan [%d-%d kHZ, %d CPID]\n",
		      freq_min, freq_max, cpid);
	      igood_num = -1;
	      break;
	    }
	  else
	    {
	      igood[igood_num] = ibm;
	      bgood[igood_num] = bm_old.bm;
	      igood_num++;
	    }
	}
    }

  /* If there are good beams in this scan, add it to the output structure. */
  /* If there are too many good beams, this is a beam-switching mode and   */
  /* we can't currently process it.                                        */
  if(igood_num >= MIN_BMS && igood_num <= hard->maxbeam)
    {
      /* Test to see that there are no duplicate beams. This is something */
      /* that may be possible to process, but isn't at the moment.        */
      if(num_unique_int_vals(igood_num, bgood) != igood_num)
	{
	  fprintf(stderr,
		  "duplicate beams in scan, requires different FoV method\n");
	}
      else
	{
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

	  scan_new->st_time = scan->st_time;
	  scan_new->ed_time = scan->ed_time;
	  scan_new->num_bms = scan->num;
	  scan_new->bm = (struct FitBSIDBeam *)
	    malloc(sizeof(struct FitBSIDBeam) * scan_new->num_bms);
	  memset(scan_new->bm, 0, sizeof(struct FitBSIDBeam)
		 * scan_new->num_bms);

	  max_rg = 0;

	  /* Cycle through each beam in the scan, but only update the beams */
	  /* with appropriate frequencies.                                  */
	  for(igbm = 0, ibm = 0; ibm < scan_new->num_bms; ibm++)
	    {
	      if(igood[igbm] == ibm)
		{
		  igbm++;
		  bm_old = scan->bm[ibm];
		  bm_new = &scan_new->bm[ibm];

		  /* This corresponds to 
		   * davitpy.pydarn.proc.fov.update_backscatter.update_beam_fit
		   *
		   * Start by initializing new beams in the new scan */
		  bm_new->cpid    = bm_old.cpid;
		  bm_new->bm      = bm_old.bm;
		  bm_new->bmazm   = bm_old.bmazm;
		  bm_new->time    = bm_old.time;
		  bm_new->intt.sc = bm_old.intt.sc;
		  bm_new->intt.us = bm_old.intt.us;

		  /* Set the beam parameter values */
		  bm_new->nave    = bm_old.nave;
		  bm_new->frang   = bm_old.frang;
		  bm_new->rsep    = bm_old.rsep;
		  bm_new->rxrise  = bm_old.rxrise;
		  bm_new->freq    = bm_old.freq;
		  bm_new->noise   = bm_old.noise;
		  bm_new->atten   = bm_old.atten;
		  bm_new->channel = bm_old.channel;

		  /* Initialize the range-dependent variables */
		  bm_new->nrang     = bm_old.nrang;
		  bm_new->rng       = (struct RadarCell *)
		    calloc(bm_new->nrang, sizeof(struct RadarCell));
		  bm_new->med_rng   = (struct RadarCell *)
		    calloc(bm_new->nrang, sizeof(struct RadarCell));
		  bm_new->rng_flgs  = (struct CellBSIDFlgs *)
		    malloc(bm_new->nrang * sizeof(struct CellBSIDFlgs));
		  bm_new->front_loc = (struct CellBSIDLoc *)
		    malloc(bm_new->nrang * sizeof(struct CellBSIDLoc));
		  bm_new->back_loc  = (struct CellBSIDLoc *)
		    malloc(bm_new->nrang * sizeof(struct CellBSIDLoc));
		  bm_new->front_elv = (struct FitElv *)
		    calloc(bm_new->nrang, sizeof(struct FitElv));
		  bm_new->back_elv  = (struct FitElv *)
		    calloc(bm_new->nrang, sizeof(struct FitElv));
		  if(bm_old.sct == NULL) bm_new->sct = (unsigned char *)(NULL);
		  else bm_new->sct = (unsigned char *)
			 calloc(bm_new->nrang, sizeof(char));

		  if(bm_new->nrang > max_rg) max_rg = bm_new->nrang;

		  for(irg = 0; irg < bm_new->nrang; irg++)
		    {
		      bm_new->sct[irg] = bm_old.sct[irg];

		      /* Load only for range gates with data */
		      if(bm_old.sct[irg] == 1)
			{
			  bm_new->rng[irg].gsct   = bm_old.rng[irg].gsct;
			  bm_new->rng[irg].p_0    = bm_old.rng[irg].p_0;
			  bm_new->rng[irg].p_0_e  = bm_old.rng[irg].p_0_e;
			  bm_new->rng[irg].v      = bm_old.rng[irg].v;
			  bm_new->rng[irg].v_e    = bm_old.rng[irg].v_e;
			  bm_new->rng[irg].w_l    = bm_old.rng[irg].w_l;
			  bm_new->rng[irg].w_l_e  = bm_old.rng[irg].w_l_e;
			  bm_new->rng[irg].p_l    = bm_old.rng[irg].p_l;
			  bm_new->rng[irg].p_l_e  = bm_old.rng[irg].p_l_e;
			  bm_new->rng[irg].phi0   = bm_old.rng[irg].phi0;
			  bm_new->rng[irg].phi0_e = bm_old.rng[irg].phi0_e;
			  bm_new->rng[irg].elv    = bm_old.rng[irg].elv;

			  /* Initialize the range gate flags */
			  bm_new->rng_flgs[irg].fov      = 0;
			  bm_new->rng_flgs[irg].fov_past = 0;
			  bm_new->rng_flgs[irg].grpflg   = 0;
			  bm_new->rng_flgs[irg].grpnum   = 0;
			  sprintf(bm_new->rng_flgs[irg].grpid, "UNSET");
			}
		    }

		  /* Update the front and back FoVs */
		  UpdateBeamFit(strict_gs, max_hop, D_hmin, D_hmax, E_hmax,
				F_hmax, hard, bm_new);

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
			  scan_vh[ireg]  = (float ***)
			    malloc(2 * sizeof(float **));
			  scan_elv[ireg] = (float ***)
			    malloc(2 * sizeof(float **));

			  if(ireg == 0) out_num = D_rgmax * scan->num;
			  else if(irg == 1) out_num = E_rgmax * scan->num;
			  else out_num = F_rgmax * scan->num;

			  for(ifov = 0; ifov < 2; ifov++)
			    {
			      scan_num[ireg][ifov] = (int *)
				calloc(max_path, sizeof(int));
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
				  scan_bm[ireg][ifov][ipath] = (int *)
				    calloc(out_num, sizeof(int));
				  scan_rg[ireg][ifov][ipath] = (int *)
				    calloc(out_num, sizeof(int));
				  scan_vh[ireg][ifov][ipath] = (float *)
				    calloc(out_num, sizeof(float));
				  scan_elv[ireg][ifov][ipath] = (float *)
				    calloc(out_num, sizeof(float));
				}
			    }
			}
		    }

		  for(irg = 0; irg < bm_new->nrang; irg++)
		    {
		      if(bm_new->sct[irg] == 1)
			{
			  /* Assign this data to the correct region list for */
			  /* each FoV (ifov is 0 for back and 1 for front)   */
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
			      if(strstr(loc.region, "D") != NULL)      ireg = 0;
			      else if(strstr(loc.region, "E") != NULL) ireg = 1;
			      else if(strstr(loc.region, "F") != NULL) ireg = 2;
			      else ireg = -1;

			      if(ireg >= 0)
				{
				  if(ipath == 0)
				    {
				      fprintf(stderr, "bad hop encountered at");
				      fprintf(stderr, " beam [%d], range", ibm);
				      fprintf(stderr, " gate [%d], FoV [", irg);
				      fprintf(stderr, "%s], time %f\n",
					      (ifov == 0) ? "back" : "front",
					      scan_new->st_time);
				      exit(1);
				    }

				  scan_bm[ireg][ifov][ipath][scan_num[ireg][ifov][ipath]] = bm_new->bm;
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
		  hmin = D_hmax;
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
		  for(ipath = 1; ipath < max_path; ipath++)
		    {
		      if(ireg == 2 && ipath >= 3) hbox = far_vh_box;

		      if(scan_num[ireg][ifov][ipath] >= min_pnts)
			{
			  /* Use select_alt_groups */
			  out_num = select_alt_groups(scan_num[ireg][ifov][ipath], scan_rg[ireg][ifov][ipath], scan_vh[ireg][ifov][ipath], hmin, hmax, hbox, min_pnts, max_vbin, vmins, vmaxs);

			  /* For each virtual height bin, determine if this */
			  /* FoV has a realistic azimuth variation.         */
			  for(ivh = 0; ivh < out_num; ivh++)
			    {
			      for(group_num = 0, irg = 0;
				  irg < scan_num[ireg][ifov][ipath]; irg++)
				{
				  if(scan_vh[ireg][ifov][ipath][irg]
				     >= vmins[ivh]
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
				  bm_num = num_unique_int_vals(group_num,
							       group_bm);

				  if(bm_num >= MIN_BMS)
				    eval_az_var_in_elv(group_num, ifov,
						       group_bm, group_rg,
						       fovflg, fovpast,
						       group_vh, group_elv,
						       fovstd, fovscore);
				}
			    }
			}
		    }
		}
	    }

	  /* Evaluate the FoV flags, removing points that are surrounded by */
	  /* data assigned to the opposite FoV.                             */
	  eval_fov_flag_consistency(max_rg, hard->maxbeam, bmwidth, D_rgmax,
				    D_nrg, E_rgmax, E_nrg, F_rgmax, F_nrg,
				    far_nrg, fovflg, fovpast, fovbelong,
				    opp_in, scan_new);

	  /* Assign the appropriate virtual heights, regions, and elevation */
	  /* angles to each point based on their FoV.                       */
	  for(ibm = 0; ibm < scan_new->num_bms; ibm++)
	    {
	      bm_new = &scan_new->bm[ibm];
	      bind   = bm_new->bm;

	      for(irg = 0; irg < bm_new->nrang; irg++)
		{
		  /* Remove or change the FoV of any points flagged as        */
		  /* outliers. Recall that in the last dimension of fovbelong */
		  /* the 0 index records the times flagged as an inlier, the  */
		  /* 1 index records the times flagged as an outlier, and     */
		  /* the index 2 records the times with mixed FoVs.           */
		  if((fovbelong[bind][irg][0] < fovbelong[bind][irg][1]
		      + fovbelong[bind][irg][2]) && fovbelong[bind][irg][1] > 0)
		    {
		      /* This point is an outlier in a structure with the */
		      /* opposite FoV. If this point fit the criteria for */
		      /* a different FoV in the past, assign that FoV.    */
		      /* Otherwise remove any FoV assignment.             */
		      if((fovbelong[bind][irg][1] > fovbelong[bind][irg][2])
			 && (fovbelong[bind][irg][1] > fovbelong[bind][irg][0]))
			fovflg[bind][irg] = fovpast[bind][irg];
		      else
			fovflg[bind][irg] = 0;

		      /* Clear the past (former present), since it proved to */
		      /* be unrealistic                                      */
		      fovpast[bind][irg] = 0;
		    }
		  else if(opp_in[bind][irg] == 1)
		    {
		      /* This point is a better inlier when swapped */
		      fovflg[bind][irg]   = fovpast[bind][irg];
		      fovpast[bind][irg] *= -1;
		    }

		  /* Update the location values for this beam and range gate */
		  bm_new->rng_flgs[irg].fov      = fovflg[bind][irg];
		  bm_new->rng_flgs[irg].fov_past = fovpast[bind][irg];	      
		}
	    }

	  /* Cycle to the next scan, if a new scan was loaded */
	  scan_new->next_scan = (struct FitBSIDScan *)
	    malloc(sizeof(struct FitBSIDScan));
	  prev_new            = scan_new;
	  scan_new            = scan_new->next_scan;
	  scan_new->prev_scan = prev_new;
	  mult_bsid->last_ptr = prev_new;
	  scan_new->next_scan = (struct FitBSIDScan *)(NULL);
	  mult_bsid->num_scans++;
	}
    }

  /* Free the assigned memory */
  free(vmins);
  free(vmaxs);

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
