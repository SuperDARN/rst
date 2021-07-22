/* write_scan.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <zlib.h>


#include "rtypes.h"
#include "rtime.h"
#include "multbsid.h"

/**
 * @brief Cycle through and write multiple scans of radar data
 *
 * @param[in] fp        - open file pointer
 *            mult_scan - Cyclable scan structure
 **/

void write_multbsid_ascii(FILE *fp, struct MultFitBSID *mult_scan)
{
  int iscan;

  struct FitBSIDScan *scan;

  void write_bsid_scan(FILE *fp, int stid, struct FitBSIDScan *scan);

  /* Write the header */
  write_bsid_scan(fp, mult_scan->stid, NULL);

  /* Cycle through all the scans */
  scan = mult_scan->scan_ptr;
  for(iscan = 0; iscan < mult_scan->num_scans; iscan++)
    {
      /* Write all the beams from this scan */
      write_bsid_scan(fp, mult_scan->stid, scan);

      /* Cycle to the next scan */
      scan = scan->next_scan;
    }

  return;
}

/**
 * @brief Write a scan of radar data
 *
 * @param[in] fp   - Open file pointer
 *            stid - Radar Station ID
 *            scan - BSID scan structure
 **/

void write_bsid_scan(FILE *fp, int stid, struct FitBSIDScan *scan)
{
  int snum, rg, yr, mo, dy, hr, mt;
  double sc;
  
  char scan_info[10], bm_info[1000], rng_info[3000];

  struct FitBSIDBeam *bm;
  struct RadarCell rng, med_rng;
  struct FitElv elv;
  struct CellBSIDLoc loc;
  struct CellBSIDFlgs rng_flgs;

  /* If there is no scan data, print the header information */
  if(scan == NULL)
    {
      sprintf(scan_info, "#STID");
      sprintf(bm_info, "DATE TIME BMNUM BMAZM CPID INTT_SC INTT_US NAVE FRANG RSEP RXRISE FREQ NOISE ATTEN CHANNEL NRANG");
      sprintf(rng_info, "RG GFLG FOVFLG GRPFLG GRPNUM GRPID P_0 P_0_ERR V V_ERR W_L W_L_ERR P_L P_L_ERR PHI0 PHI0_ERR ELV ELV_LOW ELV_HIGH VH VH_ERR VH_METHOD REGION HOP DIST MED_P_0 MED_P_0_ERR MED_V MED_V_ERR MED_W_L MED_W_L_ERR MED_P_L MED_P_L_ERR MED_PHI0 MED_PHI0_ERR");

      fprintf(fp, "%s %s %s\n", scan_info, bm_info, rng_info);
    }
  else
    {
      /* Cycle through all the beams in this scan */
      for(snum = 0; snum < scan->num_bms; snum++)
	{
	  /* Write out the desired info that is the same for this scan */
	  sprintf(scan_info, "%d", stid);

	  /* Write out the desired info that is the same for this beam */
	  bm = scan->bm;
	  TimeEpochToYMDHMS(bm->time, &yr, &mo, &dy, &hr, &mt, &sc);

	  sprintf(bm_info, "%04d-%02d-%02d %02d:%02d:%02.0f", yr, mo, dy, hr,
		  mt, sc);
	  sprintf(bm_info, "%s%d %0.3f %d %d %d %d %d %d %d %d %d %d %d %d",
		  bm_info, bm->bm, bm->bmazm, bm->cpid, bm->intt.sc,
                  bm->intt.us, bm->nave, bm->frang, bm->rsep, bm->rxrise,
                  bm->freq, bm->noise, bm->atten, bm->channel, bm->nrang);

	  /* Cycle through all the range gates */
	  for(rg = 0; rg < bm->nrang; rg++)
	    {
	      /* Write out the range info */
	      if(bm->sct[rg] == 1)
		{
		  rng      = bm->rng[rg];
		  med_rng  = bm->med_rng[rg];
		  rng_flgs = bm->rng_flgs[rg];
		  if(rng_flgs.fov == -1)
		    {
		      elv = bm->back_elv[rg];
		      loc = bm->back_loc[rg];
		    }
		  else
		    {
		      elv = bm->front_elv[rg];
		      loc = bm->front_loc[rg];
		    }
      
		  sprintf(rng_info,
			  "%d %d %d %d %d %d %s %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %s %s %f %f %f %f %f %f %f %f %f %f %f %f",
			  rg, rng.gsct, rng_flgs.fov, rng_flgs.fov_past,
			  rng_flgs.grpflg, rng_flgs.grpnum, rng_flgs.grpid,
			  rng.p_0, rng.p_0_e, rng.v, rng.v_e, rng.w_l,
			  rng.w_l_e, rng.p_l, rng.p_l_e, rng.phi0, rng.phi0_e,
			  elv.normal, elv.low, elv.high, loc.vh, loc.vh_e,
			  loc.vh_m, loc.region, loc.hop, loc.dist, med_rng.p_0,
			  med_rng.p_0_e, med_rng.v, med_rng.v_e, med_rng.w_l,
			  med_rng.w_l_e, med_rng.p_l, med_rng.p_l_e,
			  med_rng.phi0, med_rng.phi0_e);
	      
		  fprintf(fp, "%s %s %s\n", scan_info, bm_info, rng_info);
		}
	    }
	}
    }

  return;
}