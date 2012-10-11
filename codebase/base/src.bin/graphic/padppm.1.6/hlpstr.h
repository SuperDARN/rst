/* hlpstr.h
   ========
   Author: R.J.Barnes
*/

/*
 $License$
*/


char *hlpstr[]={
"padppm - Pad the edges of a Portable PixMap (PPM) image.\n",
"padppm --help\n",
"padppm [-wdt width] [-hgt height] [-xpad xpad] [-ypad ypad] [-bgcol rrggbb] [name]\n",

"--help\tprint the help message and exit.\n",
"-wdt width\tmake the output image width pixels wide.\n",
"-hgt height\tmake the output image height pixels tall.\n",
"-xpad xpad\tplace the input image, xpad pixels from the left edge of the output image.\n",
"-ypad ypad\tplace the input image ypad pixels from the top edge of the output image.\n",
"-bgcol rrggbb\tset the background color of the output image to rrggbb, specified as the  hexadecimal value for the 24-bit red,green and blue component color.\n",
"name\tinput image filename. If this is omitted the image  will be read from standard input.\n",

NULL};
