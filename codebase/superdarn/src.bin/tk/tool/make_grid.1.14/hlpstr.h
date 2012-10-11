/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"make_grid - Creates a grid format file from either a fit, fitacf or cfit format file.\n",
"make_grid --help\n",
"make_grid [options] fitname [inxname]\n",
"make_grid -new  [options] fitacfname [inxname]\n",
"make_grid -cfit [options] cfitname\n",
"make_grid -cfit -new [options] cfitname\n",
"make_grid -c [options] fitnames...\n",
"make_grid -c -new [options] fitacfnames...\n",
"make_grid -c -cfit [options] cfitnames...\n",
"make_grid -c -cfit -new [options] cfitnames...\n",

"--help\tprint the help message and exit.\n",
"-vb\tverbose. Log information to the console.\n",
"-sd yyyymmdd\tstart from the date yyyymmdd.\n",
"-st hr:mn\tstart from the time hr:mn.\n",
"-ed yyyymmdd\tstop at the date yyyymmdd.\n",
"-et hr:mn\tstop at the time hr:mn.\n",
"-ex hr:mn\tuse the interval whose extent is hr:mn.\n",
"-tl tlen\tignore the scan flag and instead use a fixed length scan of  tlen seconds.\n",
"-i intt\tintegrate the grid data into records of length intt seconds.\n",
"-cn channel\tprocess data from channel channel for stereo mode data.\n",
"-ebm ebeams\texclude the beams listed in ebeams, which is a comma separated list of beam numbers.\n",
"-minrng minrange\texclude data from range gates lower than minrange.\n",
"-maxrng maxrange\texclude data from range gates higher than maxrange.\n",
"-fwgt mode\tset the filter weighting mode to mode.\n",
"-pmax pmax\texclude data with a power greater than pmax.\n",
"-vmax vmax\texclude data with an absolute velocity greater than vmax.\n",
"-wmax wmax\texclude data with a spectral width greater than wmax.\n",
"-vemax vemax\texclude data with a velocity error greater than vemax.\n",
"-pmin pmin\texclude data with a power less than pmin.\n",
"-vmin vmin\texclude data with an absolute velocity less than vmin.\n",
"-wmin wmin\texclude data with a spectral width less than wmin.\n",
"-vemin vemin\texclude data with a velocity error less than vemin.\n",
"-fmax fmax\tset the maximum allowed variation in frequency to be fmax Hertz.\n",
"-alt alt\tset the altitude at which the mapping is done to be alt kilometers.\n",
"-nav\tdo not perform temporal averaging.\n",
"-nlm\tdo not exclude data because it exceeds the data limits.\n",
"-nb\tdo not exclude data based on the operating parameters.\n",
"-xtd\tcreate extended output that includes both the power and spectral width measurements.\n",
"-ion\texclude data marked as ground scatter.\n",
"-gs\texclude data marked as ionospheric scatter.\n",
"-both\tdo not exclude data based on the type flag.\n",
"-inertial\tcalculate the grid file in the inertial reference frame.\n",
"fitname\tfilename of the fit format file.\n",
"inxname\tfilename of the index file.\n",
"-new\tthe input file is in the fitacf format, and the output should be in grdmap format. (If the input format is in cfit format as specified by the -cfit option, then the output will be in grdmap format).\n",
"fitacfname\tfilename of the fitacf format file.\n",
"-cfit\tthe input file is in the cfit format.\n",
"cfitfname\tfilename of the cfit format file.\n",
"-c\tconcatenate multiple input files.\n",
"fitnames\tfilenames of the fit format files.\n",
"fitacfnames\tfilenames of the fitacf format files.\n",
"cfitnames\tfilenames of the cfit format files.\n",

NULL};
