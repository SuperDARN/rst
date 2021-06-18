/* FrameBufferFontDBFind.c
   =======================
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
#include "fontdb.h"

int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
} 


int main(int argc,char *argv[]) {

  char *txt={"Hello World"};

  int wdt=400;
  int hgt=400;

  struct FrameBuffer *fb=NULL;

  struct FrameBufferFontDB *fontdb=NULL;
  struct FrameBufferFont *font;

  FILE *fontfp=NULL;
  char *fntdbfname=NULL;

  char *fontname="Helvetica";
  float fontsize=34.0;

  fntdbfname=getenv("FONTDB");
  fontfp=fopen(fntdbfname,"r");
  if (fontfp !=NULL) {
   fontdb=FrameBufferFontDBLoad(fontfp);
   fclose(fontfp);
  }
 
  if (fontdb==NULL) {
   fprintf(stderr,"Could not load fonts.\n");
   exit(-1);
  }

  
  fb=FrameBufferMake("image",wdt,hgt,24);

  font=FrameBufferFontDBFind(fontdb,fontname,fontsize);


  FrameBufferText(fb,NULL,font,20,200,strlen(txt),
                  txt,0xffffffff,0x0f,1,NULL);

  FrameBufferSavePPM(fb,stdout);
  FrameBufferFree(fb);

  FrameBufferFontDBFree(fontdb);

  return 0;
}
