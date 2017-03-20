/* image.c
   ======= 
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

int SplotImage(struct Splot *ptr,
	      struct PlotMatrix *matrix,
              struct FrameBuffer *img,
	      unsigned  char mask,
              float x,float y,int sflg) {
  

  if (ptr==NULL) return -1;
  if (ptr->ps.ptr !=NULL) {
    struct PostScriptMatrix psmatrix;
    struct PostScriptMatrix *mptr=NULL;

    if (matrix !=NULL) {
       psmatrix.a=matrix->a;
       psmatrix.b=matrix->b;
       psmatrix.c=matrix->c;
       psmatrix.d=matrix->d;
       mptr=&psmatrix;
     }
     return PostScriptImage(ptr->ps.ptr,mptr,img,mask,x,y,ptr->ps.clip);
  }

  if (ptr->fb.ptr !=NULL) { 
    struct FrameBuffer *iptr;
    struct FrameBufferMatrix fbmatrix;
    struct FrameBufferMatrix *mptr=NULL;

    iptr=*(ptr->fb.ptr);
    if (iptr==NULL) return -1;


    if (matrix !=NULL) {
      fbmatrix.a=matrix->a;
      fbmatrix.b=matrix->b;
      fbmatrix.c=matrix->c;
      fbmatrix.d=matrix->d;
      mptr=&fbmatrix;
    }
    return FrameBufferImage(iptr,mptr,img,mask,x,y,sflg,ptr->fb.clip);
  }



  return 0;
}

