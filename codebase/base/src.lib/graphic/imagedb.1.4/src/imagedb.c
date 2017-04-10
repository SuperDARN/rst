/* imagedb.c
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
};


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







