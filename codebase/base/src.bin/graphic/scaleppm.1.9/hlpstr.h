/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"scaleppm - Scale a Portable PixMap (PPM) image.\n",
"scaleppm --help\n",
"scaleppm [-smooth] [-wdt width] [-hgt height] [name]\n",
"scaleppm [-smooth] [-sf scale] [name]\n",

"--help\tprint the help message and exit.\n",
"-smooth\tsmooth the output image.\n",
"-wdt width\tmake the output image width pixels wide.\n",
"-hgt height\tmake the output image height pixels tall.\n",
"name\tinput image filename. If this is omitted the image  will be read from standard input.\n",
"-sf scale\tuse the scale factor scale where 1.0 is the original size.\n",

NULL};
