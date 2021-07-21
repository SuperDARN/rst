
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "option.h"
#include "rtime.h"
#include "radar.h"
#include "rpos.h"
#include "scandata.h"
#include "multscan.h"

#include "hlpstr.h"
#include "errstr.h"

struct OptionData opt;

int main(int argc, char *argv[])
{
  short int strict_gs=0, tdiff_flag=0;

  int yr, mo, dy, hr, mt, sc, stid;
  int inum, len, fnum, channel, channel_fix, ret_stat, nfbands;
  int fbands[90][2];

  double tdiff=0.0;

  char vstr[256];
  /* Initialize input options ADD OPTION TO UPDATE TDIFF */
  /* Default frequency limits set to the limits of the HF range */
  int old=0, farg=0, tlen=0, freq_min=3000, freq_max=30000;

  double stime=-1.0, etime=-1.0, extime=0.0, sdate=-1.0, edate=-1.0;

  unsigned char cfitflg=0, fitflg=0, vb=0, catflg=0, nsflg=0;

  char *chnstr=NULL, *chnstr_fix=NULL, *exstr=NULL, *vbuf=NULL;
  char *stmestr=NULL, *etmestr=NULL, *sdtestr=NULL, *edtestr=NULL;

  /* Initialize file information */
  char **dnames=NULL, *iname=NULL;

  /* Set these parameters to davitpy defaults. Could be optional inputs */
  int min_pnts=3, band_width=300;
  int D_nrg=2, E_nrg=5, F_nrg=10, far_nrg=20;
  int D_rgmax=5, E_rgmax=25, F_rgmax=40;
  float D_hmin=75, D_hmax=115, E_hmax=150, F_hmax=450;
  float D_vh_box=40, E_vh_box=35, F_vh_box=50, far_vh_box=150;
  float max_hop=3.0;
  
  /* Declare local subroutines */
  int command_options(int argc, char *argv[], int *old, int *tlen,
		      unsigned char *vb, unsigned char *cfitflg,
		      unsigned char *fitflg, unsigned char *catflg,
		      unsigned char *nsflg, char *stmestr, char *etmestr,
		      char *sdtestr, char *edtestr, char *exstr, char *chnstr,
		      char *chnstr_fix, int *freq_min, int *freq_max,
		      short int *strict_gs, short int *tdiff_flag,
		      double *tdiff);
  int set_stereo_channel(char *chnstr);
  int set_fix_channel(char *chnstr_fix);
  double strtime(char *text);
  double strdate(char *text);

  /* Process the command line options */
  farg = command_options(argc, argv, &old, &tlen, &vb, &cfitflg, &fitflg,
			 &catflg, &nsflg, stmestr, etmestr, sdtestr, edtestr,
			 exstr, chnstr, chnstr_fix, &freq_min, &freq_max,
			 &strict_gs, &tdiff_flag, &tdiff);
  printf("OPT\n");

  /* Test the command line options */
  printf("HI! %d %d %d %d %d %d %s %s %s %s %s %s %s %s %d %d %d %d %f\n", old,
	 tlen, vb, cfitflg, fitflg, catflg, nsflg, stmestr, etmestr,
	 sdtestr, edtestr, exstr, chnstr, chnstr_fix, freq_min, freq_max,
	 strict_gs, tdiff_flag, tdiff);fflush(stdout);

  /* If 'cn' set then determine Stereo channel, either A or B */
  channel = set_stereo_channel(chnstr);
    
  /* If 'cn_fix' set then determine appropriate channel for output file */
  channel_fix = set_fix_channel(chnstr_fix);

  /* Format the time data */
  if(exstr   != NULL) extime = strtime(exstr);
  if(stmestr != NULL) stime  = strtime(stmestr);
  if(etmestr != NULL) etime  = strtime(etmestr);
  if(sdtestr != NULL) sdate  = strdate(sdtestr);
  if(edtestr != NULL) edate  = strdate(edtestr);

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

  return(0);
}
  
/**
 * Load the command line options
 **/

int command_options(int argc, char *argv[], int *old, int *tlen,
		    unsigned char *vb, unsigned char *cfitflg,
		    unsigned char *fitflg, unsigned char *catflg,
		    unsigned char *nsflg, char *stmestr, char *etmestr,
		    char *sdtestr, char *edtestr, char *exstr, char *chnstr,
		    char *chnstr_fix, int *freq_min, int *freq_max,
		    short int *strict_gs, short int *tdiff_flag, double *tdiff)
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

  /* Apply transmission frequency limits */
  OptionAdd(&opt, "tfmin", 'x', freq_min); /* Minimum transmission frequency */
  OptionAdd(&opt, "tfmax", 'x', freq_max); /* Maximum transmission frequency */

  /* Apply groundscatter strictness conditions (1=remove indeterminate GS) */
  OptionAdd(&opt, "gs_strict", 's', strict_gs);

  /* Process the tdiff options */
  OptionAdd(&opt, "update_tdiff", 's', tdiff_flag);
  OptionAdd(&opt, "tdiff", 'd', tdiff);

  /* Process command line options */
  farg = OptionProcess(1, argc, argv, &opt, rst_opterr);

  /* If command line option not recognized then print error and exit */
  if(farg == -1) exit(-1);

  if(farg == argc)
    {
      fprintf(stderr, "Input options requiired, try: test --help\n");
      exit(-1);
    }

  return farg;
}

/**
 * Outputs an error statement for an unrecognized input option
 **/

int rst_opterr(char *txt)
{
    fprintf(stderr,"Option not recognized: %s\n", txt);
    fprintf(stderr,"Please try: test --help\n");
    return(-1);
}
