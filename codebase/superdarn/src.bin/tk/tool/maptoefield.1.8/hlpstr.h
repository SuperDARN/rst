/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"maptoefield - Calculate the electric field and other parameters from convection map data.Calculate the electric field and other parameters from convection map data.\n",
"maptoefield --help\n",
"maptoefield [-vb] [-new] [-sd yyyymmdd] [-st hr:mn] [-ed yyyymmdd] [-et hr:mn] [-ex hr:mn] [-s step] [-l latmin [-mlt] [-p] [-v] name\n",

"--help\tprint the help message and exit.\n",
"-vb\tverbose. Log information to the console.\n",
"-new\tthe input file is a cnvmap format file.\n",
"-sd yyyymmdd\textract starting from the date yyyymmdd.\n",
"-st hr:mn\textract starting from the time hr:mn.\n",
"-ed yyyymmdd\tstop extracting at the date yyyymmdd.\n",
"-et hr:mn\tstop extracting at the time hr:mn.\n",
"-ex hr:mn\textract an interval whose extent is hr:mn.\n",
"-s step\tskip step number of records between each record extracted.\n",
"-l latmin\tset the lower latitude of the grid to latmin degrees.\n",
"-mlt\tinclude the magnetic local time in the output.\n",
"-p\tinclude the potential in the output.\n",
"-v\tinclude the derived velocity int the output.\n",
"name\tfilename of the convection map file to process. If this is omitted, the file is read from standard input.\n",

NULL};
