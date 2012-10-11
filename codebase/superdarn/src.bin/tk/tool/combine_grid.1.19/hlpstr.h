/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"combine_grid - Combines together grid files.\n",
"combine_grid --help\n",
"combine_grid [-vb] [-r] gridnames...\n",
"combine_grid -new [-vb] [-r] gridnames...\n",

"--help\tprint the help message and exit.\n",
"-vb\tverbose. Log information to the console.\n",
"-r\tcombine with replacement.As each input record is combined together to form the output, a check is made to see if any of the data is from a station already included. If a duplicate set of vectors is found they will replace the existing vectors in the output.\n",
"gridnames\tfilenames of the grd format files.\n",
"-new\tthe input files are in the grdmap format file.\n",

NULL};
