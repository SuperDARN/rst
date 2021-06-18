/* wrap.c
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
#include <math.h> 
#include <sys/types.h>
#include "rtypes.h"
#include "rfbuffer.h"
#include "iplot.h"
#include "polygon.h"
#include "rmath.h"
#include "rmap.h"



struct PolygonData *MapWraparound(struct PolygonData *map) {

  struct PolygonData *clip=NULL;
  struct PolygonData *ptr=NULL;
  struct PolygonData *dst=NULL;
  float *pnt;

  float dpnt[2];

  int c=0,i,j;
  int left,right;
  int top,bottom;
 
  float ox,oy,px,py; 

  clip=MapSquareClip();
  if (clip==NULL) return NULL;

  ptr=PolygonMake(2*sizeof(float),PolygonXYbbox);
  
  for (i=0;i<map->polnum;i++) {
    left=0;
    right=0;
    top=0;
    bottom=0;
 
    PolygonAddPolygon(ptr,map->type[i]);
    pnt=(float *) PolygonRead(map,i,0);
    ox=pnt[0];
    oy=pnt[1];
    dpnt[0]=ox;
    dpnt[1]=oy;
    PolygonAdd(ptr,dpnt);
    c++;
    for (j=1;j<map->num[i];j++) {
      pnt=(float *) PolygonRead(map,i,j);
      px=pnt[0];
      py=pnt[1];
     

      if ((ox-px)>0.95) {
        px+=1.0;
        left=1;
      } else if ((ox-px)<-0.95) {
        px-=1.0;
        right=1;
      }

      if ((oy-py)>0.95) {
	py+=1.0;
        top=1;
      } else if ((oy-py)<-0.95) {
        py-=1.0;
        bottom=1;
      }
      dpnt[0]=px;
      dpnt[1]=py;
      PolygonAdd(ptr,dpnt);
      ox=px;
      oy=py;
    }
    
    if (right) {
      PolygonAddPolygon(ptr,ptr->type[c-1]);
      for (j=0;j<ptr->num[c-1];j++) {
         pnt=(float *) PolygonRead(ptr,c-1,j);
         dpnt[0]=pnt[0]+1.0;
         dpnt[1]=pnt[1];
         PolygonAdd(ptr,dpnt);
      }   
      c++;         
    }
    
    if (left) {
      PolygonAddPolygon(ptr,ptr->type[c-1]);
      for (j=0;j<ptr->num[c-1];j++) {
         pnt=(float *) PolygonRead(ptr,c-1,j);
         dpnt[0]=pnt[0]-1.0;
         dpnt[1]=pnt[1];
         PolygonAdd(ptr,dpnt);
      }       
      c++;
    }
    if (top) {
      PolygonAddPolygon(ptr,ptr->type[c-1]);
      for (j=0;j<ptr->num[c-1];j++) {
         pnt=(float *) PolygonRead(ptr,c-1,j);
         dpnt[0]=pnt[0];
         dpnt[1]=pnt[1]+1.0;
         PolygonAdd(ptr,dpnt);
      }       
      c++;
    }
    if (bottom) {
      PolygonAddPolygon(ptr,ptr->type[c-1]);
      for (j=0;j<ptr->num[c-1];j++) {
         pnt=(float *) PolygonRead(ptr,c-1,j);
         dpnt[0]=pnt[0];
         dpnt[1]=pnt[1]-1.0;
         PolygonAdd(ptr,dpnt);
      }       
      c++;
    }
    
  }      
  dst=PolygonClip(clip,ptr);  
  PolygonFree(clip);
  PolygonFree(ptr);
  return dst;
}

