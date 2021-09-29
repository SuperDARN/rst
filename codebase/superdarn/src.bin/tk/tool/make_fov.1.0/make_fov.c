/* make_fov.c
   ==========
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

#include "rtime.h"
#include "option.h"
#include "fitmultbsid.h"
#include "channel.h"
#include "freqband.h"
#include "hlpstr.h"
#include "errstr.h"

#ifndef MAX_FREQ_KHZ
#define MAX_FREQ_KHZ 30000
#endif

struct OptionData opt;

/**
 * Outputs an error statement for an unrecognized input option
 **/

int rst_opterr(char *txt)
{
    fprintf(stderr, "Option not recognized: %s\n", txt);
    fprintf(stderr, "Please try: make_fov --help\n");
    return(-1);
}

/**
 * Load the command line options
 **/

int command_options(int argc, char *argv[], int *old, int *tlen,
		    unsigned char *vb, unsigned char *catflg,
		    unsigned char *nsflg, char **stmestr, char **etmestr,
		    char **sdtestr, char **edtestr, char **exstr, char **chnstr,
		    char **chnstr_fix, int *freq_min, int *freq_max,
		    int *band_width, short int *strict_gs,
		    short int *tdiff_flag, double *tdiff, float *D_hmin,
		    float *D_hmax, float *E_hmax, float *F_hmax, int *nbms)
{
  /* Initialize input options */
  int farg=0;
  unsigned char help=0, option=0, version=0;

  int rst_opterr(char *txt);

  /* If only information is desired, print it out and exit */
  OptionAdd(&opt, "-help", 'x', &help);
  OptionAdd(&opt, "-option", 'x', &option);
  OptionAdd(&opt, "-version", 'x', &version);

  /* Determine input file format */
  OptionAdd(&opt, "old", 'x', old);      /* Old fit format */

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
  OptionAdd(&opt, "ns", 'x', nsflg);

  /* Apply transmission frequency limits or options */
  OptionAdd(&opt, "tfmin", 'i', freq_min); /* Minimum transmission frequency */
  OptionAdd(&opt, "tfmax", 'i', freq_max); /* Maximum transmission frequency */
  OptionAdd(&opt, "bandwidth", 'i', band_width); /* frequency bandwidth */

  /* Apply groundscatter strictness conditions (1=remove indeterminate GS) */
  OptionAdd(&opt, "gs-strict", 'x', strict_gs);

  /* Process the tdiff options */
  OptionAdd(&opt, "update-tdiff", 'x', tdiff_flag);
  OptionAdd(&opt, "tdiff", 'd', tdiff);

  /* Process the region height limit options */
  OptionAdd(&opt, "dhmin", 'f', D_hmin);
  OptionAdd(&opt, "dhmax", 'f', D_hmax);
  OptionAdd(&opt, "ehmax", 'f', E_hmax);
  OptionAdd(&opt, "fhmax", 'f', F_hmax);

  /* Number of beams to use in UT evaluation */
  OptionAdd(&opt, "nbms", 'i', nbms);

  /* Process command line options */
  farg = OptionProcess(1, argc, argv, &opt, rst_opterr);

  /* If 'help' set then print help message */
  if(help==1)
    {
      OptionPrintInfo(stdout, hlpstr);
      exit(0);
    }

  /* If 'version' set, then print the version number */
  if(version==1)
    {
      OptionVersion(stdout);
      exit(0);
    }

  /* If 'option' set then print all command line options */
  if(option==1)
    {
      OptionDump(stdout, &opt);
      exit(0);
    }

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
 * Determines the backscatter origin field-of-view
 **/

int main(int argc, char *argv[])
{
  int inum, len, fnum, ret_stat, nfbands;
  int fbands[90][2], all_freq[MAX_FREQ_KHZ];

  char vstr[256];

  struct FitMultBSID *mult_bsid;

  /* Initialize input options */
  /* Default frequency limits set to the limits of the HF range */
  short int strict_gs=0, tdiff_flag=0;

  int old=0, farg=0, tlen=0, channel=0, channel_fix=0;
  int freq_min=3000, freq_max=30000, band_width=300, nbms=3;

  float D_hmin=75, D_hmax=100, E_hmax=120, F_hmax=750;

  double stime=-1.0, etime=-1.0, extime=0.0, sdate=-1.0, edate=-1.0, tdiff=0.0;

  unsigned char vb=0, catflg=0, nsflg=0;

  char *vbuf=NULL, *chnstr=NULL, *chnstr_fix=NULL, *exstr=NULL, *etmestr=NULL;
  char *edtestr=NULL, *sdtestr=NULL, *stmestr=NULL;

  /* Initialize file information */
  char **dnames=NULL, *iname=NULL;

  /* Set these parameters to davitpy defaults. Could be optional inputs */
  int min_pnts=3;
  int D_nrg=2, E_nrg=5, F_nrg=10, far_nrg=20;
  int D_rgmax=5, E_rgmax=25, F_rgmax=40;
  float D_vh_box=40, E_vh_box=35, F_vh_box=50, far_vh_box=150;
  float max_hop=3.0, min_frac=0.1;
  double ut_box_sec=1200.0;  /* 20 minutes */

  /* Declare local subroutines */
  int command_options(int argc, char *argv[], int *old, int *tlen,
		      unsigned char *vb, unsigned char *catflg,
		      unsigned char *nsflg, char **stmestr, char **etmestr,
		      char **sdtestr, char **edtestr, char **exstr,
		      char **chnstr, char **chnstr_fix, int *freq_min,
		      int *freq_max, int *band_width, short int *strict_gs,
		      short int *tdiff_flag, double *tdiff, float *D_hmin,
		      float *D_hmax, float *E_hmax, float *F_hmax, int *nbms);
  int load_fit_update_fov(int fnum, int channel, int channel_fix, int old,
			  int tlen, double stime, double sdate, double etime,
			  double edate, double extime, unsigned char nsflg,
			  unsigned char vb, char *vbuf, char *iname,
			  char **dnames, short int tdiff_flag, double tdiff,
			  short int strict_gs, int freq_min, int freq_max,
			  int min_pnts, int D_nrg, int E_nrg, int F_nrg,
			  int far_nrg, int D_rgmax, int E_rgmax, int F_rgmax,
			  float D_hmin, float D_hmax, float E_hmax,
			  float F_hmax, float D_vh_box, float E_vh_box,
			  float F_vh_box, float far_vh_box, float max_hop,
			  struct FitMultBSID *mult_bsid);
  void test_ut_fov_struct(unsigned char vb, char *vbuf, int nbms,
			  float min_frac, double ut_box_sec, int D_nrg,
			  int E_nrg, int F_nrg, int far_nrg, int D_rgmax,
			  int E_rgmax, int F_rgmax, float D_hmin, float D_hmax,
			  float E_hmax, float F_hmax,
			  struct FitMultBSID *mult_bsid);

  /* Process the command line options */
  farg = command_options(argc, argv, &old, &tlen, &vb, &catflg, &nsflg,
			 &stmestr, &etmestr, &sdtestr, &edtestr, &exstr,
			 &chnstr, &chnstr_fix, &freq_min, &freq_max,
			 &band_width, &strict_gs, &tdiff_flag, &tdiff, &D_hmin,
			 &D_hmax, &E_hmax, &F_hmax, &nbms);

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
      dnames = (char **)malloc(sizeof(char *));

      if(argc-farg > 1)
	{
	  iname = argv[argc-1];
	  fnum  = argc - 2;
	}
      else fnum = argc - 1;

      len       = strlen(argv[fnum]);
      dnames[0] = (char *)malloc(sizeof(char) * (len + 1));
      strcpy(dnames[0], argv[fnum]);
      fnum      = 1;
    }
  else
    {
      /* For multiple input files, no index files are allowed */
      fnum   = argc - farg;
      dnames = (char **)malloc(sizeof(char*) * fnum);

      for(inum = 0; inum < fnum; inum++)
	{
	  len = strlen(argv[inum+argc]);
	  dnames[inum] = (char *)malloc(sizeof(char) * (len + 1));
	  strcpy(dnames[inum], argv[inum + argc]);
	}
    }

  /* Dynamically establish transmission frequency bands for these scans */
  /* unless a frequency range was specified                             */
  if(freq_max - freq_min == 27000)
    {
      nfbands = get_fit_tfreq_bands(fnum, channel, channel_fix, old, tlen,
				    stime, sdate, etime, edate, extime, 1,
				    nsflg, vb, vbuf, iname, dnames, band_width,
				    fbands, all_freq);
      if(nfbands < 0)
	{
	  fprintf(stderr, "make_fov ERROR: frequency band width too small");
	  return(1);
	}
    }
  else
    {
      nfbands = 1;
      fbands[0][0] = freq_min;
      fbands[0][1] = freq_max;
    }

  /* Initialize and load the fitted data */
  mult_bsid = FitMultBSIDMake();

  /* Treat each frequency band seperately */
  for(inum = 0; inum < nfbands; inum++)
    {
      /* Cycle through the scans, updating the backscatter data in each one */
      /* Based off of DaViTpy routine:                                      */
      /*   pydarn.proc.fov.update_backscatter.update_bs_w_scan              */
      
      ret_stat = load_fit_update_fov(fnum, channel, channel_fix, old, tlen,
				     stime, sdate, etime, edate, extime, nsflg,
				     vb, vbuf, iname, dnames, tdiff_flag, tdiff,
				     strict_gs, freq_min, freq_max, min_pnts,
				     D_nrg, E_nrg, F_nrg, far_nrg, D_rgmax,
				     E_rgmax, F_rgmax, D_hmin, D_hmax, E_hmax,
				     F_hmax, D_vh_box, E_vh_box, F_vh_box,
				     far_vh_box, max_hop, mult_bsid);

      if(ret_stat < 0 && mult_bsid->num_scans > 0)
      	/* Examine the UT evolution and consistency of the elevation angles */
      	test_ut_fov_struct(vb, vbuf, nbms, min_frac, ut_box_sec, D_nrg, E_nrg,
      			   F_nrg, far_nrg, D_rgmax, E_rgmax, F_rgmax, D_hmin,
      			   D_hmax, E_hmax, F_hmax, mult_bsid);
    }

  /* Write an output file */
  if(mult_bsid->num_scans > 0)
    WriteFitMultBSIDASCII(stdout, mult_bsid);

  /* Free the data structure pointer */
  FitMultBSIDFree(mult_bsid);

  return(0);
}
