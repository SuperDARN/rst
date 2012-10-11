/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"ppmxtoppm - Convert an extended Portable PixMap (PPMX) image to Portable PixMap (PPM).\n",
"ppmxtoppm --help\n",
"ppmxtoppm [-bgcol rrggbb] [-a alphaname] [name]\n",

"--help\tprint the help message and exit.\n",
"-bgcol rrggbb\tset the background color of the output image to rrggbb, specified as the  hexadecimal value for the 24-bit red,green and blue component color.\n",
"-alpha alphaname\tsave the alpha channel of the file as the file alphaname.\n",
"name\tinput image filename. If this is omitted the image  will be read from standard input.\n",

NULL};
