/* buffer.c
   ======== 
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rxml.h"
#include "rxmldb.h"



struct XMLDBbuffer *XMLDBCopyBuffer(struct XMLDBbuffer *src) {

  struct XMLDBbuffer *dst;
  
  if (src==NULL) return NULL;

  dst=malloc(sizeof(struct XMLDBbuffer));
  if (dst==NULL) return NULL;

  dst->sze=src->sze;
  dst->stp=src->stp;
  dst->max=src->max;

  dst->buf=malloc(src->max);
  if (dst->buf==NULL) {
     free(dst);
     return NULL;
  }
   
  memcpy(dst->buf,src->buf,src->max);
  
  return dst;
}



struct XMLDBbuffer *XMLDBMakeBuffer(int stp) {
  struct XMLDBbuffer *ptr;

  ptr=malloc(sizeof(struct XMLDBbuffer));
  if (ptr==NULL) return NULL;
  ptr->sze=0;
  ptr->stp=stp;
  ptr->max=stp;
  ptr->buf=malloc(stp);
  if (ptr->buf==NULL) {
    free(ptr);
    return NULL;
  }
  return ptr;
}

void XMLDBFreeBuffer(struct XMLDBbuffer *ptr) {

  if (ptr==NULL) return;
  if (ptr->buf !=NULL) free(ptr->buf);
  free(ptr);
}




int XMLDBAddBuffer(struct XMLDBbuffer *ptr,char *str,int sze) { 
  int stp=0;
  int xnt=0;

  if (ptr==NULL) return -1;

  stp=ptr->stp;
  if (sze>stp) stp=sze+stp; /* always make sure we have spare capacity */

  xnt=ptr->sze+sze;

  if (xnt>ptr->max) {
    char *tmp;
    ptr->max+=stp;
    tmp=realloc(ptr->buf,ptr->max);
    if (tmp==NULL) return -1;
    ptr->buf=tmp;
  }
 
  if (ptr->buf==NULL) return -1;
  
  memcpy(ptr->buf+ptr->sze,str,sze);
  ptr->sze+=sze;
  return 0;
}



  



