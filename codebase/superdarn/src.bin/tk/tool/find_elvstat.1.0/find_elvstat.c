/* find_elvstat.c
   ========
   Author: Angeline G. Burrell - NRL - 2023
   This is a U.S. government work and not under copyright protection in the U.S.

   This file is part of the Radar Software Toolkit (RST).

   Disclaimer: RST is licensed under GPL v3.0. Please visit 
               <https://www.gnu.org/licenses/> to see the full license

   Modifications:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "option.h"
#include "rtime.h"
#include "channel.h"
#include "elvstat.h"
#include "hlpstr.h"
#include "errstr.h"

struct OptionData opt;

/**
 * @brief Outputs an error statement for an unrecognized input option
 **/

int rst_opterr(char *txt)
{
    fprintf(stderr,"Option not recognized: %s\n", txt);
    fprintf(stderr,"Please try: find_elvstat --help\n");
    return(-1);
}

/**
 * @brief Load the command line options
 **/

int command_options(int argc, char *argv[], int *old, unsigned char *vb,
		    unsigned char *fitflg, unsigned char *catflg,
		    unsigned char *nsflg, char **stmestr, char **etmestr,
		    char **sdtestr, char **edtestr, char **exstr,
		    char **chnstr, char **chnstr_fix)
{
  /* Initialize input options */
  int farg=0;
  unsigned char help=0, option=0, version=0, cfitflg=0;

  int rst_opterr(char *txt);

  /* If only information is desired, print it out and exit */
  OptionAdd(&opt, "-help", 'x', &help);
  OptionAdd(&opt, "-option", 'x', &option);
  OptionAdd(&opt, "-version", 'x', &version);

  /* Determine input file format */
  OptionAdd(&opt, "old", 'x', old);      /* Old fit format */
  OptionAdd(&opt, "fit", 'x', fitflg);   /* New fit format */
  OptionAdd(&opt, "cfit", 'x', &cfitflg); /* cfit format    */

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

  /* Process channel options */
  OptionAdd(&opt, "cn", 't', chnstr);         /* For stereo channel a or b   */
  OptionAdd(&opt, "cn_fix", 't', chnstr_fix); /* User-defined channel number */

  /* Apply scan flag limit (ie exclude data with scan flag = -1) */
  OptionAdd(&opt, "ns", 'x', &nsflg);

  /* Process command line options */
  farg = OptionProcess(1, argc, argv, &opt, rst_opterr);

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

  /* If command line option not recognized then print error and exit */
  if(farg == -1) exit(-1);

  if(farg == argc)
    {
      OptionPrintInfo(stderr, errstr);
      exit(-1);
    }

  /* If not explicity working from cfit files, treat input as fit or fitacf */
  if(cfitflg == 0) *fitflg = 1;

  return farg;
}

/**
 * @brief Dynamically determine the number of potentially valid elevation angles
 **/

int main(int argc, char *argv[])
{
  int inum, len, fnum, num_elv;

  char vstr[256];

  /* Initialize input options */
  int old=0, farg=0, channel=0, channel_fix=0;

  double stime=-1.0, etime=-1.0, extime=0.0, sdate=-1.0, edate=-1.0;

  unsigned char fitflg=0, vb=0, catflg=0, nsflg=0;

  char *chnstr=NULL, *chnstr_fix=NULL, *exstr=NULL, *vbuf=NULL;
  char *stmestr=NULL, *etmestr=NULL, *sdtestr=NULL, *edtestr=NULL;

  /* Initialize file information */
  char **dnames=NULL, *iname=NULL;

  /* Declare local subroutines */
  int command_options(int argc, char *argv[], int *old, unsigned char *vb,
		      unsigned char *fitflg, unsigned char *catflg,
		      unsigned char *nsflg, char **stmestr, char **etmestr,
		      char **sdtestr, char **edtestr, char **exstr,
		      char **chnstr, char **chnstr_fix);

  /* Process the command line options */
  farg = command_options(argc, argv, &old, &vb, &fitflg, &catflg, &nsflg,
			 &stmestr, &etmestr, &sdtestr, &edtestr, &exstr,
			 &chnstr, &chnstr_fix);

  /* If 'cn' set then determine Stereo channel, either A or B */
  if(chnstr != NULL) channel = set_stereo_channel(chnstr);
    
  /* If 'cn_fix' set then determine appropriate channel for output file */
  if(chnstr_fix != NULL) channel_fix = set_fix_channel(chnstr_fix);

  /* Format the time data */
  if(exstr   != NULL) extime = TimeStrToSOD(exstr);
  if(stmestr != NULL) stime  = TimeStrToSOD(stmestr);
  if(etmestr != NULL) etime  = TimeStrToSOD(etmestr);
  if(sdtestr != NULL) sdate  = TimeStrToEpoch(sdtestr);
  if(edtestr != NULL) edate  = TimeStrToEpoch(edtestr);

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

  /* Establish the number of potentially valid elevation angles */
  num_elv = get_fit_elv_stat(fnum, channel, channel_fix, old, stime, sdate,
			     etime, edate, extime, fitflg, nsflg, vb, vbuf,
			     iname, dnames);

  /* Output the number of potentially valid elevation angles */
  fprintf(stdout, "%d positive elevation angles below 90 degrees\n", num_elv);

  return(num_elv);
}
