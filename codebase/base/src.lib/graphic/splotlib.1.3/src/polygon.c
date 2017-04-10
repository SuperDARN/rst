/* polygon.c
   ========= 
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




int SplotPolygon(struct Splot *ptr,struct PlotMatrix *matrix,
                 float x,float y,
                 int num,float *px,float *py,int *t,int fill,
                 unsigned int color,unsigned char mask,float width,
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
    
    return PostScriptPolygon(ptr->ps.ptr,mptr,x,y,num,px,py,t,fill,pscolor,
                             width,ptr->ps.dash,ptr->ps.clip);

  }

  if (ptr->fb.ptr !=NULL) {
    
    int *ipx=NULL;
    int *ipy=NULL;
    int n;
    struct FrameBuffer *img;
    struct FrameBufferMatrix fbmatrix;
    struct FrameBufferMatrix *mptr=NULL;

    img=*(ptr->fb.ptr);
    if (img==NULL) return -1;

        
   ipx=malloc(sizeof(int)*num);
   if (ipx==NULL) return -1;
   ipy=malloc(sizeof(int)*num);
   if (ipy==NULL) {
     free(ipx);
     return -1;
   }
   for (n=0;n<num;n++) {
      ipx[n]=(int) px[n];
      ipy[n]=(int) py[n];

    }

    if (matrix !=NULL) {
      fbmatrix.a=matrix->a;
      fbmatrix.b=matrix->b;
      fbmatrix.c=matrix->c;
      fbmatrix.d=matrix->d;
      mptr=&fbmatrix;
    }

    SplotFrameBufferDash(ptr,dash);
    return FrameBufferPolygon(img,mptr,x,y,num,ipx,ipy,t,fill,color,
                              mask,width,ptr->fb.dash,ptr->fb.clip);


   
  }



  return 0;
} 

