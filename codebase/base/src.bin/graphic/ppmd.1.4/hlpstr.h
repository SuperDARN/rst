/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"ppmd - Displays an extended Portable PixMap (PPMX) on an X-terminal.\n",
"ppmd --help\n",
"ppmd [-smooth] [-scale sf] [-wdt width] [-hgt height] [-display display] [-xoff xoff] [-yoff yoff] [name]\n",

"--help\tprint the help message and exit.\n",
"-smooth\tsmooth the image.\n",
"-scale scale\tpercentage scale factor to display image at.\n",
"-wdt width\tset the width of the image to width pixels.\n",
"-hgt height\tset the height of the image to height pixels.\n",
"-display display\tconnect to the xterminal named display.\n",
"-xoff xoff\topen the window, xoff pixels from the left edge of the screen.\n",
"-yoff yoff\topen the window ypad pixels from the top edge of the screen.\n",
"name\timage filename. If this is omitted the image  will be read from standard input.\n",

NULL};
