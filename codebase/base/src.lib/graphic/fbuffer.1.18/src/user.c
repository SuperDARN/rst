/* user.c
   ====== 
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include "rfbuffer.h"



int FrameBufferSetUser(struct FrameBuffer *ptr,
                       int (*pixel)(int wdt,int hgt,unsigned char *img,
		       unsigned char *msk,
                       int x,int y,int depth,int off,int sze,
                       unsigned char r,unsigned char g,unsigned char b,
		       unsigned char a,void *data),void *data) {
  if (ptr==NULL) return -1;
  ptr->user.pixel=pixel;
  ptr->user.data=data;
  return 0;
}
