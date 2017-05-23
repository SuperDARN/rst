/* FrameBufferClear.c
   ==================
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include "rfbuffer.h"


int main(int argc,char *argv[]) {

  struct FrameBuffer *img=NULL;

  float wdt=400,hgt=400;
 
  unsigned int fgcol=0xffffffff;
  unsigned int bgcol=0xff0000ff;


  img=FrameBufferMake("image",wdt,hgt,24);

  FrameBufferRectangle(img,NULL,20,20,wdt-40,hgt-40,0x01,
                    fgcol,0x0f,0,NULL,NULL);

  FrameBufferClear(img,bgcol,0x0f);

  FrameBufferRectangle(img,NULL,20,20,wdt-40,hgt-40,0x00,
                    fgcol,0x0f,0,NULL,NULL);


  FrameBufferSavePPM(img,stdout);

  return 0;
}

