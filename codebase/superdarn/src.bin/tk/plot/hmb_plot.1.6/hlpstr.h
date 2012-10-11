/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"hmb_plot - Plot the Heppner-Maynard boundary from a map format files.\n",
"hmb_plot --help\n",
"hmb_plot [options] name\n",
"hmb_plot -ps [-l] [-xp xoff] [-yp yoff] [options] name\n",
"hmb_plot -ppm  [options] name\n",
"hmb_plot -ppmx [options] name\n",
"hmb_plot -xml [options] name\n",
"hmb_plot -x [-display display] [-xoff xoff] [-yoff yoff] [options] name\n",

"--help\tprint the help message and exit.\n",
"-cf cfname\tread command line options from the file cfname.\n",
"-new\tthe input file is a cnvmap format file.\n",
"-vb\tverbose. Log information to the console.\n",
"-w width\tset the width of the plot to width.\n",
"-h height\tset the height of the plot to height.\n",
"-ex hr:mn\tplot an interval whose extent is hr:mn.\n",
"name\tthe filename of the map format file to plot.\n",
"-ps\tproduce a PostScript plot as the output.\n",
"-xp xoff\tset the X offset of the PostScript plot to xoff.\n",
"-yp xoff\tset the Y offset of the PostScript plot to yoff.\n",
"-l\tset the orientation of the PostScript plot to landscape,\n",
"-ppm\tproduce a Portable PixMap (PPM) image as the output.\n",
"-ppmx\tproduce an extended Portable PixMap (PPMX) image as the output.\n",
"-xml\tproduce an XML image representation as the output.\n",
"-x\tplot the data on an X-terminal.\n",
"-display display\tconnect to the xterminal named display.\n",
"-xoff xoff\topen the window, xoff pixels from the left edge of the screen.\n",
"-yoff yoff\topen the window ypad pixels from the top edge of the screen.\n",

NULL};
