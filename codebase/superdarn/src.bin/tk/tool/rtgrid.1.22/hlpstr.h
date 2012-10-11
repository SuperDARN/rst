/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"rtgrid - Creates a grid or grdmap format file from a fitacf TCP/IP data stream.\n",
"rtgrid --help\n",
"rtgrid [options] host port\n",
"rtgrid -new [options] host port\n",

"--help\tprint the help message and exit.\n",
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
"-L logname\tlog connections and information in the file logname. By default, connections are recorded in log.rt.\n",
"-p pathname\tstore the daily files in the directory pathname.\n",
"-f filename\tstore the grid data in the file filename.\n",
"-if pidname\trecord the process Identifier (PID) of the server in the file pidname. By default, the PID is  recorded in pid.id.\n",
"-s stream\tfor a multiplexed stream, read the stream number given by stream. A value of 255 will read all streams\n",
"host\thostname or IP number of the system to connect to.\n",
"rport\tport number to connect to on the server.\n",
"-rpf\tThe remote port number is stored in a text file.\n",
"rportname\tfilename of the text file containing the remote port number.\n",
"-new\tproduce grdmap format output files.\n",

NULL};
