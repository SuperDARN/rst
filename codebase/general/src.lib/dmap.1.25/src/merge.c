/* merge.c
   ======= */


/*
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
 
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"


struct DataMapArray *DataMapMergeArray(char *name,
                                struct DataMapArray *x,struct DataMapArray *y) {
  struct DataMapArray *a=NULL;
  int type;
  int32 dim;
  int32 *rngbuf=NULL;
  int32 *inx=NULL;
  void *tmp=NULL;
  void *dp,*sp;
  int wd;
  int n=0,c,cc,t;

  type=x->type;
  dim=x->dim;
  rngbuf=malloc(dim*sizeof(int32));
  if (rngbuf==NULL) return NULL;

  inx=malloc(dim*sizeof(int32));
  if (inx==NULL) {
    if (rngbuf !=NULL) free(rngbuf);
    return NULL;
  }

  rngbuf[0]=x->rng[0]+y->rng[0];
  n=rngbuf[0];
  for (c=1;c<x->dim;c++) { 
    rngbuf[c]=x->rng[c];
    if (y->rng[c]>rngbuf[c]) rngbuf[c]=y->rng[c];
    n=n*rngbuf[c];
  }

  switch (type) {
  case DATACHAR:
    tmp=malloc(sizeof(char)*n);
    if (tmp==NULL) break;
    wd=sizeof(char);
    memset(tmp,0,sizeof(char)*n);
    break;
  case DATASHORT:
    tmp=malloc(sizeof(int16)*n);
    if (tmp==NULL) break;
    wd=sizeof(int16);
    memset(tmp,0,sizeof(int16)*n);
    break;
  case DATAINT:
    tmp=malloc(sizeof(int32)*n);
    if (tmp==NULL) break;
    wd=sizeof(int32);
    memset(tmp,0,sizeof(int32)*n);
    break;
  case DATALONG:
    tmp=malloc(sizeof(int64)*n);
    if (tmp==NULL) break;
    wd=sizeof(int64);
    memset(tmp,0,sizeof(int64)*n);
    break;
  case DATAUCHAR:
    tmp=malloc(sizeof(unsigned char)*n);
    if (tmp==NULL) break;
    wd=sizeof(unsigned char);
    memset(tmp,0,sizeof(unsigned char)*n);
    break;
  case DATAUSHORT:
    tmp=malloc(sizeof(uint16)*n);
    if (tmp==NULL) break;
    wd=sizeof(uint16);
    memset(tmp,0,sizeof(uint16)*n);
    break;
  case DATAUINT:
    tmp=malloc(sizeof(uint32)*n);
    if (tmp==NULL) break;
    wd=sizeof(uint32);
    memset(tmp,0,sizeof(uint32)*n);
    break;
  case DATAULONG:
    tmp=malloc(sizeof(uint64)*n);
    if (tmp==NULL) break;
    wd=sizeof(uint64);
    memset(tmp,0,sizeof(uint64)*n);
    break;
  case DATAFLOAT:
    tmp=malloc(sizeof(float)*n);
    if (tmp==NULL) break;
    wd=sizeof(float);
    memset(tmp,0,sizeof(float)*n);
    break;
  case DATADOUBLE:
    tmp=malloc(sizeof(double)*n);
    if (tmp==NULL) break;
    wd=sizeof(double);
    memset(tmp,0,sizeof(double)*n);
    break;
  case DATASTRING:
    tmp=malloc(sizeof(char *)*n);
    if (tmp==NULL) break;
    wd=sizeof(char *);
    memset(tmp,0,sizeof(char *)*n);
    break;
  default:
    tmp=malloc(sizeof(struct DataMap *)*n);
    if (tmp==NULL) break;
    wd=sizeof(struct DataMap *);
    memset(tmp,0,sizeof(struct DataMap *)*n);
    break;
  }
  if (tmp==NULL) {
    if (rngbuf !=NULL) free(rngbuf);
    if (inx !=NULL) free(inx);
    return NULL;
  }
 
  a=DataMapMakeArray(name,3,type,dim,rngbuf,tmp);

  n=1;
  for (c=0;c<x->dim;c++)  n=n*x->rng[c];
  for (c=0;c<n;c++) { 
    sp=(int*)x->data.vptr+wd*c;   
    t=c;
    for (cc=0;cc<x->dim;cc++) {
      inx[cc]=t % x->rng[cc];
      t=t/x->rng[cc];
    }
    t=0;
    for (cc=x->dim;cc>0;cc--) {
      if (cc !=x->dim) t=t*rngbuf[cc];
      t+=inx[cc-1];
    }
    dp=(int*)tmp+wd*t;
    memcpy(dp,sp,wd);
  }     

  n=1;
  for (c=0;c<y->dim;c++)  n=n*y->rng[c];
  for (c=0;c<n;c++) { 
    sp=(int*)y->data.vptr+wd*c;   
    t=c;
    for (cc=0;cc<y->dim;cc++) {
      inx[cc]=t % y->rng[cc];
      t=t/y->rng[cc];
    }
    inx[0]+=x->rng[0];
    t=0;
    for (cc=y->dim;cc>0;cc--) {
      if (cc !=x->dim) t=t*rngbuf[cc];
      t+=inx[cc-1];
    }
    dp=(int*)tmp+wd*t;
    memcpy(dp,sp,wd);
  }     
  free(inx);  
  return a;
}

struct DataMap *DataMapMerge(int num,struct DataMap **in) {
  int n,x,c;
  struct DataMap *out;
  struct DataMapScalar *s;
  struct DataMapArray *a;
  struct DataMapArray *t;

  out=DataMapMake();

  for (n=0;n<num;n++) { 
    
    for (x=0;x<in[n]->snum;x++) {
      s=in[n]->scl[x];   
      for (c=0;c<out->snum;c++) {
        if (strcmp((out->scl[c])->name,s->name) !=0) continue;
        if ((out->scl[c])->type != s->type) continue;
        break;
      }
      if (c !=out->snum) { /* duplicate */
        (out->scl[c])->data.vptr=s->data.vptr;
        continue;
      }
      DataMapAddScalar(out,s->name,s->type,s->data.vptr);
    }
    for (x=0;x<in[n]->anum;x++) {
      a=in[n]->arr[x];
      for (c=0;c<out->anum;c++) {
        if (strcmp((out->arr[c])->name,a->name) !=0) continue;
        if ((out->arr[c])->type != a->type) continue;
	if ((out->arr[c])->dim != a->dim) continue;
        break;
      }
      
      if (c !=out->anum) {  /* duplicate array */
        t=DataMapMergeArray(a->name,out->arr[c],a);
        DataMapFreeArray(out->arr[c]);
        out->arr[c]=t;
        continue; 
      }
      
      DataMapAddArray(out,a->name,a->type,a->dim,a->rng,a->data.vptr);
    }
  }
  return out;
}




  
