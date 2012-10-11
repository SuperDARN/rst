/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"ppmtoxml - Convert a Portable PixMap (PPM) image to an XML representation.\n",
"ppmtoxml --help\n",
"ppmtoxml [-name iname] [name]\n",
"ppmtoxml -alpha alpha [-name iname] [name]\n",

"--help\tprint the help message and exit.\n",
"-name iname\tset the image name in the output image to iname.\n",
"-alpha alpha\tset alpha channel for all pixels to thvalue given by alpha. The value can be given as a decimal, percentage or hexadecimal.\n",
"name\tinput image filename. If this is omitted the image  will be read from standard input.\n",

NULL};
