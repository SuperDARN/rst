/* fit.c
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
#include "dmap.h"
#include "fitdata.h"

struct FitData *FitMake() {

  struct FitData *ptr=NULL;
  ptr=malloc(sizeof(struct FitData));
  if (ptr==NULL) return NULL;
  memset(ptr,0,sizeof(struct FitData));
  ptr->rng=NULL;
  ptr->xrng=NULL;
  ptr->elv=NULL;
  return ptr;
}

void FitFree(struct FitData *ptr) {

  if (ptr==NULL) return;
  if (ptr->rng !=NULL) free(ptr->rng);
  if (ptr->xrng !=NULL) free(ptr->xrng);
  if (ptr->elv !=NULL) free(ptr->elv);
  free(ptr);
  return;
}

int FitSetRng(struct FitData *ptr,int nrang) {
  void *tmp=NULL;

  if (ptr==NULL) return -1;
  if (nrang==0) {
    if (ptr->rng !=NULL) free(ptr->rng);
    ptr->rng=NULL;
    return 0;
  }
  if (ptr->rng==NULL) tmp=malloc(sizeof(struct FitRange)*nrang);
  else tmp=realloc(ptr->rng,sizeof(struct FitRange)*nrang);

  if (tmp==NULL) return -1;
  memset(tmp,0,sizeof(struct FitRange)*nrang);
  ptr->rng=tmp;
  return 0;
}


int FitSetXrng(struct FitData *ptr,int nrang) {
  void *tmp=NULL;

  if (ptr==NULL) return -1;
  if (nrang==0) {
    if (ptr->xrng !=NULL) free(ptr->xrng);
    ptr->xrng=NULL;
    return 0;
  }
  if (ptr->xrng==NULL) tmp=malloc(sizeof(struct FitRange)*nrang);
  else tmp=realloc(ptr->xrng,sizeof(struct FitRange)*nrang);

  if (tmp==NULL) return -1;
  memset(tmp,0,sizeof(struct FitRange)*nrang);
  ptr->xrng=tmp;
  return 0;
}


int FitSetElv(struct FitData *ptr,int nrang) {
  void *tmp=NULL;

  if (ptr==NULL) return -1;
  if (nrang==0) {
    if (ptr->elv !=NULL) free(ptr->elv);
    ptr->elv=NULL;
    return 0;
  }
  if (ptr->elv==NULL) tmp=malloc(sizeof(struct FitElv)*nrang);
  else tmp=realloc(ptr->elv,sizeof(struct FitElv)*nrang);

  if (tmp==NULL) return -1;
  memset(tmp,0,sizeof(struct FitElv)*nrang);
  ptr->elv=tmp;
  return 0;
}



void *FitFlatten(struct FitData *ptr,int nrang,size_t *size) {
  size_t s;
  struct FitData *r;
  void *buf=NULL;
  size_t p=0;

  if (size==NULL) return NULL;

  s=sizeof(struct FitData);
  if (ptr->rng !=NULL) s+=nrang*sizeof(struct FitRange);
  if (ptr->xrng !=NULL) s+=nrang*sizeof(struct FitRange);    
  if (ptr->elv !=NULL) s+=nrang*sizeof(struct FitElv);

  buf=malloc(s);
  if (buf==NULL) return NULL;
  *size=s; 

  r=(struct FitData *) buf;

  memcpy(buf,ptr,sizeof(struct FitData));
  p=sizeof(struct FitData);

  if (ptr->rng !=NULL) {
    memcpy(buf+p,ptr->rng,nrang*sizeof(struct FitRange));
    r->rng=(void *) p;
    p+=nrang*sizeof(struct FitRange);
  }

  if (ptr->xrng !=NULL) {
    memcpy(buf+p,ptr->xrng,nrang*sizeof(struct FitRange));
    r->xrng=(void *) p;
    p+=nrang*sizeof(struct FitRange);
  }

  if (ptr->elv !=NULL) {
    memcpy(buf+p,ptr->elv,nrang*sizeof(struct FitElv));
    r->elv=(void *) p;
    p+=nrang*sizeof(struct FitElv);
  }

  return buf;
}

int FitExpand(struct FitData *ptr,int nrang,void *buffer) {
  void *p;
  if (ptr==NULL) return -1;
  if (buffer==NULL) return -1;

  if (ptr->rng !=NULL) free(ptr->rng);
  if (ptr->xrng !=NULL) free(ptr->xrng);
  if (ptr->elv !=NULL) free(ptr->elv);

  memcpy(ptr,buffer,sizeof(struct FitData));

  if (ptr->rng !=NULL) {
    p=buffer+(size_t) ptr->rng;
    ptr->rng=malloc(nrang*sizeof(struct FitRange));
    memcpy(ptr->rng,p,nrang*sizeof(struct FitRange));
  }

  if (ptr->xrng !=NULL) {
    p=buffer+(size_t) ptr->xrng;
    ptr->xrng=malloc(nrang*sizeof(struct FitRange));
    memcpy(ptr->xrng,p,nrang*sizeof(struct FitRange));
  }
 
  if (ptr->elv !=NULL) {
    p=buffer+(size_t) ptr->elv;
    ptr->elv=malloc(nrang*sizeof(struct FitElv));
    memcpy(ptr->elv,p,nrang*sizeof(struct FitElv));
  }

  return 0;
}


