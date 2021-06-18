/* make.c
   ====== 
   Author: R.J.Barnes
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



struct FrameBuffer *FrameBufferMake(char *name,
                       int wdt,int hgt,int depth) {
  int s=0;
  int d=1;
  struct FrameBuffer *ptr;

  if (wdt==0) return NULL;
  if (hgt==0) return NULL;

  ptr=malloc(sizeof(struct FrameBuffer));
  if (ptr==NULL) return NULL;
  ptr->name=NULL;
  ptr->img=NULL;
  ptr->msk=NULL;

  ptr->user.pixel=NULL;
  ptr->user.data=NULL;

  ptr->wdt=wdt;
  ptr->hgt=hgt;
  ptr->depth=depth;

  if (name !=NULL) {
    ptr->name=malloc(strlen(name)+1);
    if (ptr->name !=NULL) strcpy(ptr->name,name);
    else s=-1;
  }

  if (depth !=8) d=3;
  if (s==0) ptr->img=malloc(wdt*hgt*d);
  if (ptr->img==NULL) s=-1;
  if (s==0) ptr->msk=malloc(wdt*hgt);
  if (ptr->msk==NULL) s=-1;
  

  if (s !=0) {
    if (ptr->name !=NULL) free(ptr->name);
    if (ptr->img !=NULL) free(ptr->img);
    if (ptr->msk !=NULL) free(ptr->msk); 
    free(ptr);
    return NULL;
  }

  memset(ptr->msk,0,wdt*hgt);
  memset(ptr->img,0,wdt*hgt*d);
  
  return ptr;
 
}

void FrameBufferFree(struct FrameBuffer *ptr) {
  if (ptr==NULL) return;
  if (ptr->name !=NULL) free(ptr->name);
  if (ptr->img !=NULL) free(ptr->img);
  if (ptr->msk !=NULL) free(ptr->msk);
  free(ptr);
}

struct FrameBuffer *FrameBufferCopy(struct FrameBuffer *src) {

  int s=0;
  int d=1;
  struct FrameBuffer *dst;

  if (src==NULL) return NULL;
  if (src->wdt==0) return NULL;
  if (src->hgt==0) return NULL;
  if (src->img==NULL) return NULL;
  if (src->msk==NULL) return NULL;


  dst=malloc(sizeof(struct FrameBuffer));
  if (dst==NULL) return NULL;
  dst->name=NULL;
  dst->img=NULL;
  dst->msk=NULL;
  dst->user.pixel=NULL;
  dst->user.data=NULL;

  dst->wdt=src->wdt;
  dst->hgt=src->hgt;
  dst->depth=src->depth;

  if (src->name !=NULL) {
    dst->name=malloc(strlen(src->name)+1);
    if (dst->name !=NULL) strcpy(dst->name,src->name);
    else s=-1;
  }

  if (src->depth !=8) d=3;
  if (s==0) dst->img=malloc(src->wdt*src->hgt*d);
  if (dst->img==NULL) s=-1;
  if (s==0) dst->msk=malloc(src->wdt*src->hgt);
  if (dst->msk==NULL) s=-1;
  

  if (s !=0) {
    if (dst->name !=NULL) free(dst->name);
    if (dst->img !=NULL) free(dst->img);
    if (dst->msk !=NULL) free(dst->msk);
    free(dst);
    return NULL;
  }

  memcpy(dst->msk,src->msk,src->wdt*src->hgt);
  memcpy(dst->img,src->img,src->wdt*src->hgt*d);
  return dst;
}

int FrameBufferClear(struct FrameBuffer *ptr,unsigned int color,
                     unsigned char m) {
  unsigned char r,g,b,a;

  if (ptr==NULL) return -1;
  if (ptr->msk==NULL) return -1;
  if (ptr->img==NULL) return -1; 

  b=(color & 0xff);
  g=(color & 0xff00)>>8;
  r=(color & 0xff0000)>>16;
  a=(color & 0xff000000)>>24;
 
  if (m & 0x08) memset(ptr->msk,a,ptr->wdt*ptr->hgt);
  if (m & 0x04) memset(ptr->img,r,ptr->wdt*ptr->hgt);

  if (ptr->depth !=8) { 
    if (m & 0x02) memset(ptr->img+(ptr->wdt*ptr->hgt),g,ptr->wdt*ptr->hgt);
    if (m & 0x01) memset(ptr->img+2*(ptr->wdt*ptr->hgt),b,ptr->wdt*ptr->hgt);
  }
  return 0;
}

