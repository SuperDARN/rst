/* iq.c
   ======== 
   Author: R.J.Barnes
*/

/*
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
#include "iq.h"

struct IQ *IQMake() {
  struct IQ *ptr=NULL;
  ptr=malloc(sizeof(struct IQ));
  if (ptr==NULL) return NULL;
  memset(ptr,0,sizeof(struct IQ));
  ptr->tval=NULL;
  ptr->atten=NULL;
  ptr->noise=NULL;
  ptr->offset=NULL;
  ptr->size=NULL;
  ptr->badtr=NULL;
  return ptr;
}


void IQFree(struct IQ *ptr) {
  if (ptr==NULL) return;
  if (ptr->tval !=NULL) free(ptr->tval);
  if (ptr->atten !=NULL) free(ptr->atten);
  if (ptr->noise !=NULL) free(ptr->noise);
  if (ptr->offset !=NULL) free(ptr->offset);
  if (ptr->size !=NULL) free(ptr->size);
  if (ptr->badtr !=NULL) free(ptr->badtr);
  free(ptr);
  return;
}

int IQSetTime(struct IQ *ptr,int nave,struct timespec *tval) {
  void *tmp=NULL;

  if (ptr==NULL) return -1;

  if ((nave==0) || (tval==NULL)) {
    if (ptr->tval !=NULL) free(ptr->tval);
    ptr->tval=NULL;
    return 0;
  }

  if (ptr->tval==NULL) tmp=malloc(sizeof(struct timespec)*nave);
  else tmp=realloc(ptr->tval,sizeof(struct timespec)*nave);

  if (tmp==NULL) return -1;
  memcpy(tmp,tval,sizeof(struct timespec)*nave);
  ptr->tval=tmp;
  
  return 0;
}



int IQSetAtten(struct IQ *ptr,int nave,int *atten) {
  void *tmp=NULL;

  if (ptr==NULL) return -1;
  if ((nave==0) || (atten==NULL)) {
    if (ptr->atten !=NULL) free(ptr->atten);
    ptr->atten=NULL;
    return 0;
  }
  if (ptr->atten==NULL) tmp=malloc(sizeof(int)*nave);
  else tmp=realloc(ptr->atten,sizeof(int)*nave);

  if (tmp==NULL) return -1;
  memcpy(tmp,atten,sizeof(int)*nave);
  ptr->atten=tmp;
  return 0;
}


int IQSetNoise(struct IQ *ptr,int nave,float *noise) {
  void *tmp=NULL;

  if (ptr==NULL) return -1;
  if ((nave==0) || (noise==NULL)) {
    if (ptr->noise !=NULL) free(ptr->noise);
    ptr->noise=NULL;
    return 0;
  }
  if (ptr->noise==NULL) tmp=malloc(sizeof(float)*nave);
  else tmp=realloc(ptr->noise,sizeof(float)*nave);

  if (tmp==NULL) return -1;
  memcpy(tmp,noise,sizeof(float)*nave);
  ptr->noise=tmp;
  return 0;
}

int IQSetOffset(struct IQ *ptr,int nave,int *offset) {
  void *tmp=NULL;

  if (ptr==NULL) return -1;
  if ((nave==0) || (offset==NULL)) {
    if (ptr->offset !=NULL) free(ptr->offset);
    ptr->offset=NULL;
    return 0;
  }
  if (ptr->offset==NULL) tmp=malloc(sizeof(int)*nave);
  else tmp=realloc(ptr->offset,sizeof(int)*nave);

  if (tmp==NULL) return -1;
  memcpy(tmp,offset,sizeof(int)*nave);
  ptr->offset=tmp;
  return 0;
}

int IQSetSize(struct IQ *ptr,int nave,int *size) {
  void *tmp=NULL;

  if (ptr==NULL) return -1;
  if ((nave==0) || (size==NULL)) {
    if (ptr->size !=NULL) free(ptr->size);
    ptr->size=NULL;
    return 0;
  }
  if (ptr->size==NULL) tmp=malloc(sizeof(int)*nave);
  else tmp=realloc(ptr->size,sizeof(int)*nave);

  if (tmp==NULL) return -1;
  memcpy(tmp,size,sizeof(int)*nave);
  ptr->size=tmp;
  return 0;
}


int IQSetBadTR(struct IQ *ptr,int nave,int *badtr) {
  void *tmp=NULL;

  if (ptr==NULL) return -1;
  if ((nave==0) || (badtr==NULL)) {
    if (ptr->badtr !=NULL) free(ptr->badtr);
    ptr->badtr=NULL;
    return 0;
  }
  if (ptr->badtr==NULL) tmp=malloc(sizeof(int)*nave);
  else tmp=realloc(ptr->badtr,sizeof(int)*nave);

  if (tmp==NULL) return -1;
  memcpy(tmp,badtr,sizeof(int)*nave);
  ptr->badtr=tmp;
  return 0;
}

void *IQFlatten(struct IQ *ptr,int nave,size_t *size) {
  size_t s;
  struct IQ *r;
  void *buf=NULL;
  size_t p=0;

  if (size==NULL) return NULL;

  s=sizeof(struct IQ);
  if (ptr->tval !=NULL) s+=nave*sizeof(struct timespec);
  if (ptr->atten !=NULL) s+=nave*sizeof(int);
  if (ptr->noise !=NULL) s+=nave*sizeof(float);
  if (ptr->offset !=NULL) s+=nave*sizeof(int);
  if (ptr->size !=NULL) s+=nave*sizeof(int);
  if (ptr->badtr !=NULL) s+=nave*sizeof(int);

  buf=malloc(s);
  if (buf==NULL) return NULL;
  *size=s; 

  r=(struct IQ *) buf;

  memcpy(buf,ptr,sizeof(struct IQ));
  p=sizeof(struct IQ);

  if (ptr->tval !=NULL) {
    memcpy(buf+p,ptr->tval,nave*sizeof(struct timespec));
    r->tval=(void *) p;
    p+=nave*sizeof(struct timespec);
  }

  if (ptr->atten !=NULL) {
    memcpy(buf+p,ptr->atten,nave*sizeof(int));
    r->atten=(void *) p;
    p+=nave*sizeof(int);
  }

  if (ptr->noise !=NULL) {
    memcpy(buf+p,ptr->noise,nave*sizeof(float));
    r->noise=(void *) p;
    p+=nave*sizeof(float);
  }

  if (ptr->offset !=NULL) {
    memcpy(buf+p,ptr->offset,nave*sizeof(int));
    r->offset=(void *) p;
    p+=nave*sizeof(int);
  }

  if (ptr->size !=NULL) {
    memcpy(buf+p,ptr->size,nave*sizeof(int));
    r->size=(void *) p;
    p+=nave*sizeof(int);
  }

  if (ptr->badtr !=NULL) {
    memcpy(buf+p,ptr->badtr,nave*sizeof(int));
    r->badtr=(void *) p;
    p+=nave*sizeof(int);
  }



  return buf;
}

int IQExpand(struct IQ *ptr,int nave,void *buffer) {
  void *p;
  if (ptr==NULL) return -1;
  if (buffer==NULL) return -1;

  if (ptr->tval !=NULL) free(ptr->tval);
  if (ptr->atten !=NULL) free(ptr->atten);
  if (ptr->noise !=NULL) free(ptr->noise);
  if (ptr->offset !=NULL) free(ptr->offset);
  if (ptr->size !=NULL) free(ptr->size);
  if (ptr->badtr !=NULL) free(ptr->badtr);
  memcpy(ptr,buffer,sizeof(struct IQ));
  if (ptr->tval !=NULL) {
    p=buffer+(size_t) ptr->tval;
    ptr->tval=malloc(nave*sizeof(struct timespec));
    memcpy(ptr->tval,p,nave*sizeof(struct timespec));
  }
  if (ptr->atten !=NULL) {
    p=buffer+(size_t) ptr->atten;
    ptr->atten=malloc(nave*sizeof(int));
    memcpy(ptr->atten,p,nave*sizeof(int));
  }
  if (ptr->noise !=NULL) {
    p=buffer+(size_t) ptr->noise;
    ptr->noise=malloc(nave*sizeof(float));
    memcpy(ptr->noise,p,nave*sizeof(float));
  }
  if (ptr->offset !=NULL) {
    p=buffer+(size_t) ptr->offset;
    ptr->offset=malloc(nave*sizeof(int));
    memcpy(ptr->offset,p,nave*sizeof(int));
  }
  if (ptr->size !=NULL) {
    p=buffer+(size_t) ptr->size;
    ptr->size=malloc(nave*sizeof(int));
    memcpy(ptr->size,p,nave*sizeof(int));
  }
  if (ptr->badtr !=NULL) {
    p=buffer+(size_t) ptr->badtr;
    ptr->badtr=malloc(nave*sizeof(int));
    memcpy(ptr->badtr,p,nave*sizeof(int));
  }
  return 0;
}







