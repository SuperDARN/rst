/* cfit.c
   ======== 
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <zlib.h>
#include "rtypes.h"
#include "cfitdata.h"

struct CFitdata *CFitMake() {
  struct CFitdata *ptr=NULL;
  ptr=malloc(sizeof(struct CFitdata));
  if (ptr==NULL) return NULL;
  memset(ptr,0,sizeof(struct CFitdata));
  ptr->rng=NULL;
  ptr->data=NULL;
  return ptr;
}

void CFitFree(struct CFitdata *ptr) {

  if (ptr==NULL) return;
  if (ptr->rng !=NULL) free(ptr->rng);
  if (ptr->data !=NULL) free(ptr->data);
  free(ptr);
  return;
}

int CFitSetRng(struct CFitdata *ptr,int num) {
  void *tmp=NULL;

  if (ptr==NULL) return -1;
  if (num==0) {
    if (ptr->rng !=NULL) free(ptr->rng);
    if (ptr->data !=NULL) free(ptr->data); 
    ptr->rng=NULL;
    ptr->data=NULL;
    ptr->num=0;
    return 0;
  }

  if (ptr->rng==NULL) tmp=malloc(sizeof(int16)*num);
  else tmp=realloc(ptr->rng,sizeof(int16)*num);
  if (tmp==NULL) return -1;
  ptr->rng=tmp;

  if (ptr->data==NULL) tmp=malloc(sizeof(struct CFitCell)*num);
  else tmp=realloc(ptr->data,sizeof(struct CFitCell)*num);
  if (tmp==NULL) {
    free(ptr->rng);
    ptr->rng=NULL;
    return -1;
  }
  ptr->data=tmp;
  ptr->num=num;
  return 0;
}


