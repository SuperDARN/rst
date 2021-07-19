/* load_fit_update_fov.c
   =====================
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
#include "dmap.h" /* DMap library needs to go before rprm.h */
#include "rprm.h"
#include "scandata.h" /* scandata.h needs to go before the various fitdata.h */
#include "fitindex.h"
#include "fitdata.h"
#include "fitread.h"
#include "fitscan.h"
#include "fitseek.h"
#include "oldfitread.h"
#include "oldfitscan.h"
#include "cfitdata.h"
#include "cfitindex.h"
#include "cfitread.h"
#include "cfitscan.h"
#include "cfitseek.h"
#include "multbsid.h"

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
 *             fitflg      - 1 if a fit file, 0 if a cfit file
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
			double edate, double extime, unsigned char fitflg,
			unsigned char nsflg, unsigned char vb, char *vbuf,
			char *iname, char **dnames, short int tdiff_flag,
			double tdiff, short int strict_gs, int freq_min,
			int freq_max, int min_pnts, int D_nrg, int E_nrg,
			int F_nrg, int far_nrg, int D_rgmax, int E_rgmax,
			int F_rgmax, float D_hmin, float D_hmax, float E_hmax,
			float F_hmax, float D_vh_box, float E_vh_box,
			float F_vh_box, float far_vh_box, float max_hop,
			struct MultRadarBSID *mult_bsid)
{
  int yr, mo, dy, hr, mt, inum, num, ibm, freq, band_tol, local_max;
  int ret_flg=0, state=0, syncflg=1, min_freq=-1, max_freq=-1;

  double sc;

  FILE *fp, *fitfp;

  struct RadarSite *site;
  struct RadarBeam bm;
  struct RadarParm *prm;
  struct FitData *fit;
  struct CFitdata *cfit;
  struct FitIndex *inx;
  struct OldFitFp *oldfitfp=NULL;
  struct CFitfp *cfitfp=NULL;
  struct RadarScan *scan;

  int exclude_outofscan(struct RadarScan *ptr);
  void load_radar_site(int yr, int mo, int dy, int hr, int mt, int sc,
		       int stid, struct RadarSite *site);
  void UpdateScanBSFoV(short int strict_gs, int freq_min, int freq_max,
		       int min_pnts, int D_nrg, int E_nrg, int F_nrg,
		       int far_nrg, int D_rgmax, int E_rgmax, int F_rgmax,
		       float D_hmin, float D_hmax, float E_hmax, float F_hmax,
		       float D_vh_box, float E_vh_box, float F_vh_box,
		       float far_vh_box, float max_hop,
		       struct MultRadarScan *mult_scan, struct RadarSite *hard,
		       struct MultRadarBSID *mult_bsid);

  /* Initialize radar parameter and fit/cfit structures */
  site = (struct RadarSite *)malloc(sizeof(struct RadarSite));
  memset(site, 0, sizeof(struct RadarSite));
  scan = RadarScanMake();
  prm = RadarParmMake();
  fit = FitMake();
  cfit = CFitMake();

  /* Cycle through all of the fit files */
  for(inum=0; inum < fnum; inum++)
    {

      /* Assign the input and index file names */
      fprintf(stderr, "Opening file:%s\n", dnames[inum]);

      /* Open the fit, fitacf, or cfit file and read the first scan */
      if(fitflg)
	{
	  /* Input file is in fit or fitacf format */
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
	      /* Input file is in fitacf format */

	      /* Check if index file provided */
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
        }
      else
	{
	  /* Input file is in cfit or format. Open the cfit file for reading */
	  if((cfitfp=CFitOpen(dnames[inum])) == NULL)
	    {
	      fprintf(stderr, "File not found.\n");
	      exit(-1);
            }

	  /* Verify that the cfit file was properly opened */
	  ret_flg = CFitReadRadarScan(cfitfp, &state, scan, cfit, tlen, syncflg,
				      channel);
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
	  if(stime==-1) stime = ((int)scan->st_time % (24*3600));

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
	  if(fitflg)
	    {
	      /* Input file is in fit or fitacf format */
	      if(old)
		ret_flg = OldFitSeek(oldfitfp, yr, mo, dy, hr, mt, sc, NULL);
	      else ret_flg = FitFseek(fitfp, yr, mo, dy, hr, mt, sc, NULL, inx);

	      /* If a matching record could not be found then exit */
	      if(ret_flg == -1)
		{
		  fprintf(stderr,
			  "File does not contain the requested interval.\n");
		  exit(-1);
                }

	      /* If using scan flag instead of tlen then continue to read
		 fit records until reaching the beginning of a new scan */
	      if(tlen==0)
		{
		  if(old)
		    {
		      while((ret_flg = OldFitRead(oldfitfp,prm,fit)) != -1)
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
	      /* Input file is in cfit format */
	      ret_flg = CFitSeek(cfitfp, yr, mo, dy, hr, mt, sc, NULL, NULL);

	      /* If a matching record could not be found then exit */
	      if(ret_flg == -1)
		{
		  fprintf(stderr,
			  "File does not contain the requested interval.\n");
		  exit(-1);
                }

	      /* If using scan flag instead of tlen then continue to read
		 cfit records until reaching the beginning of a new scan */
	      if(tlen==0)
		{
		  while((ret_flg = CFitRead(cfitfp, cfit)) != -1)
		    {
		      if(cfit->scan == 1) break;
                    }
                }
	      else state = 0;

	      /* Read the first full scan of data from open cfit file
		 corresponding to the start date and time */
	      ret_flg = CFitReadRadarScan(cfitfp, &state, scan, cfit, tlen,
					  syncflg, channel);

            }
        }
      else stime = scan->st_time;   /* If the start date and time are not
				       provided, use the time of the input
				       file's first record */

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
	  if(site == NULL)
	    {
	      load_radar_site(yr, mo, dy, hr, mt, (int)sc, scan->stid, site);

	      if(tdiff_flag) site->tdiff = tdiff;
	    }

	  /* Update the backscatter FoV data using only data from this scan */
	  UpdateScanBSFoV(strict_gs, freq_min, freq_max, min_pnts, D_nrg, E_nrg,
			  F_nrg, far_nrg, D_rgmax, E_rgmax, F_rgmax, D_hmin,
			  D_hmax, E_hmax, F_hmax, D_vh_box, E_vh_box, F_vh_box,
			  far_vh_box, max_hop, scan, site, mult_bsid);


	  /* Read next radar scan from input file */
	  if(fitflg)
	    {
	      if(old)
		ret_flg = OldFitReadRadarScan(oldfitfp, &state, scan, prm, fit,
					      tlen, syncflg, channel);
	      else
		ret_flg = FitFreadRadarScan(fitfp, &state, scan, prm, fit,
					    tlen, syncflg, channel);
            }
	  else
	    ret_flg = CFitReadRadarScan(cfitfp, &state, scan, cfit, tlen,
					syncflg, channel);

	  /* If scan data is beyond end time then break out of loop */
	  if((etime != -1) && (scan->st_time > etime)) break;
	  
        } while (ret_flg != -1);

      /* Close the input file */
      if(fitflg)
	{
	  if(old) OldFitClose(oldfitfp);
	  else fclose(fitfp);
        }
      else CFitClose(cfitfp);
    }

  /* Free the local pointers */
  free(site);

  /* Return with the status flag */
  return(ret_flg);
}
