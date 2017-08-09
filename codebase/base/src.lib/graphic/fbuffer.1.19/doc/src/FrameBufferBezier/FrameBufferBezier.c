/* FrameBufferBezier.c
   ===================
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include "rfbuffer.h"


int main(int argc,char *argv[]) {

  struct FrameBuffer *img=NULL;

  float wdt=400,hgt=400;
 
  unsigned int fgcol=0xffffffff;

  img=FrameBufferMake("image",wdt,hgt,24);

  FrameBufferBezier(img,20,20,0,hgt/2,wdt,hgt/2,wdt-20,hgt-20,0.01,
                    fgcol,0x0f,1,NULL,NULL);

  FrameBufferSavePPM(img,stdout);

  return 0;
}

