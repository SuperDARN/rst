/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"ppmtoppmx - Convert a Portable PixMap (PPM) image to an extended Portable PixMap (PPMX) image.\n",
"ppmtoppmx --help\n",
"ppmtoppmx [-name iname] [name]\n",
"ppmtoppmx -alpha alpha [-name iname] [name]\n",
"ppmtoppmx -nz [-name iname] [name]\n",

"--help\tprint the help message and exit.\n",
"-name iname\tset the image name in the output image to iname.\n",
"-alpha alpha\tset alpha channel for all pixels to the value given by alpha. The value can be given as a decimal, percentage or hexadecimal.\n",
"-nz\tset alpha channel to full on (0xff) for any non-zero value pixel in the input image. \n",
"name\tinput image filename. If this is omitted the image  will be read from standard input.\n",

NULL};
