/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"map_addmodel - Calculates the statistical model and adds it to a convection map file.\n",
"map_addmodel --help\n",
"map_addmodel [-vb] [-new] [-o order] [-d doping] [mapname]\n",

"--help\tprint the help message and exit.\n",
"-vb\tverbose. Log information to the console.\n",
"-new\tthe input file is a cnvmap format file.\n",
"-o order\tset the order of fit to be order.\n",
"-d doping\tset the doping level to dp. Possible values are low, medium, or high.\n",
"mapname\tfilename of the map format file. If this is omitted, the file will be read from standard input.\n",

NULL};
