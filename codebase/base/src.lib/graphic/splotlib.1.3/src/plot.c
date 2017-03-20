/* plot.c
   ====== 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rfbuffer.h"
#include "fontdb.h"
#include "rps.h"
#include "iplot.h"
#include "splot.h"



int SplotPlotStart(struct Splot *ptr,char *name,
                  float wdt,float hgt,int depth) {

 if (ptr==NULL) return -1;
 if (ptr->ps.ptr !=NULL) 
    PostScriptPlotStart(ptr->ps.ptr);

 if (ptr->fb.ptr !=NULL) {
   struct FrameBuffer *img=NULL;
   img=*(ptr->fb.ptr);
   if (img !=NULL) FrameBufferFree(img);
   img=FrameBufferMake(name,wdt,hgt,depth);
   if (img==NULL) return -1;
   *(ptr->fb.ptr)=img;
   FrameBufferClear(img,ptr->fb.bgcolor,0x0f);
  } 
  return 0;
}

int SplotPlotEnd(struct Splot *ptr) {
  if (ptr==NULL) return -1;
  if (ptr->ps.ptr !=NULL) PostScriptPlotEnd(ptr->ps.ptr);
  if (ptr->fb.ptr !=NULL) {
    struct FrameBuffer *img=NULL;
    img=*(ptr->fb.ptr);
    if (ptr->fb.trigger !=NULL) return (ptr->fb.trigger)(img,ptr->fb.data);
  }
  return 0;
}



