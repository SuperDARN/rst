/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"make_smr - Creates a smr format file from either a fit or fitacf format file.\n",
"make_smr --help\n",
"make_smr [-vb] [-t minpwr] [-b beams...] fitnames...\n",
"make_smr -new [-vb] [-t minpwr] [-b beams...] [fitacfname]\n",

"--help\tprint the help message and exit.\n",
"-vb\tverbose. Log information to the console.\n",
"-t minpwr\tfilter out data with values of lag-zero power less than minpwr.\n",
"-b beams\trecord the beams specified by beams in the smr file. The list of beams is comma separated.\n",
"fitnames...\tfilenames of the fit format file.\n",
"-new\tthe input file is in the fitacf format.\n",
"fitacfname\tfilename of the fitacf format file.\n",

NULL};
