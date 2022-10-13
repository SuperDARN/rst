/* fit.c
   ======== 
   Author: R.J.Barnes
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
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
  ptr->algorithm=NULL;
  ptr->rng=NULL;
  ptr->xrng=NULL;
  ptr->elv=NULL;
  return ptr;
}

void FitFree(struct FitData *ptr) {

  if (ptr==NULL) return;
  if (ptr->algorithm !=NULL) free(ptr->algorithm);
  if (ptr->rng !=NULL) free(ptr->rng);
  if (ptr->xrng !=NULL) free(ptr->xrng);
  if (ptr->elv !=NULL) free(ptr->elv);
  free(ptr);
  return;
}


int FitSetAlgorithm(struct FitData *ptr,char *str) {
  char *tmp=NULL;
  if (ptr==NULL) return -1;

  if (str==NULL) {
    if (ptr->algorithm !=NULL) free(ptr->algorithm);
    ptr->algorithm=NULL;
    return 0;
  }

  if (ptr->algorithm==NULL) tmp=malloc(strlen(str+1));
  else tmp=realloc(ptr->algorithm,strlen(str)+1);

  if (tmp==NULL) return -1;
  strcpy(tmp,str);
  ptr->algorithm=tmp;
  return 0;
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
  if (ptr->algorithm !=NULL) s+=strlen(ptr->algorithm)+1;
  if (ptr->rng !=NULL) s+=nrang*sizeof(struct FitRange);
  if (ptr->xrng !=NULL) s+=nrang*sizeof(struct FitRange);    
  if (ptr->elv !=NULL) s+=nrang*sizeof(struct FitElv);

  buf=malloc(s);
  if (buf==NULL) return NULL;
  *size=s; 

  r=(struct FitData *) buf;

  memcpy(buf,ptr,sizeof(struct FitData));
  p=sizeof(struct FitData);

  if (ptr->algorithm !=NULL) {
    strcpy(buf+p,ptr->algorithm);
    r->algorithm=(void *) p;
    p+=strlen(ptr->algorithm)+1;
  }

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

  if (ptr->algorithm !=NULL) free(ptr->algorithm);
  if (ptr->rng !=NULL) free(ptr->rng);
  if (ptr->xrng !=NULL) free(ptr->xrng);
  if (ptr->elv !=NULL) free(ptr->elv);

  memcpy(ptr,buffer,sizeof(struct FitData));

  if (ptr->algorithm !=NULL) {
    p=buffer+(size_t) ptr->algorithm;
    ptr->algorithm=malloc(strlen(p)+1);
    strcpy(ptr->algorithm,p);
  }

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


