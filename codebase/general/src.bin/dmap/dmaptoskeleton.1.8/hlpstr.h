/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"dmaptoskeleton - Scans a DataMap file to produce a Skeleton table that can be used to create a Common Data Format (CDF) file.\n",
"dmaptoskeleton --help\n",
"dmaptoskeleton [-vb] datamap skeleton map\n",

"--help\tprint the help message and exit.\n",
"-vb\tverbose. Log information to the console.\n",
"datamap\tfilename of the DataMap file to scan.\n",
"skeleton\tfilename of the skeleton table to generate.\n",
"map\tfilename of mapping that relates variables in the DataMap file to those in the skeleton table.\n",

NULL};
