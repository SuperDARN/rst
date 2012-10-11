/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"trim_grid - Extracts a section from a grid or grdmap format file.\n",
"trim_grid --help\n",
"trim_grid [-vb] [-sd yyyymmdd] [-st hr:mn] [-ed yyyymmdd] [-et hr:mn] [-ex hr:mn] [-cn channel] [-exc stid...] [-i intt] name\n",
"trim_grid -new [-vb] [-sd yyyymmdd] [-st hr:mn] [-ed yyyymmdd] [-et hr:mn] [-ex hr:mn] [-cn channel] [-exc stid...] [-i intt] name\n",

"--help\tprint the help message and exit.\n",
"-vb\tverbose. Log information to the console.\n",
"-sd yyyymmdd\textract starting from the date yyyymmdd.\n",
"-st hr:mn\textract starting from the time hr:mn.\n",
"-ed yyyymmdd\tstop extracting at the date yyyymmdd.\n",
"-et hr:mn\tstop extracting at the time hr:mn.\n",
"-ex hr:mn\textract an interval whose extent is hr:mn.\n",
"-cn channel\textract only those records from channel channel.\n",
"-exc stid...\texclude data from the stations listed in stid which is a comma separated list of station identifier codes or numbers.\n",
"-i intt\tre-integrate the data so that each record is intt seconds in length.\n",
"name\tfilename of the grid format file to trim. If this is omitted, the file is read from standard input.\n",
"-new\tthe input file is a grdmap format file.\n",

NULL};
