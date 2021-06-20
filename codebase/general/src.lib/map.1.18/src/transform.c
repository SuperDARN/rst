/* transform.c
   ===========
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
#ifndef _DARWIN
#include <malloc.h>
#endif
#include <math.h>
#include "rfbuffer.h"
#include "iplot.h"
#include "polygon.h"
#include "rmap.h"

#define FSTEP 0.05
#define DMIN 0.00005

float MapProximity(int sze,int isze,void *p1,void *p2,int (*trf)(int ssze,void *src,int dsze,
				      void *dst,void *data),void *data) {

  int s;
  char *dpnt=NULL;
  float *pptr=NULL;

  float ax,ay,bx,by,d=0;

  dpnt=malloc(sze);
  if (dpnt==NULL) return -1;
  pptr=(float *) dpnt;

  s=(*trf)(isze,p1,sze,dpnt,data);
  if (s !=0) {
    free(dpnt);
    return -1;
  }
  ax=pptr[0];
  ay=pptr[1];


  s=(*trf)(isze,p2,sze,dpnt,data);
  if (s !=0) {
    free(dpnt);
    return -1;
  }
  bx=pptr[0];
  by=pptr[1];

  d=sqrt((bx-ax)*(bx-ax)+(by-ay)*(by-ay));

  free(dpnt);
  return d;

}

struct PolygonData *MapTransform(struct PolygonData *src,
                                     int sze,
				     int (*bbox)(void *x,
						void *y,int s),
                                     int (*trf)(int ssze,void *src,int dsze,
                                                void *dst,void *data),
                                     void *data) {

  int flg,iflg;
  void *spnt,*p1,*p2;
  char *dpnt,*ipnt;
  int n,s;
  int i,j;
  int oj,fj;
  double f;
  float d=0;

  struct PolygonData *dst=NULL;

  if (src==NULL) return NULL;
  if (trf==NULL) return NULL;

  dpnt=malloc(sze);
  ipnt=malloc(src->sze);
  if (dpnt==NULL) {
    free(ipnt);
    return NULL;
  }
  dst=PolygonMake(sze,bbox);
  if (dst==NULL) {
    free(ipnt);
    free(dpnt);
    return NULL;
  }
  
  for (i=0;i<src->polnum;i++) {    
    flg=1;
    n=src->num[i];
    oj=-1;
    fj=-1;
    iflg=0;
    for (j=0;j<n;j++) {
      spnt=PolygonRead(src,i,j);
      s=(*trf)(src->sze,spnt,sze,dpnt,data);
      if (s !=0) {
        iflg=1;       
        continue;
      }
      if (flg==1) {
        s=PolygonAddPolygon(dst,src->type[i]);
        if (s !=0) break;
        flg=0;
        fj=j;
      }
      
      if ((oj !=-1) && (iflg==1)) { /* fill in missing segment */
        p1=PolygonRead(src,i,oj);
        p2=PolygonRead(src,i,j);
	
	d=MapProximity(sze,src->sze,p1,p2,trf,data);
	if (d<DMIN) {
          oj=j;
          continue;
	}
	
        for (f=0;f<1;f+=FSTEP) {
          MapGreatCircle(p1,p2,f,ipnt);
          s=(*trf)(src->sze,ipnt,sze,dpnt,data);
          if (s==0) {
            s=PolygonAdd(dst,dpnt);
            if (s !=0) break;
	  }
        }
        oj=j;
      } else {
        oj=j;
        s=PolygonAdd(dst,dpnt);
        if (s !=0) break;
      }
    }

    if ((flg==0) && (iflg==1)) { /* close segment */

      p1=PolygonRead(src,i,oj);
      p2=PolygonRead(src,i,fj);

      d=MapProximity(sze,src->sze,p1,p2,trf,data);
      if (d<DMIN) continue;
     
      for (f=0;f<1;f+=FSTEP) {
        MapGreatCircle(p1,p2,f,ipnt);
        s=(*trf)(src->sze,ipnt,sze,dpnt,data);
        if (s==0) {
          s=PolygonAdd(dst,dpnt);
          if (s !=0) break;
	}
      }
    }


    if (j !=n) break;
  }
  free(ipnt);
  free(dpnt);

  if (i !=src->polnum) {
    PolygonFree(dst);
    return NULL;
  } 
  return dst;
}


void MapModify(struct PolygonData *src,
                   int (*trf)(int ssze,void *src,int dsze,
                              void *dst,void *data),void *data) {

  void *pnt;
  int n,s;
  int i,j;
  if (src==NULL) return;
  if (trf==NULL) return;
 
  for (i=0;i<src->polnum;i++) {    
    n=src->num[i];       
    for (j=0;j<n;j++) {
      pnt=PolygonRead(src,i,j);
      s=(*trf)(src->sze,pnt,src->sze,pnt,data);
      if (s !=0) {
        continue;
      }
    }
  }
}


