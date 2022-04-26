/* PostScriptImage
   ===============
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
#include "rps.h"

int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
} 


int main(int argc,char *argv[]) {

  struct FrameBuffer *img;
  struct PostScript *ps=NULL;

  float wdt=400,hgt=400;
  float xoff=50,yoff=50;

  int x,y;

  unsigned int bgcol=0xffffffff;
  unsigned int fgcol=0xff000000;


  img=FrameBufferMake("FrameBuffer",10,10,24);
  
  FrameBufferClear(img,bgcol,0x0f);

  FrameBufferEllipse(img,NULL,5,5,5,5,0,fgcol,0x0f,0,NULL,NULL);

  ps=PostScriptMake();
  PostScriptSetText(ps,stream,stdout);   
  PostScriptMakeDocument(ps,xoff,yoff,wdt,hgt,0);
  PostScriptMakePlot(ps);

  for (x=0;x<wdt;x+=10) {
    for (y=0;y<hgt;y+=10) {
      PostScriptImage(ps,NULL,img,0x0f,x,y,NULL);
    }
  }

  PostScriptEndPlot(ps);
  PostScriptEndDocument(ps);
  return 0;
}

