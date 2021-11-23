/* write_ascii.c
   =============
   Author: Angeline G. Burrell - NRL - 2021
   This is a U.S. government work and not under copyright protection in the U.S.

   This file is part of the Radar Software Toolkit (RST).

   Disclaimer: RST is licensed under GPL v3.0. Please visit 
               <https://www.gnu.org/licenses/> to see the full license

   Modifications:

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <zlib.h>


#include "rtypes.h"
#include "rtime.h"
#include "fitmultbsid.h"

/**
 * @brief Cycle through and write multiple scans of radar data
 *
 * @param[in] fp        - open file pointer
 *            mult_scan - Cyclable scan structure
 **/

void WriteFitMultBSIDASCII(FILE *fp, struct FitMultBSID *mult_scan)
{
  int iscan;

  struct FitBSIDScan *scan;

  /* Write the header */
  WriteFitBSIDScanASCII(fp, mult_scan->stid, NULL);

  /* Cycle through all the scans */
  scan = mult_scan->scan_ptr;

  for(iscan = 0; iscan < mult_scan->num_scans; iscan++)
    {
      /* Write all the beams from this scan */
      WriteFitBSIDScanASCII(fp, mult_scan->stid, scan);

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

void WriteFitBSIDScanASCII(FILE *fp, int stid, struct FitBSIDScan *scan)
{
  int ibm, irg, yr, mo, dy, hr, mt;
  double sc;
  
  char scan_info[10], bm_info[1000], rng_info[3000];

  struct FitBSIDBeam bm;
  struct RadarCell rng, med_rng;
  struct FitElv elv, opp_elv;
  struct CellBSIDLoc loc, opp_loc;
  struct CellBSIDFlgs rng_flgs;

  /* If there is no scan data, print the header information */
  if(scan == NULL)
    {
      sprintf(scan_info, "#STID");
      sprintf(bm_info, "DATE TIME INTT_US BMNUM BMAZM CPID NAVE FRANG RSEP RXRISE FREQ NOISE ATTEN CHANNEL NRANG");
      sprintf(rng_info, "RG GFLG FOVFLG FOV_PAST GRPFLG GRPNUM GRPID P_0 P_0_ERR V V_ERR W_L W_L_ERR P_L P_L_ERR PHI0 PHI0_ERR ELV ELV_LOW ELV_HIGH VH VH_ERR VH_METHOD REGION HOP DIST MED_P_0 MED_P_0_ERR MED_V MED_V_ERR MED_W_L MED_W_L_ERR MED_P_L MED_P_L_ERR MED_PHI0 MED_PHI0_ERR OPP_ELV OPP_ELV_LOW OPP_ELV_HIGH OPP_VH OPP_VH_ERR OPP_VH_METHOD OPP_REGION OPP_HOP OPP_DIST");

      fprintf(fp, "%s %s %s\n", scan_info, bm_info, rng_info);
    }
  else
    {
      /* Cycle through all the beams in this scan */
      for(ibm = 0; ibm < scan->num_bms; ibm++)
	{
	  /* Write out the desired info that is the same for this scan */
	  sprintf(scan_info, "%d", stid);

	  /* Write out the desired info that is the same for this beam */
	  bm = scan->bm[ibm];
	  TimeEpochToYMDHMS(bm.time, &yr, &mo, &dy, &hr, &mt, &sc);

	  sprintf(bm_info, "%04d-%02d-%02d %02d:%02d:%02d %d", yr, mo, dy, hr,
		  mt, bm.intt.sc, bm.intt.us);
	  sprintf(bm_info, "%s %d %0.3f %d %d %d %d %d %d %d %d %d %d",
		  bm_info, bm.bm, bm.bmazm, bm.cpid, bm.nave, bm.frang, bm.rsep,
		  bm.rxrise, bm.freq, bm.noise, bm.atten, bm.channel, bm.nrang);

	  /* Cycle through all the range gates */
	  for(irg = 0; irg < bm.nrang; irg++)
	    {
	      /* Write out the range info */
	      if(bm.sct[irg] == 1)
		{
		  rng      = bm.rng[irg];
		  med_rng  = bm.med_rng[irg];
		  rng_flgs = bm.rng_flgs[irg];
		  if(rng_flgs.fov == -1)
		    {
		      elv = bm.back_elv[irg];
		      loc = bm.back_loc[irg];
		      opp_elv = bm.front_elv[irg];
		      opp_loc = bm.front_loc[irg];
		    }
		  else
		    {
		      elv = bm.front_elv[irg];
		      loc = bm.front_loc[irg];
		      opp_elv = bm.back_elv[irg];
		      opp_loc = bm.back_loc[irg];
		    }
      
		  sprintf(rng_info,
			  "%d %d %d %d %d %d %s %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %s %s %f %f %f %f %f %f %f %f %f %f %f %f",
			  irg, rng.gsct, rng_flgs.fov, rng_flgs.fov_past,
			  rng_flgs.grpflg, rng_flgs.grpnum, rng_flgs.grpid,
			  rng.p_0, rng.p_0_e, rng.v, rng.v_e, rng.w_l,
			  rng.w_l_e, rng.p_l, rng.p_l_e, rng.phi0, rng.phi0_e,
			  elv.normal, elv.low, elv.high, loc.vh, loc.vh_e,
			  loc.vh_m, loc.region, loc.hop, loc.dist, med_rng.p_0,
			  med_rng.p_0_e, med_rng.v, med_rng.v_e, med_rng.w_l,
			  med_rng.w_l_e, med_rng.p_l, med_rng.p_l_e,
			  med_rng.phi0, med_rng.phi0_e);

		  sprintf(rng_info, "%s %f %f %f %f %f %s %s %f %f", rng_info,
			  opp_elv.normal, opp_elv.low, opp_elv.high, opp_loc.vh,
			  opp_loc.vh_e, opp_loc.vh_m, opp_loc.region,
			  opp_loc.hop, opp_loc.dist);
	      
		  fprintf(fp, "%s %s %s\n", scan_info, bm_info, rng_info);
		}
	    }
	}
    }

  return;
}
