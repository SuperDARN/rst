/* color.c
   ======= 
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include "rfbuffer.h"



unsigned int FrameBufferColor(unsigned int r,unsigned int g,unsigned int b,
                              unsigned int a) {
  unsigned int c;
  c=(a<<24) | (r<<16) | (g<<8) | b;
  return c;
}

