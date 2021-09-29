/* load_fit_update_fov.c
   =====================
   Author: Angeline G. Burrell - NRL - 2021
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
#include "dmap.h" /* DMap library needs to go before rprm.h */
#include "radar.h"
#include "rprm.h"
#include "llist.h"
#include "pthread.h"
#include "scandata.h" /* scandata.h needs to go before the various fitdata.h */
#include "fitindex.h"
#include "fitdata.h"
#include "fitread.h"
#include "fitscan.h"
#include "fitseek.h"
#include "oldfitread.h"
#include "oldfitscan.h"
#include "fitmultbsid.h"

/**
 * @brief Load the fit data and determine the return direction
 *
 * @params[in] fnum        - Number of files
 *             channel     - Stereo channel number
 *             channel_fix - User-specified channel number
 *             old         - Flag for old/new FitACF input
 *             tlen        - Lenth of custon scan in seconds
 *             stime       - Starting time
 *             sdate       - Starting date
 *             etime       - Ending time
 *             edate       - Ending date
 *             extime      - Time extent (zero to use start/end date-times)
 *             nsflg       - If 1, exclude data where scan flag is -1
 *             vb          - Verbosity flag
 *             vbuf        - Verbosity buffer
 *             iname       - Index filename or NULL
 *             dnames      - Array of filenames to open
 *             strict_gs   - (1/0) Remove indeterminate backscatter (1)
 *             freq_min    - Minimum allowed transmission frequency in kHz
 *             freq_max    - Maximum transmission frequency limit in kHz
 *             min_pnts    - Minimum number of points necessary to perfrom
 *                           certain range gate or beam specific evaluations (3)
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
 *             max_hop     - maximum number of hops to consider (3.0)
 *
 * @params[out] mult_bsid - Ouptut data structure
 *              ret_flg   - Return status flag (-1 for an error)
 **/

int load_fit_update_fov(int fnum, int channel, int channel_fix, int old,
			int tlen, double stime, double sdate, double etime,
			double edate, double extime, unsigned char nsflg,
			unsigned char vb, char *vbuf, char *iname,
			char **dnames, short int tdiff_flag, double tdiff,
			short int strict_gs, int freq_min, int freq_max,
			int min_pnts, int D_nrg, int E_nrg, int F_nrg,
			int far_nrg, int D_rgmax, int E_rgmax, int F_rgmax,
			float D_hmin, float D_hmax, float E_hmax, float F_hmax,
			float D_vh_box, float E_vh_box, float F_vh_box,
			float far_vh_box, float max_hop,
			struct FitMultBSID *mult_bsid)
{
  int inum, yr = -1, mo = -1, dy = -1, hr = -1, mt = -1, ret_flg = 0, state = 0;
  int syncflg = 1, site_flg = 0;

  double sc = 0.0;

  FILE *fp, *fitfp;

  struct RadarSite *site = NULL;
  struct RadarParm *prm;
  struct FitData *fit;
  struct FitIndex *inx;
  struct OldFitFp *oldfitfp = NULL;
  struct RadarScan *scan;

  struct RadarSite *load_radar_site(int yr, int mo, int dy, int hr, int mt,
				    int sc, int stid);
  void UpdateScanBSFoV(short int strict_gs, int freq_min, int freq_max,
		       int min_pnts, int D_nrg, int E_nrg, int F_nrg,
		       int far_nrg, int D_rgmax, int E_rgmax, int F_rgmax,
		       float D_hmin, float D_hmax, float E_hmax, float F_hmax,
		       float D_vh_box, float E_vh_box, float F_vh_box,
		       float far_vh_box, float max_hop, struct RadarScan *scan,
		       struct RadarSite *hard, struct FitMultBSID *mult_bsid);

  /* Initialize radar parameter and fit structures. If you initialize `site` */
  /* it won't set properly.                                                  */
  scan = RadarScanMake();
  prm  = RadarParmMake();
  fit  = FitMake();

  /* Cycle through all of the fit files */
  for(inum=0; inum < fnum; inum++)
    {
      /* Assign the input and index file names */
      fprintf(stderr, "Opening file:%s\n", dnames[inum]);

      /* Open the fit or fitacf file, reading the first scan */
      if(old)
	{
	  /* Input file is in fit format. Open the fit file for reading */
	  if((oldfitfp = OldFitOpen(dnames[inum], iname)) == NULL)
	    {
	      fprintf(stderr, "File not found.\n");
	      exit(-1);
	    }

	  /* Read first available radar scan in fit file (will use scan
	     flag if tlen not provided) */
	  ret_flg = OldFitReadRadarScan(oldfitfp, &state, scan, prm, fit,
					tlen, syncflg, channel);

	  /* Verify that scan was properly read */
	  if(ret_flg == -1)
	    {
	      fprintf(stderr,"Error reading file.\n");
	      exit(-1);
	    }
	}
      else
	{
	  /* Input file is in fitacf format. Check if index file provided */
	  if(iname != NULL)
	    {
	      /* Open the index file for reading */
	      if((fp = fopen(iname, "r")) == NULL)
		fprintf(stderr, "Could not open index.\n");

	      else
		{
		  if((inx = FitIndexFload(fp)) == NULL)
		    fprintf(stderr, "Error loading index.\n");
		  fclose(fp);
		}
	    }

	  /* Open the fitacf file for reading */
	  if((fitfp = fopen(dnames[inum], "r")) == NULL)
	    {
	      fprintf(stderr, "File not found.\n");
	      exit(-1);
	    }

	  /* Read first available radar scan in fitacf file (will use scan
	     flag if tlen not provided) */
	  ret_flg = FitFreadRadarScan(fitfp, &state, scan, prm, fit,
				      tlen, syncflg, channel);

	  /* Verify that scan was properly read */
	  if(ret_flg == -1)
	    {
	      fprintf(stderr, "Error reading file.\n");
	      exit(-1);
	    }
	}

      /* If either start time or date not provided as input then determine it */
      if((stime != -1) || (sdate != -1))
	{
	  /* We must skip the start of the files. If start time not provided */
	  /* then use time of first record in the fit file                   */
	  if(stime==-1) stime = ((int)scan->st_time % (24 * 3600));

	  /* If start date not provided then use date of first record */
	  /* in fit file, otherwise use provided sdate                */
	  if (sdate == -1) stime += scan->st_time - ((int)scan->st_time
						     % (24 * 3600));
	  else stime += sdate;

	  /* Calculate year, month, day, hour, minute, and second of the */
	  /* scan start time.                                            */
	  TimeEpochToYMDHMS(stime, &yr, &mo, &dy, &hr, &mt, &sc);

	  /* Search for index of corresponding record in input file given */
	  /* start time                                                   */
	  if(old)
	    ret_flg = OldFitSeek(oldfitfp, yr, mo, dy, hr, mt, sc, NULL);
	  else ret_flg = FitFseek(fitfp, yr, mo, dy, hr, mt, sc, NULL, inx);

	  /* If a matching record could not be found then exit */
	  if(ret_flg == -1)
	    {
	      fprintf(stderr, "File doesn't contain the requested interval.\n");
	      exit(-1);
	    }

	  /* If using scan flag instead of tlen then continue to read
	     fit records until reaching the beginning of a new scan */
	  if(tlen==0)
	    {
	      if(old)
		{
		  while((ret_flg = OldFitRead(oldfitfp, prm, fit)) != -1)
		    {
		      if(abs(prm->scan) == 1) break;
		    }
		}
	      else
		{
		  while((ret_flg = FitFread(fitfp, prm, fit)) != -1)
		    {
		      if(abs(prm->scan) == 1) break;
		    }
		}
	    }
	  else state=0;

	  /* Read the first full scan of data from open fit or fitacf file
	     corresponding to the requested start date and time */
	  if(old)
	    ret_flg = OldFitReadRadarScan(oldfitfp, &state, scan, prm, fit,
					  tlen, syncflg, channel);
	  else
	    ret_flg = FitFreadRadarScan(fitfp, &state, scan, prm, fit, tlen,
					syncflg, channel);
	}
      else
	{
	  /* If the start date and time are not provided, use the time of */
	  /* the input file's first record                                */
	  stime = scan->st_time;   

	  /* Calculate year, month, day, hour, minute, and second of the */
	  /* scan start time.                                            */
	  TimeEpochToYMDHMS(stime, &yr, &mo, &dy, &hr, &mt, &sc);
	}

      /* If end time provided then determine end date */
      if(etime != -1)
	{
	  /* If end date not provided then use date of first record
	     in input file */
	  if(edate == -1) etime += scan->st_time - ((int) scan->st_time
						    % (24 * 3600));
	  else etime += edate;
        }

      /* If time extent provided then use that to calculate end time */
      if(extime != 0) etime = stime + extime;

      /* Continue updating the frequency limits until the input scan data is */
      /* beyond the end time or end of input file is reached                 */
      do
	{
	  /* If 'ns' option set then exclude data where scan flag = -1 */
	  if(nsflg) exclude_outofscan(scan);

	  /* Load the appropriate radar hardware information for the day
	     and time of the radar scan (only done once) */
	  if(site_flg == 0)
	    {
	      /* Get the radar site */
	      site     = load_radar_site(yr, mo, dy, hr, mt, sc, scan->stid);
	      site_flg = 1;

	      if(tdiff_flag) site->tdiff = tdiff;
	    }

	  /* Update the backscatter FoV data using only data from this scan */
	  UpdateScanBSFoV(strict_gs, freq_min, freq_max, min_pnts, D_nrg, E_nrg,
	  		  F_nrg, far_nrg, D_rgmax, E_rgmax, F_rgmax, D_hmin,
	  		  D_hmax, E_hmax, F_hmax, D_vh_box, E_vh_box, F_vh_box,
	  		  far_vh_box, max_hop, scan, site, mult_bsid);

	  /* Read next radar scan from input file */
	  if(old)
	    ret_flg = OldFitReadRadarScan(oldfitfp, &state, scan, prm, fit,
					  tlen, syncflg, channel);
	  else
	    ret_flg = FitFreadRadarScan(fitfp, &state, scan, prm, fit,
					tlen, syncflg, channel);

	  /* If scan data is beyond end time then break out of loop */
	  if((etime != -1) && (scan->st_time > etime)) break;
	  
        } while (ret_flg != -1);

      /* Close the input file */
      if(old) OldFitClose(oldfitfp);
      else fclose(fitfp);
    }

  /* Free the local pointers. Freeing site here causes abort(6) */
  if(scan != NULL) RadarScanFree(scan);
  if(prm != NULL) RadarParmFree(prm);

  /* Close the scan structure */
  if(mult_bsid->num_scans > 0)
    mult_bsid->last_ptr->next_scan = (struct FitBSIDScan *)(NULL);

  /* Return with the status flag */
  return(ret_flg);
}
