/* FrameBufferClear.c
   ==================
   Author: R.J.Barnes
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:




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

