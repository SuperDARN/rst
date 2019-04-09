/* FrameBufferFontDBFind.c
   =======================
   Author: R.J.Barnes
*/


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
