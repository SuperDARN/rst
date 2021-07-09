/* read_fitacf.c
   =============
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

#include "option.h"
#include "radar.h"
#include "rpos.h"
#include "scandata.h"
#include "multscan.h"
#include "hlpstr.h"
#include "errstr.h"

struct OptionData opt;

/**
 * Writes a fit, fitacf, or cfit format file to stdout
 **/

int main(int argc, char *argv[])
{
  int inum, len, fnum, channel, channel_fix, ret_stat;

  char vstr[256];

  /* Initialize input options */
  int old=0, farg=0, tlen=0;
  
  double stime=-1.0, etime=-1.0, extime=0.0, sdate=-1.0, edate=-1.0;

  unsigned char cfitflg=0, fitflg=0, vb=0, catflg=0, nsflg=0;

  char *chnstr=NULL, *chnstr_fix=NULL, *exstr=NULL, *vbuf=NULL;
  char *stmestr=NULL, *etmestr=NULL, *sdtestr=NULL, *edtestr=NULL;

  /* Initialize file information */
  char **dnames=NULL, *iname=NULL;

  /* Declare local subroutines */
  int command_options(int argc, char *argv[], int *old, int *tlen,
		      unsigned char *vb, unsigned char *cfitflg,
		      unsigned char *fitflg, unsigned char *catflg,
		      unsigned char *nsflg, char *stmestr, char *etmestr,
		      char *sdtestr, char *edtestr, char *exstr, char *chnstr,
		      char *chnstr_fix);
  int set_stereo_channel(char *chnstr);
  int set_fix_channel(char *chnstr_fix);
  double strtime(char *text);
  double strdate(char *text);
  int load_fit(int fnum, int channel, int channel_fix, int old, int tlen,
	       double stime, double sdate, double etime, double edate,
	       double extime, unsigned char wrtflg, unsigned char cfitflg,
	       unsigned char fitflg, unsigned char nsflg, unsigned char vb,
	       char *vbuf, char *iname, char **dnames,
	       struct MultRadarScan *mult_scan);
  
  /* Process the command line options */
  farg = command_options(argc, argv, &old, &tlen, &vb, &cfitflg, &fitflg,
			 &catflg, &nsflg, stmestr, etmestr, sdtestr,
			 edtestr, exstr, chnstr, chnstr_fix);

  /* If 'cn' set then determine Stereo channel, either A or B */
  channel = set_stereo_channel(chnstr);
    
  /* If 'cn_fix' set then determine appropriate channel for output file */
  channel_fix = set_fix_channel(chnstr_fix);

  /* Format the time data */
  if(exstr != NULL)  extime = strtime(exstr);
  if(stmestr != NULL) stime = strtime(stmestr);
  if(etmestr != NULL) etime = strtime(etmestr);
  if(sdtestr != NULL) sdate = strdate(sdtestr);
  if(edtestr != NULL) edate = strdate(edtestr);

  /* If verbose, set output */
  if(vb) vbuf = vstr;

  /* Make a list of input files */
  if(catflg == 0)
    {
      /* For a single input file, an index file may also be provided */
      dnames = (char **)malloc(sizeof(char*));

      if(argc-farg > 1)
	{
	  iname = argv[argc-1];
	  fnum = argc - 2;
	}
      else fnum = argc - 1;

      len = strlen(argv[fnum]);
      dnames[0] = (char *)malloc(sizeof(char) * (len+1));
      strcpy(dnames[0], argv[fnum]);
      fnum = 1;
    }
  else
    {
      /* For multiple input files, no index files are allowed */
      fnum = argc - farg;
      dnames = (char **)malloc(sizeof(char*) * fnum);

      for(inum=0; inum<fnum; inum++)
	{
	  len = strlen(argv[inum+argc]);
	  dnames[inum] = (char *)malloc(sizeof(char) * (len+1));
	  strcpy(dnames[inum], argv[inum+argc]);
	}
    }

  /* Load and output the data here */
  ret_stat = load_fit(fnum, channel, channel_fix, old, tlen, stime, sdate,
		      etime, edate, extime, 1, cfitflg, fitflg, nsflg, vb, vbuf,
		      iname, dnames, NULL);

  return ret_stat;
}


/**
 * Load the command line options
 **/

int command_options(int argc, char *argv[], int *old, int *tlen,
		    unsigned char *vb, unsigned char *cfitflg,
		    unsigned char *fitflg, unsigned char *catflg,
		    unsigned char *nsflg, char *stmestr,
		    char *etmestr, char *sdtestr, char *edtestr, char *exstr,
		    char *chnstr, char *chnstr_fix)
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

/**
 * Outputs an error statement for an unrecognized input option
 **/

int rst_opterr(char *txt) {
    fprintf(stderr,"Option not recognized: %s\n",txt);
    fprintf(stderr,"Please try: rxead_fit --help\n");
    return(-1);
}
