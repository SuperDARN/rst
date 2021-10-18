/* write_binary.c
   ==============
   Author Angeline G. Burrell - NRL - 2021
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
#include <sys/types.h>
#include <zlib.h>

#include "rtypes.h"
#include "rtime.h"
#include "fitmultbsid.h"

/**
 *  @brief Encode the beam data into a general data mapping structure
 *
 * @param[in] grp_flg - boolean flag, 0 to exclude or 1 to include group IDs
 *            med_flg - boolean flag, 0 to exclude or 1 to include median data
 *            ptr     - pointer to the DataMap binary writing structure
 *            bm      - data-filled FitBSIDBeam structure
 *
 * @param[out] status - returns zero upon success and -1 upon failure
 **/

int FitBSIDBeamEncode(int grp_flg, int med_flg, struct DataMap *ptr,
		      struct FitBSIDBeam bm)
{
  short int has_fov;
  int igood, irg, ngood;

  int cpid, bmnum, yr, mo, dy, hr, mt, sc, us;
  int nave, frang, rsep, rxrise, freq, noise, atten, channel, nrang;
  float bmazm;
  double seconds;

  int16 *slist=NULL, *gflg=NULL, *med_gflg=NULL;
  int16 *fov=NULL, *fov_past=NULL, *grpflg=NULL, *grpnum=NULL;
  float *pwr0=NULL, *pwr0_e=NULL, *v=NULL, *v_e=NULL;
  float *p_l=NULL, *p_l_e=NULL, *w_l=NULL, *w_l_e=NULL;
  float *phi0=NULL, *phi0_e=NULL, *elv=NULL, *elv_low=NULL, *elv_high=NULL;
  float *med_pwr0=NULL, *med_pwr0_e=NULL, *med_v=NULL, *med_v_e=NULL;
  float *med_p_l=NULL, *med_p_l_e=NULL, *med_w_l=NULL, *med_w_l_e=NULL;
  float *med_phi0=NULL, *med_phi0_e=NULL, *med_elv=NULL; 
  float *vh=NULL, *vh_e=NULL, *hop=NULL, *dist=NULL;
  char *sct=NULL, **grpid=NULL, **vh_m=NULL, **region=NULL;

  struct FitElv bm_elv;
  struct CellBSIDLoc bm_loc;

  /* Add the general beam data */
  cpid = bm.cpid;
  bmnum = bm.bm;
  bmazm = bm.bmazm;

  TimeEpochToYMDHMS(bm.time, &yr, &mo, &dy, &hr, &mt, &seconds);
  sc = bm.intt.sc;
  us = bm.intt.us;

  TimeEpochToYMDHMS(bm.time, &yr, &mo, &dy, &hr, &mt, &seconds);
  DataMapStoreScalar(ptr, "cpid", DATAINT, &cpid);
  DataMapStoreScalar(ptr, "bmnum", DATASHORT, &bmnum);
  DataMapStoreScalar(ptr, "bmazm", DATAFLOAT, &bmazm);
  DataMapStoreScalar(ptr, "yr", DATAINT, &yr);
  DataMapStoreScalar(ptr, "mo", DATAINT, &mo);
  DataMapStoreScalar(ptr, "dy", DATAINT, &dy);
  DataMapStoreScalar(ptr, "hr", DATAINT, &hr);
  DataMapStoreScalar(ptr, "mt", DATAINT, &mt);
  DataMapStoreScalar(ptr, "sc", DATAINT, &sc);
  DataMapStoreScalar(ptr, "us", DATAINT, &us);

  /* Add the beam parameter values */
  nave    = bm.nave;
  frang   = bm.frang;
  rsep    = bm.rsep;
  rxrise  = bm.rxrise;
  freq    = bm.freq;
  noise   = bm.noise;
  atten   = bm.atten;
  channel = bm.channel;
  nrang   = bm.nrang;

  DataMapStoreScalar(ptr, "nave", DATAINT, &nave);
  DataMapStoreScalar(ptr, "frang", DATAINT, &frang);
  DataMapStoreScalar(ptr, "rsep", DATAINT, &rsep);
  DataMapStoreScalar(ptr, "rxrise", DATAINT, &rxrise);
  DataMapStoreScalar(ptr, "freq", DATAINT, &freq);
  DataMapStoreScalar(ptr, "noise", DATAINT, &noise);
  DataMapStoreScalar(ptr, "atten", DATAINT, &atten);
  DataMapStoreScalar(ptr, "channel", DATAINT, &channel);
  DataMapStoreScalar(ptr, "nrang", DATAINT, &nrang);

  /* Get the size of the data arrays (only as long as the actual data) */
  sct = DataMapStoreArray(ptr, "sct", DATACHAR, 1, &nrang, NULL);

  for(ngood = 0, irg = 0; irg < nrang; irg++)
    {
      sct[irg] = bm.sct[irg];
      if(bm.sct[irg] == 1) ngood++;
    }

  /* Initiallize the range gate information */
  slist    = DataMapStoreArray(ptr, "slist", DATASHORT, 1, &ngood, NULL);
  gflg     = DataMapStoreArray(ptr, "gsct", DATASHORT, 1, &ngood, NULL);
  fov      = DataMapStoreArray(ptr, "fov", DATASHORT, 1, &ngood, NULL);
  fov_past = DataMapStoreArray(ptr, "fov_past", DATASHORT, 1, &ngood, NULL);
  pwr0     = DataMapStoreArray(ptr, "pwr0", DATAFLOAT, 1, &ngood, NULL);
  pwr0_e   = DataMapStoreArray(ptr, "pwr0_e", DATAFLOAT, 1, &ngood, NULL);
  p_l      = DataMapStoreArray(ptr, "p_l", DATAFLOAT, 1, &ngood, NULL);
  p_l_e    = DataMapStoreArray(ptr, "p_l_e", DATAFLOAT, 1, &ngood, NULL);
  w_l      = DataMapStoreArray(ptr, "p_l", DATAFLOAT, 1, &ngood, NULL);
  w_l_e    = DataMapStoreArray(ptr, "p_l_e", DATAFLOAT, 1, &ngood, NULL);
  v        = DataMapStoreArray(ptr, "v", DATAFLOAT, 1, &ngood, NULL);
  v_e      = DataMapStoreArray(ptr, "v_e", DATAFLOAT, 1, &ngood, NULL);
  phi0     = DataMapStoreArray(ptr, "phi0", DATAFLOAT, 1, &ngood, NULL); 
  phi0_e   = DataMapStoreArray(ptr, "phi0_e", DATAFLOAT, 1, &ngood, NULL); 
  elv      = DataMapStoreArray(ptr, "elv", DATAFLOAT, 1, &ngood, NULL);
  elv_low  = DataMapStoreArray(ptr, "elv_low", DATAFLOAT, 1, &ngood, NULL); 
  elv_high = DataMapStoreArray(ptr, "elv_high", DATAFLOAT, 1, &ngood, NULL);
  vh       = DataMapStoreArray(ptr, "vh", DATAFLOAT, 1, &ngood, NULL);
  vh_e     = DataMapStoreArray(ptr, "vh_e", DATAFLOAT, 1, &ngood, NULL);
  vh_m     = DataMapStoreArray(ptr, "vh_m", DATASTRING, 1, &ngood, NULL);
  region   = DataMapStoreArray(ptr, "region", DATASTRING, 1, &ngood, NULL);
  hop      = DataMapStoreArray(ptr, "hop", DATAFLOAT, 1, &ngood, NULL);
  dist     = DataMapStoreArray(ptr, "dist", DATAFLOAT, 1, &ngood, NULL);

  /* Initialize the optional structures */
  if(med_flg == 1)
    {
      med_gflg = DataMapStoreArray(ptr, "med_gsct", DATASHORT, 1, &ngood, NULL);
      med_pwr0 = DataMapStoreArray(ptr, "med_pwr0", DATAFLOAT, 1, &ngood, NULL);
      med_pwr0_e = DataMapStoreArray(ptr, "med_pwr0_e", DATAFLOAT, 1, &ngood,
				     NULL);
      med_p_l = DataMapStoreArray(ptr, "med_p_l", DATAFLOAT, 1, &ngood, NULL);
      med_p_l_e = DataMapStoreArray(ptr, "med_p_l_e", DATAFLOAT, 1, &ngood,
				    NULL);
      med_w_l = DataMapStoreArray(ptr, "med_p_l", DATAFLOAT, 1, &ngood, NULL);
      med_w_l_e = DataMapStoreArray(ptr, "med_p_l_e", DATAFLOAT, 1, &ngood,
				    NULL);
      med_v = DataMapStoreArray(ptr, "med_v", DATAFLOAT, 1, &ngood, NULL);
      med_v_e = DataMapStoreArray(ptr, "med_v_e", DATAFLOAT, 1, &ngood, NULL);
      med_phi0 = DataMapStoreArray(ptr, "med_phi0", DATAFLOAT, 1, &ngood,
				   NULL); 
      med_phi0_e = DataMapStoreArray(ptr, "med_phi0_e", DATAFLOAT, 1, &ngood,
				     NULL); 
      med_elv = DataMapStoreArray(ptr, "med_elv", DATAFLOAT, 1, &ngood, NULL);
    }

  if(grp_flg == 1)
    {
      grpflg = DataMapStoreArray(ptr, "grpflg", DATASHORT, 1, &ngood, NULL);
      grpnum = DataMapStoreArray(ptr, "grpnum", DATAINT, 1, &ngood, NULL);
      grpid  = DataMapStoreArray(ptr, "grpid", DATASTRING, 1, &ngood, NULL);
    }

  /* Set the data for all of the initialized arrays */
  for(igood = 0, irg = 0; irg < nrang; irg++)
    {
      if(bm.sct[irg] == 1)
	{
	  /* Check that the number of good points has not exceeded the */
	  /* expected maximum.                                         */
	  if(igood >= ngood) return(-1);
	  
	  /* Assign the basic data at this range gate */
	  slist[igood]    = irg;
	  gflg[igood]     = bm.rng[irg].gsct;
	  fov[igood]      = bm.rng_flgs[irg].fov;
	  fov_past[igood] = bm.rng_flgs[irg].fov_past;
	  pwr0[igood]     = bm.rng[irg].p_0;
	  pwr0_e[igood]   = bm.rng[irg].p_0_e;
	  p_l[igood]      = bm.rng[irg].p_l;
	  p_l_e[igood]    = bm.rng[irg].p_l_e;
	  w_l[igood]      = bm.rng[irg].w_l;
	  w_l_e[igood]    = bm.rng[irg].w_l_e;
	  v[igood]        = bm.rng[irg].v;
	  v_e[igood]      = bm.rng[irg].v_e;
	  phi0[igood]     = bm.rng[irg].phi0;
	  phi0_e[igood]   = bm.rng[irg].phi0_e;

	  /* Assign the FoV dependent information at this range gate */
	  if(fov[igood] == 1)
	    {
	      bm_elv  = bm.front_elv[irg];
	      bm_loc  = bm.front_loc[irg];
	      has_fov = 1;
	    }
	  else if(fov[igood] == -1)
	    {
	      bm_elv  = bm.back_elv[irg];
	      bm_loc  = bm.back_loc[irg];
	      has_fov = 1;
	    }
	  else has_fov = 0;

	  vh_m[igood] = malloc(sizeof(char) * 2);
	  region[igood] = malloc(sizeof(char) * 2);

	  if(has_fov == 1)
	    {
	      elv[igood]      = bm_elv.normal;
	      elv_low[igood]  = bm_elv.low; 
	      elv_high[igood] = bm_elv.high;
	      vh[igood]       = bm_loc.vh;
	      vh_e[igood]     = bm_loc.vh_e;
	      hop[igood]      = bm_loc.hop;
	      dist[igood]     = bm_loc.dist;

	      memcpy(vh_m[igood], bm_loc.vh_m, 2);
	      memcpy(region[igood], bm_loc.region, 2);
	    }
	  else
	    {
	      elv[igood]      = 0.0;
	      elv_low[igood]  = 0.0; 
	      elv_high[igood] = 0.0;
	      vh[igood]       = 0.0;
	      vh_e[igood]     = 0.0;
	      hop[igood]      = 0.0;
	      dist[igood]     = 0.0;

	      memcpy(vh_m[igood], "U", 2);
	      memcpy(region[igood], "U", 2);
	    }

	  /* Assign the median data, if desired */
	  if(med_flg == 1)
	    {
	      med_gflg[igood]   = bm.med_rng[irg].gsct;
	      med_pwr0[igood]   = bm.med_rng[irg].p_0;
	      med_pwr0_e[igood] = bm.med_rng[irg].p_0_e;
	      med_p_l[igood]    = bm.med_rng[irg].p_l;
	      med_p_l_e[igood]  = bm.med_rng[irg].p_l_e;
	      med_w_l[igood]    = bm.med_rng[irg].w_l;
	      med_w_l_e[igood]  = bm.med_rng[irg].w_l_e;
	      med_v[igood]      = bm.med_rng[irg].v;
	      med_v_e[igood]    = bm.med_rng[irg].v_e;
	      med_phi0[igood]   = bm.med_rng[irg].phi0;
	      med_phi0_e[igood] = bm.med_rng[irg].phi0_e;
	      med_elv[igood]    = bm.med_rng[irg].elv;
	    }

	  /* Assign the group data, if desired */
	  if(grp_flg == 1)
	    {
	      grpflg[igood] = bm.rng_flgs[irg].grpflg;
	      grpnum[igood] = bm.rng_flgs[irg].grpnum;
	      grpid[igood] = malloc(sizeof(char) * 2);
	      memcpy(grpid[igood], bm.rng_flgs[irg].grpid, 2);
	    }

	  /* Cycle to the next good index */
	  igood++;
	}
    }

  return(0);
}

/**
 *  @brief Encode and write the beam data from the fit fov/bsid structure
 *
 * @param[in] fid     - integer file ID
 *            grp_flg - boolean flag, 0 to exclude or 1 to include group IDs
 *            med_flg - boolean flag, 0 to exclude or 1 to include median data
 *            bm      - FitBSIDBeam structure object
 *
 * @param[out] status - returns -1 upon failure and the size of data written
 *                      upon success
 **/

int WriteFitBSIDBeamBin(int fid, int grp_flg, int med_flg,
			struct FitBSIDBeam bm)
{
  int status;
  struct DataMap *ptr = NULL;

  /* Initialize the binary data mapping structure */
  if((ptr = DataMapMake()) == NULL) return(-1);

  /* Encode the data mapping structure with the beam data */
  status = FitBSIDBeamEncode(grp_flg, med_flg, ptr, bm);

  /* Write the encoded beam data */
  if(status >= 0)
    {
      if(fid != -1) status = DataMapWrite(fid, ptr);
      else          status = DataMapSize(ptr);
    }

  /* Free the header data pointer */
  DataMapFree(ptr);

  return status;
}

/**
 * @brief Write a binary file containing the Fit, FoV, and BSID data
 *
 * @param[in] fp        - open file pointer
 *            grp_flg   - boolean flag, 0 to exclude or 1 to include group IDs
 *            med_flg   - boolean flag, 0 to exclude or 1 to include median data
 *            mult_scan - FitMultBSID structure with data to write
 *
 * @param[out] status - A positive value on success and -1 if there is an error
 **/

int WriteFitMultBSIDBin(FILE *fp, int grp_flg, int med_flg,
			struct FitMultBSID *mult_scan)
{
  int fid, status, iscan, ibm;

  struct FitBSIDScan *scan;
  struct DataMap *ptr = NULL;

  /* Get the file ID and set (or return) the status */
  if((fid = fileno(fp)) == -1) return(-1);

  status = 0;

  /* Initialize the binary data mapping structure */
  if((ptr = DataMapMake()) == NULL) return(-1);

  /* Add the header info */
  DataMapAddScalar(ptr, "fitmultbsid.version.major", DATAINT,
		   &mult_scan->version.major);
  DataMapAddScalar(ptr, "fitmultbsid.version.minor", DATAINT,
		   &mult_scan->version.minor);
  DataMapAddScalar(ptr, "stid", DATASHORT, &mult_scan->stid);
  DataMapAddScalar(ptr, "num_scans", DATAINT, &mult_scan->num_scans);

  /* Write the header data */
  if(status >= 0)
    {
      if(fid != -1) status = DataMapWrite(fid, ptr);
      else          status = DataMapSize(ptr);
    }

  /* Free the header data pointer */
  DataMapFree(ptr);

  /* Cycle through all the scans, encoding the binary data */
  scan = mult_scan->scan_ptr;

  for(iscan = 0; iscan < mult_scan->num_scans && status >= 0; iscan++)
    {
      /* Cycle through all the beams in this scan */
      for(ibm = 0; ibm < scan->num_bms && status >= 0; ibm++)
	status = WriteFitBSIDBeamBin(fid, grp_flg, med_flg, scan->bm[ibm]);
      
      scan = scan->next_scan;
    }

  return(status);
}


