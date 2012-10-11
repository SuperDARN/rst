/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"trim_cfit - Extracts a section from a cfit format file.\n",
"trim_cfit --help\n",
"trim_cfit [-vb] [-sd yyyymmdd] [-st hr:mn] [-ed yyyymmdd] [-et hr:mn] [-ex hr:mn] [-cp cpid] [-cn channel] name\n",

"--help\tprint the help message and exit.\n",
"-vb\tverbose. Log information to the console.\n",
"-sd yyyymmdd\textract starting from the date yyyymmdd.\n",
"-st hr:mn\textract starting from the time hr:mn.\n",
"-ed yyyymmdd\tstop extracting at the date yyyymmdd.\n",
"-et hr:mn\tstop extracting at the time hr:mn.\n",
"-ex hr:mn\textract an interval whose extent is hr:mn.\n",
"-cp cp\textract only those records with the program identifier cp.\n",
"-cn channel\textract only those records from channel channel.\n",
"name\tfilename of the cfit format file to trim. If this is omitted, the file is read from standard input.\n",

NULL};
