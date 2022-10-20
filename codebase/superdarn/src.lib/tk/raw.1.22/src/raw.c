/* raw.c
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
#include "rawdata.h"

struct RawData *RawMake() {
  int n;
  struct RawData *ptr=NULL;
  ptr=malloc(sizeof(struct RawData));
  if (ptr==NULL) return NULL;
  memset(ptr,0,sizeof(struct RawData));
  ptr->pwr0=NULL;
  for (n=0;n<2;n++) {
    ptr->acfd[n]=NULL;
    ptr->xcfd[n]=NULL;
  }
  return ptr;
}

void RawFree(struct RawData *ptr) {
  int n;
  if (ptr==NULL) return;
  if (ptr->pwr0 !=NULL) free(ptr->pwr0);
  for (n=0;n<2;n++) {
    if (ptr->acfd[n] !=NULL) free(ptr->acfd[n]);
    if (ptr->xcfd[n] !=NULL) free(ptr->xcfd[n]);
  }
  free(ptr);
  return;
}

int RawSetPwr(struct RawData *ptr,int nrang,float *pwr0,int snum,int *slist) {
  void *tmp=NULL;
  int s;
  if (ptr==NULL) return -1;
  if ((nrang==0) || (pwr0==NULL)) {
    if (ptr->pwr0 !=NULL) free(ptr->pwr0);
    ptr->pwr0=NULL;
    return 0;
  }
  if (ptr->pwr0==NULL) tmp=malloc(sizeof(float)*nrang);
  else tmp=realloc(ptr->pwr0,sizeof(float)*nrang);

  if (tmp==NULL) return -1;
  ptr->pwr0=tmp;
  if (slist==NULL) memcpy(tmp,pwr0,sizeof(float)*nrang);
  else {
    memset(tmp,0,sizeof(float)*nrang);
    for (s=0;s<snum;s++) ptr->pwr0[slist[s]]=pwr0[s];
  }
  return 0;
}


int RawSetACF(struct RawData *ptr,int nrang,int mplgs,float *acfd,int snum,int *slist) {
  float *tmp=NULL;
  int n,x,y,s;
  if (ptr==NULL) return -1;
  if ((nrang*mplgs==0) || (acfd==NULL)) {
    for (n=0;n<2;n++) {
      if (ptr->acfd[n] !=NULL) free(ptr->acfd[n]);
      ptr->acfd[n]=NULL;
    }
    return 0;
  }

  for (n=0;n<2;n++) {
    if (ptr->acfd[n]==NULL) tmp=malloc(sizeof(float)*nrang*mplgs);
    else tmp=realloc(ptr->acfd[n],sizeof(float)*nrang*mplgs);
    if (tmp==NULL) return -1;
    ptr->acfd[n]=tmp;
    if (slist==NULL) {
      for (x=0;x<nrang;x++) {
        for (y=0;y<mplgs;y++) { 
          tmp[x*mplgs+y]=acfd[2*(x*mplgs+y)+n];
        }
      }
    } else {
      memset(tmp,0,sizeof(float)*nrang*mplgs);
      for (s=0;s<snum;s++) {
        x=slist[s];
        for (y=0;y<mplgs;y++) {
          tmp[x*mplgs+y]=acfd[2*(s*mplgs+y)+n];
        }
      }
    }
  }
  return 0;
}


int RawSetXCF(struct RawData *ptr,int nrang,int mplgs,float *xcfd,int snum,int *slist) {
  float *tmp=NULL;
  int n,x,y,s;
  if (ptr==NULL) return -1;
  if ((nrang*mplgs==0) || (xcfd==NULL)) {
    for (n=0;n<2;n++) {
      if (ptr->xcfd[n] !=NULL) free(ptr->xcfd[n]);
      ptr->xcfd[n]=NULL;
    }
    return 0;
  }

  for (n=0;n<2;n++) {
    if (ptr->xcfd[n]==NULL) tmp=malloc(sizeof(float)*nrang*mplgs);
    else tmp=realloc(ptr->xcfd[n],sizeof(float)*nrang*mplgs);
    if (tmp==NULL) return -1;
    ptr->xcfd[n]=tmp;
    if (slist==NULL) {
      for (x=0;x<nrang;x++) {
        for (y=0;y<mplgs;y++) {
          tmp[x*mplgs+y]=xcfd[2*(x*mplgs+y)+n];
        }
      }
    } else {
      memset(tmp,0,sizeof(float)*nrang*mplgs);
      for (s=0;s<snum;s++) {
        x=slist[s];
        for (y=0;y<mplgs;y++) {
          tmp[x*mplgs+y]=xcfd[2*(s*mplgs+y)+n];
        }
      }
    }
  }
  return 0;
}


void *RawFlatten(struct RawData *ptr,int nrang,int mplgs,size_t *size) {
  size_t s;
  int n;
  struct RawData *r;
  void *buf=NULL;
  size_t p=0;

  if (size==NULL) return NULL;

  s=sizeof(struct RawData);
  if (ptr->pwr0 !=NULL) s+=nrang*sizeof(float);
  for (n=0;n<2;n++) 
    if (ptr->acfd[n] !=NULL) s+=(nrang*mplgs)*sizeof(float);
  for (n=0;n<2;n++) 
    if (ptr->xcfd[n] !=NULL) s+=(nrang*mplgs)*sizeof(float);
  
  buf=malloc(s);
  if (buf==NULL) return NULL;
  *size=s; 

  r=(struct RawData *) buf;

  memcpy(buf,ptr,sizeof(struct RawData));
  p=sizeof(struct RawData);

  if (ptr->pwr0 !=NULL) {
    memcpy(buf+p,ptr->pwr0,nrang*sizeof(float));
    r->pwr0=(void *) p;
    p+=nrang*sizeof(float);
  }

  for (n=0;n<2;n++) {
    if (ptr->acfd[n]==NULL) continue; 
    memcpy(buf+p,ptr->acfd[n],(nrang*mplgs)*sizeof(float));
    r->acfd[n]=(void *) p;
    p+=(nrang*mplgs)*sizeof(float); 
  }

  for (n=0;n<2;n++) {
    if (ptr->xcfd[n]==NULL) continue; 
    memcpy(buf+p,ptr->xcfd[n],(nrang*mplgs)*sizeof(float));
    r->xcfd[n]=(void *) p;
    p+=(nrang*mplgs)*sizeof(float); 
  }

  return buf;
}

int RawExpand(struct RawData *ptr,int nrang,int mplgs,void *buffer) {
  void *p;
  int n;
  if (ptr==NULL) return -1;
  if (buffer==NULL) return -1;

  if (ptr->pwr0 !=NULL) free(ptr->pwr0);
  for (n=0;n<2;n++) 
    if (ptr->acfd[n] !=NULL) free(ptr->acfd[n]);
  for (n=0;n<2;n++) 
    if (ptr->xcfd[n] !=NULL) free(ptr->xcfd[n]);
  memcpy(ptr,buffer,sizeof(struct RawData));

  if (ptr->pwr0 !=NULL) {
    p=buffer+(size_t) ptr->pwr0;
    ptr->pwr0=malloc(nrang*sizeof(float));
    memcpy(ptr->pwr0,p,nrang*sizeof(float));
  }

  for (n=0;n<2;n++) {
    if (ptr->acfd[n]==NULL) continue;
    p=buffer+(size_t) ptr->acfd[n];
    ptr->acfd[n]=malloc((nrang*mplgs)*sizeof(float));
    memcpy(ptr->acfd[n],p,(nrang*mplgs)*sizeof(float));
  }

  for (n=0;n<2;n++) {
    if (ptr->xcfd[n]==NULL) continue;
    p=buffer+(size_t) ptr->xcfd[n];
    ptr->xcfd[n]=malloc((nrang*mplgs)*sizeof(float));
    memcpy(ptr->xcfd[n],p,(nrang*mplgs)*sizeof(float));
  }
 
  return 0;
}

