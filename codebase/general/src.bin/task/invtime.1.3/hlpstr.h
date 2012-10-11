/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"invtime - Convert back from second of epoch or second of day to regular time.\n",
"invtime --help\n",
"invtime [-c columns...] [name]\n",
"invtime -d -sd yyyymmdd [-c columns...] [name]\n",

"--help\tprint the help message and exit.\n",
"-c columns\tprint only the columns given by the comma separated list columns.\n",
"-d\tthe time is given as seconds of day.\n",
"-sd yyyymmdd\tset the start date to the value of yyyymmdd.\n",
"name\tfilename of the ASCII file, if this is omitted the file will be read from standard input.\n",

NULL};
