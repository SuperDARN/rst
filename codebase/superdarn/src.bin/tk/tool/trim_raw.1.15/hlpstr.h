/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"trim_raw - Extracts a section from a raw (dat) or rawacf format file.\n",
"trim_raw --help\n",
"trim_raw [-vb] [-t thr] [-sd yyyymmdd] [-st hr:mn] [-ed yyyymmdd] [-et hr:mn] [-ex hr:mn] rawinput rawoutput\n",
"trim_raw -new [-vb] [-t thr] [-sd yyyymmdd] [-st hr:mn] [-ed yyyymmdd] [-et hr:mn] [-ex hr:mn] [rawacfname]\n",

"--help\tprint the help message and exit.\n",
"-vb\tverbose. Log information to the console.\n",
"-t thr\tuse the threshold factor thr to detemine whether data is saved or not.\n",
"-sd yyyymmdd\textract starting from the date yyyymmdd.\n",
"-st hr:mn\textract starting from the time hr:mn.\n",
"-ed yyyymmdd\tstop extracting at the date yyyymmdd.\n",
"-et hr:mn\tstop extracting at the time hr:mn.\n",
"-ex hr:mn\textract an interval whose extent is hr:mn.\n",
"rawinput\tfilename of the raw (dat) format file to trim.\n",
"rawoutput\tfilename of the raw (dat) format file to create.\n",
"-new\tthe input file is a rawacf format file.\n",
"rawacfname\tfilename of the rawacf (dat) format file to trim. If this is omitted, the file will be read from standard input\n",

NULL};
