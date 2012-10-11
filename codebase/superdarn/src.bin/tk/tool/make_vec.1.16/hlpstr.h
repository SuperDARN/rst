/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"make_vec - Creates a vec format file from a grd format or grdmap format file.\n",
"make_vec --help\n",
"make_vec [-vb] [-i stid] name\n",
"make_vec -new [-vb] [-i stid] name\n",

"--help\tprint the help message and exit.\n",
"-vb\tverbose. Log information to the console.\n",
"-i stid\tcreate the vec data for the radar station with identifier stid.\n",
"name\tfilename of the grd format file.\n",
"-new\tthe input file is a grdmap format file.\n",

NULL};
