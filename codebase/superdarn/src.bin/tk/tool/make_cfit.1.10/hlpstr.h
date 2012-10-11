/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"make_cfit - Creates a cfit format file from either a fit or fitacf format file.\n",
"make_cfit --help\n",
"make_cfit [-vb] [-p minpwr] fitname\n",
"make_cfit -new [-vb] [-p minpwr] [fitacfname]\n",

"--help\tprint the help message and exit.\n",
"-vb\tverbose. Log information to the console.\n",
"-p minpwr\tfilter out data with values of lag-zero power less than minpwr.\n",
"fitname\tfilename of the fit format file.\n",
"-new\tthe input file is in the fitacf format.\n",
"fitacfname\tfilename of the fitacf format file.\n",

NULL};
