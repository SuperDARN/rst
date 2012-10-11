/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"vec_plot - Plot vec format files.\n",
"vec_plot --help\n",
"vec_plot [options] names...\n",
"vec_plot -ps [-xp xoff] [-yp yoff] [options] names...\n",
"vec_plot -ppm [options] names...\n",
"vec_plot -ppmx [options] names...\n",
"vec_plot -xml [options] names...\n",
"vec_plot -x [-display display] [-xoff xoff] [-yoff yoff] [options] names..\n",

"--help\tprint the help message and exit.\n",
"-cf cfname\tread command line options from the file cfname.\n",
"-wdt width\tset the width of the plot to width.\n",
"-logo\tadd a credit and logo to the plot.\n",
"-p panel\tset the number of panels in the plot to panel.\n",
"-bgcol aarrggbb\tset the background color to aarrggbb, specified as the hexadecimal value for the 32-bit alpha,red,green and blue component color.\n",
"-txtcol aarrggbb\tset the color of the text to aarrggbb, specified as the hexadecimal value for the 32-bit alpha,red,green and blue component color.\n",
"-grdcol aarrggbb\tset the color of the grid to aarrggbb, specified as the hexadecimal value for the 32-bit alpha,red,green and blue component color.\n",
"name...\tthe filenames of the vec format files to plot.\n",
"-ps\tproduce a PostScript plot as the output.\n",
"-xp xoff\tset the X offset of the PostScript plot to xoff.\n",
"-yp xoff\tset the Y offset of the PostScript plot to yoff.\n",
"-ppm\tproduce a Portable PixMap (PPM) image as the output.\n",
"-ppmx\tproduce an extended Portable PixMap (PPMX) image as the output.\n",
"-xml\tproduce an XML image representation as the output.\n",
"-x\tplot the data on an X-terminal.\n",
"-display display\tconnect to the xterminal named display.\n",
"-xoff xoff\topen the window, xoff pixels from the left edge of the screen.\n",
"-yoff yoff\topen the window ypad pixels from the top edge of the screen.\n",

NULL};
