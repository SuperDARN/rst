/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"test_raw - Tests the integrity of raw (dat) or rawacf format files by reading them and printing the contents.\n",
"test_raw --help\n",
"test_raw rawnames...\n",
"test_raw -new [rawacfname]\n",

"--help\tprint the help message and exit.\n",
"rawnames\tfilenames of the raw (dat) format files.\n",
"-new\tfiles are in the rawacf format.\n",
"rawacfname\tfilename of the rawacf  format file. If this is omitted the file is read from standard input.\n",

NULL};
