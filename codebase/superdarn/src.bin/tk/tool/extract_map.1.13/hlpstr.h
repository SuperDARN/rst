/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"extract_map - Extract statistics or the original grid file from a convection map data file.\n",
"extract_map --help\n",
"extract_map [-new] [-mid] [name]\n",
"extract_map -p [-new] [-mid] [name]\n",
"extract_map -s [-new] [-mid] [name]\n",
"extract_map -l [-new] [-mid] [name]\n",

"--help\tprint the help message and exit.\n",
"-new\tthe input file is a cnvmap format file.\n",
"-mid\trecord the time in the middle of the map data, not the start and end times.\n",
"name\tfilename of the map format file. If this is omitted, the file will be read from standard input.\n",
"-p\textract the cross polar cap potential, the IMF, the statistical model, and the scatter and error statistics.\n",
"-s\textract the scatter statistics.\n",
"-l\textract the lower latitude boundary.\n",

NULL};
