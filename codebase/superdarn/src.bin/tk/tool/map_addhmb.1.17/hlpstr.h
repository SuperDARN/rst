/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"map_addhmb - Calculates the position of the Heppner Maynard Boundary and adds it to a map format file.\n",
"map_addhmb --help\n",
"map_addhmb [-vb] [-new] [-nodef] [-cnt count] [-vel vmin] [-ex exlist] [mapname]\n",
"map_addhmb -t [-vb] [-new] [-cnt count] [-vel vmin] [-ex exlist] [mapname]\n",
"map_addhmb -lf latname [-vb] [-new] [mapname]\n",
"map_addhmb -lat latmin [-vb] [-new] [mapname]\n",

"--help\tprint the help message and exit.\n",
"-vb\tverbose. Log information to the console.\n",
"-new\tthe input file is a cnvmap format file.\n",
"-nodef\tdo not use a default boundary if there is insufficient data in the map file to find one. Boundary is instead determined by model pattern in map_addmodel procedure.\n",
"-cnt count\tset the number of scatter points to get a match to be count.\n",
"-vel vmin\tset the minimum velocity to  vmin.\n",
"-nodef\tdo not use a default boundary if there is insufficient data in the map file to find one. Boundary is instead determined by model pattern in map_addmodel procedure.\n",
"-ex exlist\texclude data from the stations listed in  exlist, which is a comma separated list of radar station numbers.\n",
"mapname\tfilename of the map format file. If this is omitted, the file will be read from standard input.\n",
"-t\tinstead of adding the calculated boundary to the map file, write the record time and the calculated location as ASCII text to standard output.\n",
"-lf latname\tdo not calculate the boundary, instead read it from the ASCII text file latname.\n",
"-lat latmin\tdo not calculate the boundary, instead set the lower latitude boundary to latmin degrees.\n",

NULL};
