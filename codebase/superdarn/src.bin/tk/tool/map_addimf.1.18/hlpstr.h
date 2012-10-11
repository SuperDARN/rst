/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"map_addimf - Adds IMF data to a convection map file.\n",
"map_addimf --help\n",
"map_addimf [-vb] [-new] [-ace] [-wind] [-p path] [-d hr:mn] [-ex hr:mn] [mapname]\n",
"map_addimf [-vb] [-new] [-ace] [-wind] [-p path] [-df delayname] [-ex hr:mn] [mapname]\n",
"map_addimf [-vb] [-new] [-ace] [-if imfname] [-d hr:mn] [mapname]\n",
"map_addimf [-vb] [-new] [-ace] [-if imfname] [-df delayname] [mapname]\n",
"map_addimf [-vb] [-new] [-ace] [-bx bx] [-by by] [-bz bz</bz>] [mapname]\n",

"--help\tprint the help message and exit.\n",
"-vb\tverbose. Log information to the console.\n",
"-new\tthe input file is a cnvmap format file.\n",
"-ace\tuse IMF data from ACE.\n",
"-wind\tuse IMF data from wind.\n",
"-p pathname\tset the directory path to search for the IMF data in to pathname.\n",
"-d hr:mn\tset the IMF delay time to  hr:mn.\n",
"-ex hr:mn\tread hr hours and mn minutes of IMF data. By default, 24 hours of data is read.\n",
"mapname\tfilename of the map format file. If this is omitted, the file will be read from standard input.\n",
"-df delayname\tread the delay times from the ASCII text file delayname.\n",
"-if imfname\tread the IMF data from the ASCII text file imfname.\n",
"-bx bx\tset the X component of the magnetic field to bx.\n",
"-by bx\tset the Y component of the magnetic field to by.\n",
"-bz bx\tset the Z component of the magnetic field to bz.\n",

NULL};
