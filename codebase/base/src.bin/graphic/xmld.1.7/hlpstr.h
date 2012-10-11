/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"xmld - Displays an XML image representation on an X-terminal.\n",
"xmld --help\n",
"xmld [-smooth] [-scale sf] [-check] [-bgcol rrggbb] [-display display] [-xoff xoff] [-yoff yoff] [name]\n",

"--help\tprint the help message and exit.\n",
"-smooth\tsmooth the image.\n",
"-scale scale\tpercentage scale factor to display image at.\n",
"-check\tplot the background as a chequer board.\n",
"-bgcol rrggbb\tset the background color of window to rrggbb, specified as the  hexadecimal value for the 24-bit red,green and blue component color.\n",
"-display display\tconnect to the xterminal named display.\n",
"-xoff xoff\topen the window, xoff pixels from the left edge of the screen.\n",
"-yoff yoff\topen the window ypad pixels from the top edge of the screen.\n",
"name\timage filename. If this is omitted the image  will be read from standard input.\n",

NULL};
