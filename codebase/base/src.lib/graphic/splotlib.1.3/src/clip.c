/* clip.c
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




int SplotClip(struct Splot *ptr,
              int num,float *px,float *py,int *t) {

  int s=0;
  if (ptr==NULL) return -1;
  if (ptr->ps.ptr !=NULL) {
    struct PostScript *ps;
    ps=ptr->ps.ptr;
    if (ptr->ps.clip !=NULL) PostScriptFreeClip(ptr->ps.clip);
    ptr->ps.clip=NULL;
    if (num==0) return 0;
    ptr->ps.clip=PostScriptMakeClip(ps->x,ps->y,ps->wdt,ps->hgt,num,px,py,t);
    if (ptr->ps.clip==NULL) s=-1;
  }
  if (ptr->fb.ptr !=NULL) {
    int *ix=NULL,*iy=NULL;
    int i;
    struct FrameBuffer *img;
    img=*(ptr->fb.ptr);
    if (img==NULL) return -1;

    if (ptr->fb.clip !=NULL) FrameBufferFreeClip(ptr->fb.clip);
    ptr->fb.clip=NULL;
    if (num==0) return 0;

    ix=malloc(sizeof(int)*num);
    if (ix==NULL) return -1;
    iy=malloc(sizeof(int)*num);
    if (iy==NULL) {
      free(ix);
      return -1;
    }
    for (i=0;i<num;i++) {
      ix[i]=px[i];
      iy[i]=py[i];
    }

    ptr->fb.clip=FrameBufferMakeClip(img->wdt,img->hgt,num,ix,iy,t);
    if (ptr->fb.clip==NULL) s=-1;
    free(ix);
    free(iy);
  }



  return s;
} 






