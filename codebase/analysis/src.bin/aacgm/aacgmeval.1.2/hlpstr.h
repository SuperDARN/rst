/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"aacgmeval - Convert to and from Altitude Adjusted Corrected Geo-Magnetic Coordinates (AACGM).\n",
"aacgmeval --help\n",
"aacgmeval [-i] [-lat lat] [-lon lon] [-alt alt] [-fmt format]\n",
"aacgmeval [-i] [-f filename]  [-fmt format]\n",

"--help\tprint the help message and exit.\n",
"-i\tperform the inverse conversion from AACGM to geographic.\n",
"-lat lat\tcalculate for the latitude lat.\n",
"-lon lon\tcalculate for the longitude lon.\n",
"-alt mlon\tcalculate for the altitude in kilometers, e alt.\n",
"-fmt format\tuse the string fmt, to format the output.\n",
"-f filename\tread a sequence of dates, times and longitudes from the ASCII file, filename.\n",

NULL};
