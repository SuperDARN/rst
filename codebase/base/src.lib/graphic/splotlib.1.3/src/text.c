/* text.c
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





int SplotText(struct Splot *ptr,
	      struct PlotMatrix *matrix,
	      char *fname,float fsize,
              float x,float y,int num,char *txt,
              unsigned int color,unsigned  char mask,
              int sflg) {

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

    return PostScriptText(ptr->ps.ptr,mptr,fname,fsize,x,y,num,txt,pscolor,
                 ptr->ps.clip);
  }

  if (ptr->fb.ptr !=NULL) {
    struct FrameBuffer *img;
    struct FrameBufferMatrix fbmatrix;
    struct FrameBufferMatrix *mptr=NULL;
    struct FrameBufferFont *font=NULL;


    img=*(ptr->fb.ptr);
    if (img==NULL) return -1;


    if (matrix !=NULL) {
      fbmatrix.a=matrix->a;
      fbmatrix.b=matrix->b;
      fbmatrix.c=matrix->c;
      fbmatrix.d=matrix->d;
      mptr=&fbmatrix;
    }

    font=FrameBufferFontDBFind(ptr->fb.fontdb,fname,(int) fsize);


    return FrameBufferText(img,mptr,font,x,y,num,txt,color,
                           mask,sflg,ptr->fb.clip);
  }




 return 0;

} 

