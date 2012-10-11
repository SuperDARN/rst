/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"mlteval - Find Magnetic Local Time (MLT).\n",
"mlteval --help\n",
"mlteval [-d yyyymmdd] [-t hr:mn] [-l mlon] [-fmt format]\n",
"mlteval [-f filename]  [-fmt format]\n",

"--help\tprint the help message and exit.\n",
"-d yyyymmdd\tcalculate for the date yyyymmdd.\n",
"-t hr:mn\tcalculate for the time hr:mn.\n",
"-l mlon\tcalculate for the magnetic longitude mlon.\n",
"-fmt format\tuse the string fmt, to format the output.\n",
"-f filename\tread a sequence of dates, times and longitudes from the ASCII file, filename.\n",

NULL};
