/* fitmultbsidtoascii.c
   ====================
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

#include "fitmultbsid.h"
#include "option.h"
#include "hlpstr.h"
#include "errstr.h"

struct OptionData opt;

/**
 * Outputs an error statement for an unrecognized input option
 **/

int rst_opterr(char *txt)
{
    fprintf(stderr, "Option not recognized: %s\n", txt);
    fprintf(stderr, "Please try: fitmultbsidtoascii --help\n");
    return(-1);
}

/**
 * Load the command line options
 **/

int command_options(int argc, char *argv[])
{
  /* Initialize input options */
  int farg=0;
  unsigned char help=0, option=0, version=0;

  int rst_opterr(char *txt);

  /* If only information is desired, print it out and exit */
  OptionAdd(&opt, "-help", 'x', &help);
  OptionAdd(&opt, "-option", 'x', &option);
  OptionAdd(&opt, "-version", 'x', &version);

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
 * Reads in a binary FitMultBSID file and outputs an ASCII file
 **/

int main(int argc, char *argv[])
{
  int inum, len, fnum, farg, status;

  FILE *fp=NULL;

  struct FitMultBSID *mult_bsid;

  /* Initialize file information */
  char **dnames=NULL;

  /* Declare local subroutines */
  int command_options(int argc, char *argv[]);

  /* Process the command line options */
  farg = command_options(argc, argv);

  /* Make a list of input files */
  fnum = argc - farg;
  if(fnum == 0)
    {
      fprintf(stderr, "must provide input file(s) to read\n");
      exit(1);
    }
  else
    {
      dnames = (char **)malloc(sizeof(char *) * fnum);

      for(inum = 0; inum < fnum; inum++)
	{
	  len = strlen(argv[inum + argc - 1]);
	  dnames[inum] = (char *)malloc(sizeof(char) * (len + 1));
	  strcpy(dnames[inum], argv[inum + argc - 1]);
	}
    }

  /* Initialize and load the binary data */
  mult_bsid = FitMultBSIDMake();

  /* Read in each file */
  for(status = 0, inum = 0; inum < fnum && status >= 0; inum++)
    {
      if((fp = fopen(dnames[inum], "r")) == NULL)
	{
	  fprintf(stderr, "unable to open input file [%s]\n", dnames[inum]);
	  exit(1);
	}
      status = ReadFitMultBSIDBin(fp, mult_bsid);
    }

  /* Write the output in ASCII format */
  if(mult_bsid->num_scans > 0)
    WriteFitMultBSIDASCII(stdout, mult_bsid);

  /* Free the data structure pointer */
  FitMultBSIDFree(mult_bsid);

  /* Free the remaining pointers */
  for(inum = 0; inum < fnum; inum++) free(dnames[inum]);
  free(dnames);

  return(status);
}
