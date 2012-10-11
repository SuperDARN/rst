/* radarscan.c
   ============ 
   Author R.J.Barnes
*/


/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "scandata.h"


struct RadarScan *RadarScanMake() {
  struct RadarScan *ptr=NULL;
  
  ptr=malloc(sizeof(struct RadarScan));
  if (ptr==NULL) return NULL;
  memset(ptr,0,sizeof(struct RadarScan));
  ptr->bm=NULL;
  return ptr;
}


void RadarScanFree(struct RadarScan *ptr) {
  if (ptr==NULL) return;
  RadarScanReset(ptr);
  free(ptr);
}
  

int RadarScanReset(struct RadarScan *ptr) {
  int n;
  if (ptr==NULL) return -1;
  if (ptr->bm !=NULL) {
    for (n=0;n<ptr->num;n++) {
      if (ptr->bm[n].sct !=NULL) free(ptr->bm[n].sct);
      if (ptr->bm[n].rng !=NULL) free(ptr->bm[n].rng);
    }
    free(ptr->bm);
  }
  ptr->bm=0;
  ptr->num=0;
  return 0;
}



int RadarScanResetBeam(struct RadarScan *ptr,int bmnum,int *bmptr) {
  int c,n,num=0;
  struct RadarBeam *tmp;
  if (bmnum==0) return 0;
  if (bmptr==NULL) return -1;
  if (ptr==NULL) return -1;
  if (ptr->num==0) return -1;
  tmp=malloc(sizeof(struct RadarBeam)*ptr->num);
  if (tmp==NULL) return -1;

  for (n=0;n<ptr->num;n++) {
    for (c=0;c<bmnum;c++) 
      if (ptr->bm[n].bm==bmptr[c]) break;
    if (c !=bmnum) {
      if (ptr->bm[n].sct !=NULL) free(ptr->bm[n].sct);
      if (ptr->bm[n].rng !=NULL) free(ptr->bm[n].rng);
      continue;
    } 
    memcpy(&tmp[num],&ptr->bm[n],sizeof(struct RadarBeam));
    num++;
  }
  free(ptr->bm);
  if (num>0) {
    ptr->bm=realloc(tmp,sizeof(struct RadarBeam)*num);
    if (ptr->bm==NULL) {
      free(tmp);
      ptr->num=0;
      return -1;
    }
  } else {
    free(tmp);
    ptr->bm=NULL;
  }
  ptr->num=num;
  return 0;
}

struct RadarBeam *RadarScanAddBeam(struct RadarScan *ptr,int nrang) {
  void *tmp;
  struct RadarBeam *bm=NULL;

  int s=0;
  if (ptr==NULL) return NULL;
  
  if (ptr->bm==NULL) tmp=malloc(sizeof(struct RadarBeam));
  else tmp=realloc(ptr->bm,sizeof(struct RadarBeam)*(ptr->num+1));
  if (tmp==NULL) return NULL;

  ptr->bm=tmp;
  bm=&ptr->bm[ptr->num];
  memset(bm,0,sizeof(struct RadarBeam));
  bm->sct=NULL;
  bm->rng=NULL;


  if (nrang !=0) {
    bm->nrang=nrang;
    bm->sct=malloc(sizeof(char)*nrang);
    if (bm->sct==NULL) s=-1;
    bm->rng=malloc(sizeof(struct RadarCell)*nrang);
    if (bm->rng==NULL) s=-1;
  } 

  if (s==-1) {
    if (bm->sct !=NULL) free(bm->sct);
    if (bm->rng !=NULL) free(bm->rng);
    if (ptr->num==0) {
      free(ptr->bm);
      tmp=NULL;
    } else tmp=realloc(ptr->bm,sizeof(struct RadarBeam)*ptr->num); 
    ptr->bm=tmp;
    return NULL;
  }

  if (nrang !=0) {
    memset(bm->sct,0,sizeof(char)*nrang);
    memset(bm->rng,0,sizeof(struct RadarCell)*nrang);
  }
  ptr->num++;
  return bm;
}



