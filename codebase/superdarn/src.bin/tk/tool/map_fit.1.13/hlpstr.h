/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"map_fit - None\n",
"map_fit --help\n",
"map_fit [-vb] [-new] [-ew errwgt] [-mw modelwgt] [-s source] [mapname]\n",

"--help\tprint the help message and exit.\n",
"-vb\tverbose. Log information to the console.\n",
"-new\tthe input file is a cnvmap format file.\n",
"-ew errwgt\tset the error weighting to errwgt, which can be either yes or no.\n",
"-mw modelwgt\tset the model weighting to errwgt, which can be either yes or no.\n",
"-s source\tset the text string indicating the data source  to source.\n",
"mapname\tfilename of the map format file. If this is omitted, the file will be read from standard input.\n",

NULL};
