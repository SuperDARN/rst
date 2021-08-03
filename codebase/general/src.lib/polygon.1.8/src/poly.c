/* poly.c
   ======
   Author: R.J.Barnes
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
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _DARWIN
#include <malloc.h>
#endif
#include <math.h>
#include "polygon.h"




struct PolygonData *PolygonMake(int sze,int (*bbox)(void *x,
                                                       void *y,int s)) {

  struct PolygonData *a;

  a=malloc(sizeof(struct PolygonData));
  a->polnum=0;
  a->pntnum=0;
  a->num=NULL;
  a->off=NULL;
  a->type=NULL;
  a->data=NULL;
  a->box=NULL;
  a->bbox=bbox;
  a->sze=sze;

  return a;
}


void PolygonFree(struct PolygonData *a) {
  if (a==NULL) return;
  if (a->num !=NULL) free(a->num);
  if (a->off !=NULL) free(a->off);
  if (a->type !=NULL) free(a->type);
  if (a->data !=NULL) free(a->data);
  if (a->box !=NULL) free(a->box);
  free(a);
}
 
int PolygonAddPolygon(struct PolygonData *a,int type) {
  void *tmp;
 
  if (a->num==NULL) tmp=malloc(sizeof(int));
  else tmp=realloc(a->num,(a->polnum+1)*sizeof(int));
  if (tmp==NULL) return -1;
  a->num=tmp;

  if (a->off==NULL) tmp=malloc(sizeof(int));
  else tmp=realloc(a->off,(a->polnum+1)*sizeof(int));
  if (tmp==NULL) return -1;
  a->off=tmp;

  if (a->type==NULL) tmp=malloc(sizeof(int));
  else tmp=realloc(a->type,(a->polnum+1)*sizeof(int));
  if (tmp==NULL) return -1;
  a->type=tmp;

  if (a->box==NULL) tmp=malloc(2*a->sze);
  else tmp=realloc(a->box,(a->polnum+1)*2*a->sze);
  if (tmp==NULL) return -1;
  a->box=tmp;

  if (a->bbox !=NULL) 
     a->bbox(NULL,(char *) a->box+(2*a->sze*(a->polnum)),a->sze);

  a->type[a->polnum]=type;
  a->num[a->polnum]=0;
  a->off[a->polnum]=a->pntnum;
  a->polnum++;
  return 0;
}

int PolygonAdd(struct PolygonData *a,void *pnt) {
  void *tmp;
  if (a->data==NULL) tmp=malloc(a->sze);
  else tmp=realloc(a->data,a->sze*(a->pntnum+1));
  if (tmp==NULL) return -1;
  a->data=tmp;

  memcpy((void *) ((char *) a->data+(a->sze*a->pntnum)),pnt,a->sze);
  
  if (a->bbox !=NULL) 
     a->bbox(pnt,(char *) a->box+(2*a->sze*(a->polnum-1)),a->sze);

  a->pntnum++;
  a->num[a->polnum-1]++;
  return 0; 
}

int PolygonReadType(struct PolygonData *ptr,int n) {
  if (ptr==NULL) return -1;
  if (n>=ptr->polnum) return -1;
  return ptr->type[n];
}

void *PolygonRead(struct PolygonData *ptr,int n,int o) {
  if (ptr==NULL) return NULL;
  if (n>=ptr->polnum) return NULL;
  if (o>=ptr->num[n]) return NULL;
  return  (void *) ((char *) ptr->data+(ptr->off[n]+o)*ptr->sze);
}
  
struct PolygonData *PolygonCopy(struct PolygonData *src) {
  int s=0;
  struct PolygonData *dst=NULL;
  dst=malloc(sizeof(struct PolygonData));
  if (dst==NULL) return NULL;
  dst->num=NULL;
  dst->off=NULL;
  dst->type=NULL;
  dst->box=NULL;
  dst->data=NULL;

  dst->num=malloc(sizeof(int)*(src->polnum+1));
  if (dst->num==NULL) s=-1;
  dst->off=malloc(sizeof(int)*(src->polnum+1));
  if (dst->off==NULL) s=-1;
  dst->type=malloc(sizeof(int)*(src->polnum+1));
  if (dst->type==NULL) s=-1;
  dst->box=malloc(2*src->sze*(src->polnum+1));
  if (dst->box==NULL) s=-1;
  dst->data=malloc(src->sze*src->pntnum);
  if (dst->data==NULL) s=-1;
  
  if (s !=0) {
    if (dst->num !=NULL) free(dst->num);
    if (dst->off !=NULL) free(dst->off);
    if (dst->type !=NULL) free(dst->type);
    if (dst->box !=NULL) free(dst->box);
    if (dst->data !=NULL) free(dst->data);
    free(dst);
  }
  dst->polnum=src->polnum;
  dst->pntnum=src->pntnum;
  dst->sze=src->sze;
  dst->bbox=src->bbox;

  memcpy(dst->num,src->num,sizeof(int)*(src->polnum+1));
  memcpy(dst->off,src->off,sizeof(int)*(src->polnum+1));
  memcpy(dst->type,src->type,sizeof(int)*(src->polnum+1));
  memcpy(dst->box,src->box,src->sze*2*(src->polnum+1));
  memcpy(dst->data,src->data,src->sze*src->pntnum);
  return dst;
}


int PolygonRemove(struct PolygonData *a) {
  void *tmp;
  if (a==NULL) return -1;
  if (a->polnum==0) return -1;
  a->pntnum-=a->num[a->polnum-1];
  a->num[a->polnum-1]=0;

  if (a->pntnum==0) {
    free(a->data);
    a->data=NULL;
  } else {
    tmp=realloc(a->data,a->sze*a->pntnum);
    if (tmp==NULL) return -1;
    a->data=tmp;
  }

  a->polnum--;
  if (a->polnum==0) {
    free(a->num);
    free(a->off);
    free(a->type);
    free(a->box);
    a->num=NULL;
    a->off=NULL;
    a->type=NULL;
    a->box=NULL;
  } else {
    tmp=realloc(a->num,a->polnum*sizeof(int));
    if (tmp==NULL) return -1;
    a->num=tmp;

    tmp=realloc(a->off,a->polnum*sizeof(int));
    if (tmp==NULL) return -1;
    a->off=tmp;

    tmp=realloc(a->type,a->polnum*sizeof(int));
    if (tmp==NULL) return -1;
    a->type=tmp;

    tmp=realloc(a->box,a->polnum*2*a->sze);
    if (tmp==NULL) return -1;
    a->box=tmp;
  }
 
  return 0;
}  

