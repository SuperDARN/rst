/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"radar_id - Print out information about the radar sites.\n",
"radar_id --help\n",
"radar_id [-vb] [-n] [-s] [-o] [-h] [-d yyyymmdd] [-t hr:mn] stid\n",
"radar_id -c [-vb] [-n] [-s] [-o] [-h] [-d yyyymmdd] [-t hr:mn] stnum\n",

"--help\tprint the help message and exit.\n",
"-vb\tverbose. Print out the extra information describing the output.\n",
"-n\tprint the station name.\n",
"-s\tprint the operational status of the station.\n",
"-o\tprint the name of the operator of the station.\n",
"-h\tprint the geographic (longitude,latitude), location of the station.\n",
"-d yyyymmdd\tget the hardware information for the date yyyymmdd.\n",
"-t hr:mn\tget the hardware information for the time hr:mn.\n",
"stid\tstation identifier code.\n",
"-c\tget the station identifier code, not the station number (assumes that the station number is given on the command line).\n",
"stnum\tstation number.\n",

NULL};
