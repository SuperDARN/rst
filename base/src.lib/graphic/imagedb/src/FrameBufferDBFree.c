/* FrameBufferDBFree.c
   ===================
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
#include <string.h>
#include "rfbuffer.h"
#include "imagedb.h"

int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
} 


int main(int argc,char *argv[]) {

  char name[32];

  int wdt=400;
  int hgt=400;
  int n;

  struct FrameBuffer *out,*img;

  struct FrameBufferDB *imagedb;

  imagedb=FrameBufferDBMake();

  for (n=0;n<3;n++) {
    sprintf(name,"image%d",n);
    img=FrameBufferMake(name,10,10,24);

    FrameBufferClear(img,0xff000000,0x0f);
    FrameBufferEllipse(img,NULL,5,5,2+n,2+n,0,0xffff0000,0x0f,1,NULL,NULL);
    FrameBufferDBAdd(imagedb,img);

  }


  
  out=FrameBufferMake("image",wdt,hgt,24);

  img=FrameBufferDBFind(imagedb,"image0");
  if (img !=NULL) FrameBufferImage(out,NULL,img,0x0f,40,190,1,NULL);

  img=FrameBufferDBFind(imagedb,"image1");
  if (img !=NULL) FrameBufferImage(out,NULL,img,0x0f,190,190,1,NULL);

  img=FrameBufferDBFind(imagedb,"image2");
  if (img !=NULL) FrameBufferImage(out,NULL,img,0x0f,340,190,1,NULL);


  FrameBufferSavePPM(out,stdout);

  
  FrameBufferFree(out);
  FrameBufferDBFree(imagedb);
  

  return 0;
}
