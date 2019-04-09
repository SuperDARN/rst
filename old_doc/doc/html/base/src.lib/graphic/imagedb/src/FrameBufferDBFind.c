/* FrameBufferDBFind.c
   ===================
   Author: R.J.Barnes
*/


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
    FrameBufferEllipse(img,NULL,5,5,2+n,2+n,1,0xffffffff,0x0f,0,NULL,NULL);
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
