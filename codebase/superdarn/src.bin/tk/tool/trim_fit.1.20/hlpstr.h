/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"trim_fit - Extracts a section from a fit or fitacf format file.\n",
"trim_fit --help\n",
"trim_fit [-vb] [-sd yyyymmdd] [-st hr:mn] [-ed yyyymmdd] [-et hr:mn] [-ex hr:mn]  [-cp cpid] [-cn channel] fitinput fitoutput\n",
"trim_fit -i [-vb] [-sd yyyymmdd] [-st hr:mn] [-ed yyyymmdd] [-et hr:mn] [-ex hr:mn]  [-cp cpid] [-cn channel] fitinput inxinput fitoutput inxoutput\n",
"trim_fit -new [-vb] [-sd yyyymmdd] [-st hr:mn] [-ed yyyymmdd] [-et hr:mn] [-ex hr:mn]  [-cp cpid] [-cn channel] [fitacfname]\n",
"trim_fit -i -new [-vb] [-sd yyyymmdd] [-st hr:mn] [-ed yyyymmdd] [-et hr:mn] [-ex hr:mn]  [-cp cpid] [-cn channel] fitacfname inxname \n",

"--help\tprint the help message and exit.\n",
"-vb\tverbose. Log information to the console.\n",
"-sd yyyymmdd\textract starting from the date yyyymmdd.\n",
"-st hr:mn\textract starting from the time hr:mn.\n",
"-ed yyyymmdd\tstop extracting at the date yyyymmdd.\n",
"-et hr:mn\tstop extracting at the time hr:mn.\n",
"-ex hr:mn\textract an interval whose extent is hr:mn.\n",
"-cp cp\textract only those records with the program identifier cp.\n",
"-cn channel\textract only those records from channel channel.\n",
"fitinput\tfilename of the fit format file to trim.\n",
"fitoutput\tfilename of the fit format file to create.\n",
"-i\tindex files are available, for fit format files, an index file is created for the ouput file.\n",
"inxinput\tfilename of the index file associated with the fit or fitacf format input file.\n",
"inxoutput\tfilename of the index file to create.\n",
"-new\tthe input file is a fitacf format file.\n",
"fitacfname\tfilename of the fitacf (dat) format file to trim. If this is omitted, the file will be read from standard input\n",

NULL};
