/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"map_grd - Reformats a grid file into an empty map format file.\n",
"map_grd --help\n",
"map_grd [-vb] [-new] [-sh] [-l latmin] [-s latshft] [gridname]\n",

"--help\tprint the help message and exit.\n",
"-vb\tverbose. Log information to the console.\n",
"-new\tthe input file is a grdmap format file and the output is a cnvmap format file..\n",
"-sh\tthe map file is for southern hemisphere data.\n",
"-l latmin\tset the lower latitude boundary to latmin.\n",
"-s latshft\tset the pole shift to latshft.\n",
"gridname\tfilename of the grd format file. If this is omitted, the file will be read from standard input.\n",

NULL};
