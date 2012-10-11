/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"trim_map - Extracts a section from a map format file.\n",
"trim_map --help\n",
"trim_map [-vb] [-new] [-sd yyyymmdd] [-st hr:mn] [-ed yyyymmdd] [-et hr:mn] [-ex hr:mn] name\n",

"--help\tprint the help message and exit.\n",
"-vb\tverbose. Log information to the console.\n",
"-new\tthe input file is a cnvmap format file.\n",
"-sd yyyymmdd\textract starting from the date yyyymmdd.\n",
"-st hr:mn\textract starting from the time hr:mn.\n",
"-ed yyyymmdd\tstop extracting at the date yyyymmdd.\n",
"-et hr:mn\tstop extracting at the time hr:mn.\n",
"-ex hr:mn\textract an interval whose extent is hr:mn.\n",
"name\tfilename of the map format file to trim. If this is omitted, the file is read from standard input.\n",

NULL};
