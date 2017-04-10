/* shape.c
   ======= 
   Author:R.J.Barnes
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

int SplotRectangle(struct Splot *ptr,
	           struct PlotMatrix *matrix,
                   float x,float y,float w,float h,
                   int fill,unsigned int color,unsigned  char mask,
                   float width,
		   struct PlotDash *dash) {

  if (ptr==NULL) return -1;
  if (ptr->ps.ptr !=NULL) {
    struct PostScriptMatrix psmatrix;
    struct PostScriptMatrix *mptr=NULL;
    unsigned int pscolor;

    if (matrix !=NULL) {
      psmatrix.a=matrix->a;
      psmatrix.b=matrix->b;
      psmatrix.c=matrix->c;
      psmatrix.d=matrix->d;
      mptr=&psmatrix;
    }
    SplotPostScriptDash(ptr,dash);
    pscolor=SplotPostScriptColor(color);
 
    return PostScriptRectangle(ptr->ps.ptr,mptr,x,y,w,h,fill,pscolor,
                               width,ptr->ps.dash,ptr->ps.clip);

  }


  if (ptr->fb.ptr !=NULL) {
    struct FrameBuffer *img;
    struct FrameBufferMatrix fbmatrix;
    struct FrameBufferMatrix *mptr=NULL;

    img=*(ptr->fb.ptr);
    if (img==NULL) return -1;


    if (matrix !=NULL) {
      fbmatrix.a=matrix->a;
      fbmatrix.b=matrix->b;
      fbmatrix.c=matrix->c;
      fbmatrix.d=matrix->d;
      mptr=&fbmatrix;
    }

    SplotFrameBufferDash(ptr,dash);
    return FrameBufferRectangle(img,mptr,x,y,w,h,fill,color,mask,
                          width,ptr->fb.dash,ptr->fb.clip);
  }

  return 0;
} 

int SplotEllipse(struct Splot *ptr,
	           struct PlotMatrix *matrix,
                   float x,float y,float w,float h,
                   int fill,unsigned int color,unsigned  char mask,
                   float width,
		   struct PlotDash *dash) {
 

  if (ptr==NULL) return -1;
  if (ptr->ps.ptr !=NULL) {
    struct PostScriptMatrix psmatrix;
    struct PostScriptMatrix *mptr=NULL;
    unsigned int pscolor;

    if (matrix !=NULL) {
      psmatrix.a=matrix->a;
      psmatrix.b=matrix->b;
      psmatrix.c=matrix->c;
      psmatrix.d=matrix->d;
      mptr=&psmatrix;
    }
    pscolor=SplotPostScriptColor(color);

    return PostScriptEllipse(ptr->ps.ptr,mptr,x,y,w,h,fill,pscolor,
                             width,ptr->ps.dash,ptr->ps.clip);

  }


  if (ptr->fb.ptr !=NULL) {
    struct FrameBuffer *img;
    struct FrameBufferMatrix fbmatrix;
    struct FrameBufferMatrix *mptr=NULL;

    img=*(ptr->fb.ptr);
    if (img==NULL) return -1;


    if (matrix !=NULL) {
      fbmatrix.a=matrix->a;
      fbmatrix.b=matrix->b;
      fbmatrix.c=matrix->c;
      fbmatrix.d=matrix->d;
      mptr=&fbmatrix;
    }

    SplotFrameBufferDash(ptr,dash);
    return FrameBufferEllipse(img,mptr,x,y,w,h,fill,color,mask,
                          width,ptr->fb.dash,ptr->fb.clip);
  }



  return 0;
} 



