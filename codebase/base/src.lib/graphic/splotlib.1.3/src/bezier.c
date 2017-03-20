/* bezier.c
   ======== 
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

int SplotBezier(struct Splot *ptr,
                float x1,float y1,float x2,float y2,
                float x3,float y3,float x4,float y4,
                unsigned int color,unsigned char mask,float width,
                struct PlotDash *dash) {

  if (ptr==NULL) return -1;
  if (ptr->ps.ptr !=NULL) {
    unsigned int pscolor;      
    pscolor=SplotPostScriptColor(color);
    SplotPostScriptDash(ptr,dash);
    return PostScriptBezier(ptr->ps.ptr,x1,y1,x2,y2,x3,y3,x4,y4,
                     pscolor,width,ptr->ps.dash,ptr->ps.clip);
  }

  if (ptr->fb.ptr !=NULL) {
    struct FrameBuffer *img;
    img=*(ptr->fb.ptr);
    if (img==NULL) return -1;
    SplotFrameBufferDash(ptr,dash);
    return FrameBufferBezier(img,x1,y1,x2,y2,x3,y3,x4,y4,0.05,color,mask,
                          width,ptr->fb.dash,ptr->fb.clip);
  }


  return 0;
}

