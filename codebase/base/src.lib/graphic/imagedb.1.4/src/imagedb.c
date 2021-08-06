/* imagedb.c
   ========= 
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
#include "imagedb.h"



void FrameBufferDBFree(struct FrameBufferDB *ptr) {
  int n;
  if (ptr==NULL) return;
  if (ptr->img !=NULL) {
    for (n=0;n<ptr->num;n++) 
      if (ptr->img[n] !=NULL) FrameBufferFree(ptr->img[n]);
    free(ptr->img);
  }
  free(ptr);
}

struct FrameBufferDB *FrameBufferDBMake() {
  struct FrameBufferDB *ptr;
  ptr=malloc(sizeof(struct FrameBufferDB));
  if (ptr==NULL) return NULL;
  ptr->num=0;
  ptr->img=NULL;
  return ptr;
}

int FrameBufferDBAdd(struct FrameBufferDB *ptr,struct FrameBuffer *img) {
  struct FrameBuffer **tmp;
  
  if (ptr==NULL) return -1;
  if (img==NULL) return -1;

  if (ptr->img==NULL) ptr->img=malloc(sizeof(struct FrameBuffer *));
  else {
    tmp=realloc(ptr->img,sizeof(struct FrameBuffer *)*(ptr->num+1));
    if (tmp==NULL) return -1;
    ptr->img=tmp;
  }

  if (ptr->img==NULL) return -1;
  ptr->img[ptr->num]=img;
  ptr->num++;
  return 0;
}

struct FrameBuffer *FrameBufferDBFind(struct FrameBufferDB *ptr,char *name) {
  int n;

  if (ptr==NULL) return NULL;

  if (ptr->img==NULL) return NULL;
  
  for (n=0;n<ptr->num;n++) {
    if ((ptr->img[n])->name==NULL) continue;
    if (strcmp((ptr->img[n])->name,name)==0) return ptr->img[n];
  }
  return NULL;
}







