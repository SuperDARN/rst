/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"makeall - Searches a source code tree looking for source code directories and compiles them.\n",
"makeall --help\n",
"makeall [-vb] [-a] [-q] src\n",
"makeall [-vb] [-a] [-q] src lst...\n",
"makeall [-vb] [-a] [-q] src stdin\n",

"--help\tprint the help message and exit.\n",
"-vb\tverbose. Log status to standard error in addition to standard out.\n",
"-a\tcompile all directories found, do not perform version checking to find the most recent.\n",
"-q\tquiet mode. Keep going if an error occurs.\n",
"src\tsource directory in which to search for source code.\n",
"lst...\tlist of source code directories to compile.\n",
"stdin\tread source code directories from standard input.\n",

NULL};
