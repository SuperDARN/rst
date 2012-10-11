/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"extract_grid - Extract statistics from a grid file.\n",
"extract_grid --help\n",
"extract_grid [-mid] [gridname]\n",
"extract_grid -new [-mid] [gridname]\n",

"--help\tprint the help message and exit.\n",
"-mid\trecord the time in the middle of the grid data, not the start and end times.\n",
"gridname\tfilename of the grd format file. If this is omitted, the file will be read from standard input.\n",
"-new\tthe input file is a grdmap format file.\n",

NULL};
