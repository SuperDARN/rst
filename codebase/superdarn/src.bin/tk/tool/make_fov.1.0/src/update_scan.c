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
  int iscan, ibm, irg;

  float near_rg;

  
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

  /* Initialize the local pointers */
  scan_old = mult_scan->scan_ptr;
  bm_new = (struct RadarBSIDBeam *)(malloc(sizeof(struct RadarBSIDBeam)))

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
	}

     /* To determine the FoV, evaluate the elevation variations across all */
     /* beams for a range gate and virtual height band, considering each   */
     /* propagation path (region and hop) seperately.                      */
     
    }

  return;
}
