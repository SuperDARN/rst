/* shape.c
   ======= 
   Author:R.J.Barnes
*/

/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
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



