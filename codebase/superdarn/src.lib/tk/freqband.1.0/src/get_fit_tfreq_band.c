/* get_fit_tfreq_band.c
   ====================
   Author: Angeline G. Burrell - NRL - 2021
   This is a U.S. government work and not under copyright protection in the U.S.

   This file is part of the Radar Software Toolkit (RST).

   Disclaimer: This code is licensed under GPL v3.0 please LICENSE to see the
               full license

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
#include "freqband.h"

/**
 * @brief Determine appropriate frequency bands for a fit* data set
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
 *             band_width  - Desired width for frequency bands in kHz (300)
 *
 * @params[out] fbands   - frequency bands, with the first array dimension
 *                         corresponding to a given band and the second
 *                         dimensioon holding the minimum (0) and maximum (1).
 *              all_freq - Number of observations at each frequency.
 *              num      - Returned value, number of frequency bands or -1 if
 *                         `band_width` resulted in more than 90 frequency bands
 **/
int get_fit_tfreq_bands(int fnum, int channel, int channel_fix, int old,
			int tlen, double stime, double sdate, double etime,
			double edate, double extime, unsigned char fitflg,
			unsigned char nsflg, unsigned char vb, char *vbuf,
			char *iname, char **dnames,int band_width,
			int fbands[90][2], int all_freq[MAX_FREQ_KHZ])
{
  int yr, mo, dy, hr, mt, inum, num, ibm, freq, band_tol, local_max;
  int ret_flg=0, state=0, syncflg=1, min_freq=-1, max_freq=-1;

  double sc;

  FILE *fp, *fitfp;

  struct RadarBeam bm;
  struct RadarParm *prm;
  struct FitData *fit;
  struct CFitdata *cfit;
  struct FitIndex *inx;
  struct OldFitFp *oldfitfp=NULL;
  struct CFitfp *cfitfp=NULL;
  struct RadarScan *scan;

  /* Initialize radar parameter and fit/cfit structures */
  scan = RadarScanMake();
  prm = RadarParmMake();
  fit = FitMake();
  cfit = CFitMake();
  for(inum = 0; inum < MAX_FREQ_KHZ; inum++)
    all_freq[inum] = 0;

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

	  /* Update the local frequency data */
	  for(ibm = 0; ibm < scan->num; ibm++)
	    {
	      bm = scan->bm[ibm];
	      all_freq[bm.freq]++;
	      if(min_freq < 0 || min_freq > bm.freq) min_freq = bm.freq;
	      if(max_freq < bm.freq) max_freq = bm.freq;
	    }

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

  /* Use the data frequency range and desired bandwidth to specify frequency */
  /* bands. Allow a small tolerance for the frequency band of 10%.           */
  band_tol = band_width / 10;
  num      = 0;
  freq     = min_freq;

  while(freq <= max_freq && num < 90)
    {
      /* Define the upper limit of this frequency band */
      local_max = freq + band_width;

      /* If the observed upper limit is close, extend it */
      if(local_max + band_tol > max_freq) local_max = max_freq + 1;

      /* Determine if there are observations in this band */
      for(ibm = 0, inum = freq; inum < local_max; inum++)
	ibm += all_freq[inum];

      /* Save the result */
      if(ibm > 0)
	{
	  fbands[num][0] = freq;
	  fbands[num][1] = local_max;
	  num++;
	}

      /* Cycle to the next possible frequency band */
      freq = local_max;
      while((freq < max_freq) && (all_freq[freq] == 0)) freq++;
    }

  /* The band width was too small to split into 90 bands or less */
  if(num == 90 && fbands[89][1] <= max_freq) num = -1;

  /* Return the number of frequency bands */
  return(num);
}
