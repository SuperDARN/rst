/* read_fitacf.c
   ===========
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
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "option.h"
#include "rtime.h"
#include "radar.h"
#include "rprm.h"
#include "fitdata.h"
#include "cfitdata.h"
#include "scandata.h"
#include "fitread.h"
#include "fitscan.h"
#include "fitindex.h"
#include "fitseek.h"
#include "oldfitread.h"
#include "oldfitscan.h"
#include "cfitread.h"
#include "cfitindex.h"
#include "cfitseek.h"
#include "cfitscan.h"
#include "fitscan.h"
#include "filter.h"
#include "bound.h"
#include "checkops.h"
#include "rpos.h"

#include "hlpstr.h"
#include "errstr.h"

struct RadarParm *prm;
struct FitData *fit;
struct CFitdata *cfit;
struct RadarScan *src;

struct FitIndex *inx;

struct RadarNetwork *network;
struct Radar *radar;
struct RadarSite *site;

struct OptionData opt;

/**
 * Writes a fit, fitacf, or cfit format file to stdout
 **/

int main(int argc, char *argv[])
{
  int inum, fnum;
  int yr, mo, dy, hr, mt;
 
  double sc;

  char vstr[256], *envstr;
  FILE *fp;

  /* Initialize input options */
  int s=0, state=0, syncflg=1;
  int old=0, farg=0, tlen=0, channel=0, channel_fix=-1;
  
  double stime=-1, etime=-1, extime=0, sdate=-1, edate=-1;

  unsigned char cfitflg=0, fitflg=0, vb=0, catflg=0, nsflg=0;

  char *chnstr=NULL, *chnstr_fix=NULL;
  char *stmestr=NULL, *etmestr=NULL, *sdtestr=NULL, *edtestr=NULL;
  char *exstr=NULL, *vbuf=NULL;

  /* Initialize file information */
  char *dname=NULL, *iname=NULL;
  FILE *fitfp=NULL;

  /* Initialize structures */
  struct OldFitFp *oldfitfp=NULL;
  struct CFitfp *cfitfp=NULL;

  /* Declare local subroutines */
  int command_options(int argc, char *argv[], int *old, int *tlen,
		      unsigned char *vb, unsigned char *cfitflg,
		      unsigned char *fitflg, unsigned char *catflg,
		      unsigned char *nsflg, char *stmestr, char *etmestr,
		      char *sdtestr, char *edtestr, char *exstr, char *chnstr,
		      char *chnstr_fix);
  double strtime(char *text);
  int exclude_outofscan(struct RadarScan *ptr);
  double strdate(char *text);
  void write_scan(FILE *fp, struct RadarScan *scan, unsigned char vb,
		  char *vbuf);

  /* Process the command line options */
  farg = command_options(argc, argv, &old, &tlen, &vb, &cfitflg, &fitflg,
			 &catflg, &nsflg, stmestr, etmestr, sdtestr,
			 edtestr, exstr, chnstr, chnstr_fix);

  /* Initialize radar parameter and fit/cfit structures */
  prm = RadarParmMake();
  fit = FitMake();
  cfit = CFitMake();

  /* Initialize RadarScan structure */
  src = RadarScanMake();

  /* Write the header line */
  write_scan(stdout, NULL, vb, vbuf);

  /* Make sure the SD_RADAR environment variable is set */
  envstr=getenv("SD_RADAR");
  if(envstr==NULL)
    {
      fprintf(stderr, "Environment variable 'SD_RADAR' must be defined.\n");
      exit(-1);
    }

  /* Open the radar information file */
  fp = fopen(envstr, "r");
  if(fp==NULL)
    {
      fprintf(stderr,"Could not locate radar information file.\n");
      exit(-1);
    }

  /* Load the radar network information */
  network = RadarLoad(fp);
  fclose(fp);
  if(network==NULL)
    {
      fprintf(stderr,"Failed to read radar information.\n");
      exit(-1);
    }

  /* Make sure the SD_HDWPATH environment variable is set */
  envstr = getenv("SD_HDWPATH");
  if(envstr==NULL)
    {
      fprintf(stderr,"Environment variable 'SD_HDWPATH' must be defined.\n");
      exit(-1);
    }

  /* Load the hardware information for the radar network */
  RadarLoadHardware(envstr, network);

  /* If 'cn' set then determine Stereo channel, either A or B */
  if(chnstr != NULL)
    {
      if(tolower(chnstr[0]) == 'a') channel = 1;
      if(tolower(chnstr[0]) == 'b') channel = 2;
    }
    
  /* If 'cn_fix' set then determine appropriate channel for output file */
  if(chnstr_fix != NULL)
    {
      if(tolower(chnstr_fix[0]) == 'a') channel_fix=1;
      if(tolower(chnstr_fix[0]) == 'b') channel_fix=2;
      if(tolower(chnstr_fix[0]) == 'c') channel_fix=3;
      if(tolower(chnstr_fix[0]) == 'd') channel_fix=4;
    }

  /* Format the time data */
  if (exstr != NULL)  extime = strtime(exstr);
  if (stmestr != NULL) stime = strtime(stmestr);
  if (etmestr != NULL) etime = strtime(etmestr);
  if (sdtestr != NULL) sdate = strdate(sdtestr);
  if (edtestr != NULL) edate = strdate(edtestr);

  /* If verbose, set output */
  if(vb) vbuf = vstr;

  /* Determine the file name indeces */
  inum = -1;
  if(catflg == 0)
    {
      /* For a single input file, an index file may also be provided */
      if(argc - farg > 1)
	{
	  fnum = argc - 2;
	  inum = argc - 1;
	}
      else
	fnum = argc - 1;
    }
  else
    {
      /* For multiple input files, no index files are allowed */
      fnum = farg;
    }

  for(; fnum < argc; fnum++)
    {
      /* Assign the input and index file names */
      dname = argv[fnum];
      if(inum >= 0) iname = argv[inum];

      fprintf(stderr, "Opening file:%s\n", argv[fnum]);

      /* Open the fit, fitacf, or cfit file and read the first scan */
      if(fitflg)
	{
	  /* Input file is in fit or fitacf format */
	  if(old)
	    {
	      /* Input file is in fit format */

	      /* Open the fit file for reading */
	      if((oldfitfp = OldFitOpen(dname, iname)) == NULL)
		{
		  fprintf(stderr,"File not found.\n");
		  exit(-1);
                }

	      /* Read first available radar scan in fit file (will use scan
		 flag if tlen not provided) */
	      s = OldFitReadRadarScan(oldfitfp, &state, src, prm,
                                      fit, tlen, syncflg, channel);

	      /* Verify that scan was properly read */
	      if(s == -1)
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
		    {
		      fprintf(stderr,"Could not open index.\n");
                    }
		  else
		    {
		      if((inx = FitIndexFload(fp)) == NULL)
			fprintf(stderr,"Error loading index.\n");
		      fclose(fp);
                    }
                }

	      /* Open the fitacf file for reading */
	      if((fitfp=fopen(dname,"r")) == NULL)
		{
		  fprintf(stderr,"File not found.\n");
		  exit(-1);
                }

	      /* Read first available radar scan in fitacf file (will use scan
		 flag if tlen not provided) */
	      s = FitFreadRadarScan(fitfp, &state, src, prm, fit,
                                    tlen, syncflg, channel);

	      /* Verify that scan was properly read */
	      if(s == -1)
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
	  if((cfitfp=CFitOpen(dname)) == NULL)
	    {
	      fprintf(stderr,"File not found.\n");
	      exit(-1);
            }

	  /* Verify that the cfit file was properly opened */
	  s = CFitReadRadarScan(cfitfp, &state, src, cfit, tlen, syncflg,
				channel);
	  if(s == -1)
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
	  if(stime==-1) stime = ((int) src->st_time % (24*3600));

	  /* If start date not provided then use date of first record
	   * in fit file, otherwise use provided sdate */
	  if (sdate==-1) stime += src->st_time -
			   ((int) src->st_time % (24*3600));
	  else stime += sdate;

	  /* Calculate year, month, day, hour, minute, and second of 
	     grid start time */
	  TimeEpochToYMDHMS(stime, &yr, &mo, &dy, &hr, &mt, &sc);

	  /* Search for index of corresponding record in input file given
	     grid start time */
	  if(fitflg)
	    {
	      /* Input file is in fit or fitacf format */

	      if(old) s = OldFitSeek(oldfitfp, yr, mo, dy, hr, mt, sc, NULL);
	      else s = FitFseek(fitfp, yr, mo, dy, hr, mt, sc, NULL, inx);

	      /* If a matching record could not be found then exit */
	      if(s == -1)
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
		      while((s=OldFitRead(oldfitfp,prm,fit)) != -1)
			{
			  if(abs(prm->scan) == 1) break;
                        }
                    }
		  else
		    {
		      while((s=FitFread(fitfp,prm,fit)) != -1)
			{
			  if(abs(prm->scan) == 1) break;
                        }
                    }
                }
	      else state=0;

	      /* Read the first full scan of data from open fit or fitacf file
		 corresponding to grid start date and time */
	      if(old) s = OldFitReadRadarScan(oldfitfp, &state, src, prm,
					      fit, tlen, syncflg, channel);
	      else s = FitFreadRadarScan(fitfp, &state, src, prm, fit,
					 tlen, syncflg, channel);
            }
	  else
	    {
	      /* Input file is in cfit format */

	      s = CFitSeek(cfitfp, yr, mo, dy, hr, mt, sc, NULL, NULL);

	      /* If a matching record could not be found then exit */
	      if(s == -1)
		{
		  fprintf(stderr,
			  "File does not contain the requested interval.\n");
		  exit(-1);
                }

	      /* If using scan flag instead of tlen then continue to read
		 cfit records until reaching the beginning of a new scan */
	      if(tlen==0)
		{
		  while((s=CFitRead(cfitfp,cfit)) != -1)
		    {
		      if(cfit->scan == 1) break;
                    }
                }
	      else state = 0;

	      /* Read the first full scan of data from open cfit file
		 corresponding to the start date and time */
	      s = CFitReadRadarScan(cfitfp, &state, src, cfit, tlen,
				    syncflg, channel);

            }
        }
      else stime = src->st_time;   /* If the start date and time are not
				      provided, use the time of the input
				      file's first record */

      /* If end time provided then determine end date */
      if(etime != -1)
	{
	  /* If end date not provided then use date of first record
	     in input file */
	  if(edate == -1) etime += src->st_time -
                            ((int) src->st_time % (24*3600));
	  else etime += edate;
        }

      /* If time extent provided then use that to calculate end time */
      if(extime != 0) etime = stime + extime;

      /* Continue writing data to stdout until the input scan data is beyond
	 the end time or end of input file is reached */
      do
	{
	  /* If 'ns' option set then exclude data where scan flag = -1 */
	  if(nsflg) exclude_outofscan(src);

	  /* Calculate year, month, day, hour, minute and second of the output
	     scan start time */
	  TimeEpochToYMDHMS(src->st_time,&yr,&mo,&dy,&hr,&mt,&sc);

	  /* Load the appropriate radar hardware information for the day
	     and time of the radar scan (only done once) */
	  if(site == NULL)
	    {
	      radar = RadarGetRadar(network, src->stid);
	      if(radar==NULL)
		{
		  fprintf(stderr,"Failed to get radar information.\n");
		  exit(-1);
		}
	      site = RadarYMDHMSGetSite(radar, yr, mo, dy, hr, mt, (int)sc);
	    }

	  /* write data to output file */
	  write_scan(stdout, src, vb, vbuf);

	  if(vbuf != NULL) fprintf(stderr,"Storing:%s\n", vbuf);

	  /* Read next radar scan from input file */
	  if(fitflg)
	    {
	      if(old) s = OldFitReadRadarScan(oldfitfp, &state, src, prm, fit,
					      tlen, syncflg, channel);
	      else s = FitFreadRadarScan(fitfp, &state, src, prm, fit,
					 tlen, syncflg, channel);
            }
	  else
	    s = CFitReadRadarScan(cfitfp, &state, src, cfit, tlen, syncflg,
				  channel);

	  /* If scan data is beyond end time then break out of loop */
	  if((etime != -1) && (src->st_time>etime)) break;
	  
        } while (s != -1);

      /* Close the input file */
      if(fitflg)
	{
	  if(old) OldFitClose(oldfitfp);
	  else fclose(fitfp);
        }
      else CFitClose(cfitfp);
    }

    return 0;

}


/**
 * Load the command line options
 **/

int command_options(int argc, char *argv[], int *old, int *tlen,
		    unsigned char *vb, unsigned char *cfitflg,
		    unsigned char *fitflg, unsigned char *catflg,
		    unsigned char *nsflg, char *stmestr, char *etmestr,
		    char *sdtestr, char *edtestr, char *exstr, char *chnstr,
		    char *chnstr_fix)
{
  /* Initialize input options */
  int farg=0;
  unsigned char help=0, option=0, version=0;

  int rst_opterr(char *txt);


  /* If only information is desired, print it out and exit */
  OptionAdd(&opt, "-help", 'x', &help);
  OptionAdd(&opt, "-option", 'x', &option);
  OptionAdd(&opt, "-version", 'x', &version);

  /* If 'help' set then print help message */
  if(help==1)
    {
      OptionPrintInfo(stdout, hlpstr);
      exit(0);
    }

  /* If 'option' set then print all command line options */
  if(option==1)
    {
      OptionDump(stdout, &opt);
      exit(0);
    }

  /* If 'version' set, then print the version number */
  if(version==1)
    {
      OptionVersion(stdout);
      exit(0);
    }

  /* Determine input file format */
  OptionAdd(&opt, "old", 'x', old);      /* Old fit format */
  OptionAdd(&opt, "fit", 'x', fitflg);   /* New fit format */
  OptionAdd(&opt, "cfit", 'x', cfitflg); /* cfit format    */

  /* If not explicity working from cfit files, treat input as fit or fitacf */
  if(*cfitflg == 0) *fitflg = 1;

  /* Concatenate multiple input files */
  OptionAdd(&opt, "c", 'x', catflg); 

  /* Verbose: log information to console */
  OptionAdd(&opt, "vb", 'x', vb);

  /* Set time information */
  OptionAdd(&opt, "st", 't', stmestr);  /* Start time in HH:MM format */
  OptionAdd(&opt, "et", 't', etmestr);  /* End time in HH:MM format */
  OptionAdd(&opt, "sd", 't', sdtestr);  /* Start date in YYYYMMDD format */
  OptionAdd(&opt, "ed", 't', edtestr);  /* End date in YYYYMMDD format */
  OptionAdd(&opt, "ex", 't', exstr);    /* Use interval with extent HH:MM */

  /* Ignore scan flag, use scan length of tl seconds */
  OptionAdd(&opt, "tl", 'i', tlen);

  /* Process channel options */
  OptionAdd(&opt, "cn", 't', chnstr);         /* For stereo channel a or b   */
  OptionAdd(&opt, "cn_fix", 't', chnstr_fix); /* User-defined channel number */

  /* Apply scan flag limit (ie exclude data with scan flag = -1) */
  OptionAdd(&opt, "ns", 'x', &nsflg);

  /* Process command line options */
  farg = OptionProcess(1, argc, argv, &opt, rst_opterr);

  /* If command line option not recognized then print error and exit */
  if(farg == -1) exit(-1);

  if(farg == argc)
    {
      OptionPrintInfo(stderr, errstr);
      exit(-1);
    }

  return farg;
}
