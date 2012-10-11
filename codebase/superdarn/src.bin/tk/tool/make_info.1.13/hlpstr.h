/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"make_info - Extract information (metadata) from fit, fitacf or cfit format files.\n",
"make_info --help\n",
"make_info fitnames...\n",
"make_info -new [fitacfname]\n",
"make_info -cfit cfitnames...\n",

"--help\tprint the help message and exit.\n",
"fitnames\tfilenames of the fit format files.\n",
"-new\tfiles are in the fitacf format.\n",
"fitacfname\tfilename of the fitacf  format file. If this is omitted the file is read from standard input.\n",
"-cfit\tfiles are in the cfit format.\n",
"cfitnames\tfilenames of the cfit format files.\n",

NULL};
