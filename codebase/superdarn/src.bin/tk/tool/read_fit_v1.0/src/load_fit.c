/* load_fit.c
   ===================
   Author: Angeline G. Burrell - NRL - 2019
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
#include "multscan.h"
/* #include "errstr.h" */

int load_fit(int fnum, int channel, int channel_fix, int old, int tlen,
	     double stime, double sdate, double etime, double edate,
	     double extime, unsigned char wrtflg, unsigned char cfitflg,
	     unsigned char fitflg, unsigned char nsflg, unsigned char vb,
	     char *vbuf, char *iname, char **dnames,
	     struct MultRadarScan *mult_scan)
{
  int yr, mo, dy, hr, mt, inum;
  int ret_flg=0, state=0, syncflg=1;
 
  double sc;

  FILE *fp, *fitfp;

  struct RadarParm *prm;
  struct FitData *fit;
  struct CFitdata *cfit;
  struct FitIndex *inx;
  struct RadarSite *site=NULL;
  struct OldFitFp *oldfitfp=NULL;
  struct CFitfp *cfitfp=NULL;
  struct RadarScanCycl *data_ptr, *prev_ptr;

  void load_radar_site(int yr, int mo, int dy, int hr, int mt, int sc,
		       int stid, struct RadarSite *site);
  int exclude_outofscan(struct RadarScan *ptr);
  void write_scan(FILE *fp, struct RadarScan *scan, unsigned char vb,
		  char *vbuf);

  /* Initialize radar parameter and fit/cfit structures */
  prm = RadarParmMake();
  fit = FitMake();
  cfit = CFitMake();

  /* Initialize RadarScan structure */
  /* scan = RadarScanMake(); */
  data_ptr = (struct RadarScanCycl *)(NULL);
  prev_ptr = (struct RadarScanCycl *)(NULL);

  /* Write the header line or initialize the multi-scan output */
  if(wrtflg)
    {
      write_scan(stdout, NULL, vb, vbuf);
      data_ptr = (struct RadarScanCycl *)(malloc(sizeof(struct RadarScanCycl)));
      memset(data_ptr, 0, sizeof(struct RadarScanCycl));
      data_ptr->prev_scan = (struct RadarScanCycl *)(NULL);
      data_ptr->next_scan = (struct RadarScanCycl *)(NULL);
    }
  else if(mult_scan->num_scans == 0)
    {
      /* Initialize a new multi-scan output */
      mult_scan->scan_ptr = &mult_scan->scan;
      data_ptr            = mult_scan->scan_ptr;
      data_ptr->prev_scan = prev_ptr;
      data_ptr->next_scan = (struct RadarScanCycl *)(NULL);
      mult_scan->last_ptr = (struct RadarScanCycl *)(NULL);
    }
  else
    {
      /* Add to an existing multi-scan output */
      prev_ptr            = mult_scan->last_ptr;
      prev_ptr->next_scan = (struct RadarScanCycl *)
	(malloc(sizeof(struct RadarScanCycl)));
      data_ptr            = prev_ptr->next_scan;
      data_ptr->prev_scan = prev_ptr;
      data_ptr->next_scan = NULL;
    }

  data_ptr->scan_data = RadarScanMake();

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
	      /* Input file is in fit format */

	      /* Open the fit file for reading */
	      if((oldfitfp = OldFitOpen(dnames[inum], iname)) == NULL)
		{
		  fprintf(stderr, "File not found.\n");
		  exit(-1);
                }

	      /* Read first available radar scan in fit file (will use scan
		 flag if tlen not provided) */
	      ret_flg = OldFitReadRadarScan(oldfitfp, &state,
					    data_ptr->scan_data, prm, fit, tlen,
					    syncflg, channel);

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
		    fprintf(stderr,"Could not open index.\n");

		  else
		    {
		      if((inx = FitIndexFload(fp)) == NULL)
			fprintf(stderr,"Error loading index.\n");
		      fclose(fp);
                    }
                }

	      /* Open the fitacf file for reading */
	      if((fitfp = fopen(dnames[inum], "r")) == NULL)
		{
		  fprintf(stderr,"File not found.\n");
		  exit(-1);
                }

	      /* Read first available radar scan in fitacf file (will use scan
		 flag if tlen not provided) */
	      ret_flg = FitFreadRadarScan(fitfp, &state, data_ptr->scan_data,
					  prm, fit,
					  tlen, syncflg, channel);

	      /* Verify that scan was properly read */
	      if(ret_flg == -1)
		{
		  fprintf(stderr,"Error reading file.\n");
		  exit(-1);
                }
            }
        }
      else
	{
	  /* Input file is in cfit or format */

	  /* Open the cfit file for reading */
	  if((cfitfp=CFitOpen(dnames[inum])) == NULL)
	    {
	      fprintf(stderr,"File not found.\n");
	      exit(-1);
            }

	  /* Verify that the cfit file was properly opened */
	  ret_flg = CFitReadRadarScan(cfitfp, &state, data_ptr->scan_data, cfit,
				      tlen, syncflg, channel);
	  if(ret_flg == -1)
	    {
	      fprintf(stderr,"Error reading file.\n");
	      exit(-1);
            }
        }

      /* If either start time or date not provided as input then determine it */
      if((stime != -1) || (sdate != -1))
	{
	  /* we must skip the start of the files */

	  /* If start time not provided then use time of first record
	   * in fit file */
	  if(stime==-1) stime = ((int)data_ptr->scan_data->st_time % (24*3600));

	  /* If start date not provided then use date of first record
	   * in fit file, otherwise use provided sdate */
	  if (sdate==-1) stime += data_ptr->scan_data->st_time -
			   ((int)data_ptr->scan_data->st_time % (24*3600));
	  else stime += sdate;

	  /* Calculate year, month, day, hour, minute, and second of 
	     grid start time */
	  TimeEpochToYMDHMS(stime, &yr, &mo, &dy, &hr, &mt, &sc);

	  /* Search for index of corresponding record in input file given
	     grid start time */
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
		      while((ret_flg = FitFread(fitfp,prm,fit)) != -1)
			{
			  if(abs(prm->scan) == 1) break;
                        }
                    }
                }
	      else state=0;

	      /* Read the first full scan of data from open fit or fitacf file
		 corresponding to grid start date and time */
	      if(old) ret_flg = OldFitReadRadarScan(oldfitfp, &state,
						    data_ptr->scan_data, prm,
						    fit, tlen, syncflg,
						    channel);
	      else
		ret_flg = FitFreadRadarScan(fitfp, &state, data_ptr->scan_data,
					    prm, fit, tlen, syncflg, channel);
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
		  while((ret_flg = CFitRead(cfitfp,cfit)) != -1)
		    {
		      if(cfit->scan == 1) break;
                    }
                }
	      else state = 0;

	      /* Read the first full scan of data from open cfit file
		 corresponding to the start date and time */
	      ret_flg = CFitReadRadarScan(cfitfp, &state, data_ptr->scan_data,
					  cfit, tlen, syncflg, channel);

            }
        }
      else stime = data_ptr->scan_data->st_time;   /* If the start date and time
						      are not provided, use the
						      time of the input
						      file's first record */

      /* If end time provided then determine end date */
      if(etime != -1)
	{
	  /* If end date not provided then use date of first record
	     in input file */
	  if(edate == -1) etime += data_ptr->scan_data->st_time -
                            ((int) data_ptr->scan_data->st_time % (24*3600));
	  else etime += edate;
        }

      /* If time extent provided then use that to calculate end time */
      if(extime != 0) etime = stime + extime;

      /* Continue writing data to stdout until the input scan data is beyond
	 the end time or end of input file is reached */
      do
	{
	  /* If 'ns' option set then exclude data where scan flag = -1 */
	  if(nsflg) exclude_outofscan(data_ptr->scan_data);

	  /* Calculate year, month, day, hour, minute and second of the output
	     scan start time */
	  TimeEpochToYMDHMS(data_ptr->scan_data->st_time, &yr, &mo, &dy, &hr,
			    &mt, &sc);

	  /* Load the appropriate radar hardware information for the day
	     and time of the radar scan (only done once) */
	  if(site == NULL)
	    load_radar_site(yr, mo, dy, hr, mt, (int)sc,
			    data_ptr->scan_data->stid, site);

	  /* write data to output file or save into structure */
	  if(wrtflg)
	    {
	      write_scan(stdout, data_ptr->scan_data, vb, vbuf);
	      if(vbuf != NULL) fprintf(stderr,"Storing:%s\n", vbuf);
	    }
	  else
	    {
	      /* Assign global starting data */
	      if(mult_scan->num_scans == 0)
		{
		  mult_scan->st_time = data_ptr->scan_data->st_time;
		  mult_scan->stid    = data_ptr->scan_data->stid;
		  mult_scan->version.major = data_ptr->scan_data->version.major;
		  mult_scan->version.minor = data_ptr->scan_data->version.minor;
		}

	      /* Update ending data and cycle to the next scan */
	      mult_scan->num_scans++;
	      mult_scan->ed_time = data_ptr->scan_data->ed_time;
	      
	      data_ptr->next_scan = (struct RadarScanCycl *)
		(malloc(sizeof(struct RadarScanCycl)));
	      prev_ptr            = data_ptr;
	      data_ptr            = data_ptr->next_scan;
	      data_ptr->scan_data = RadarScanMake();
	      data_ptr->prev_scan = prev_ptr;
	      data_ptr->next_scan = (struct RadarScanCycl *)(NULL);
	    }

	  /* Read next radar scan from input file */
	  if(fitflg)
	    {
	      if(old)
		ret_flg = OldFitReadRadarScan(oldfitfp, &state,
					      data_ptr->scan_data, prm, fit,
					      tlen, syncflg, channel);
	      else
		ret_flg = FitFreadRadarScan(fitfp, &state, data_ptr->scan_data,
					    prm, fit,
					    tlen, syncflg, channel);
            }
	  else
	    ret_flg = CFitReadRadarScan(cfitfp, &state, data_ptr->scan_data,
					cfit, tlen, syncflg, channel);

	  /* If scan data is beyond end time then break out of loop */
	  if((etime != -1) && (data_ptr->scan_data->st_time > etime)) break;
	  
        } while (ret_flg != -1);

      /* Close the input file */
      if(fitflg)
	{
	  if(old) OldFitClose(oldfitfp);
	  else fclose(fitfp);
        }
      else CFitClose(cfitfp);
    }

  /* Close open ends in data structure */
  if(data_ptr != (struct RadarScanCycl *)(NULL))
    {
      RadarScanFree(data_ptr->scan_data);
      data_ptr = prev_ptr;

      if(data_ptr != (struct RadarScanCycl *)(NULL))
	data_ptr->next_scan = NULL;

      if(mult_scan != (struct MultRadarScan *)(NULL))
	mult_scan->last_ptr = data_ptr;
    } 

  return(ret_flg);
}
