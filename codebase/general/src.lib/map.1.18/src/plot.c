/* plot.c
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



void MapPlotPolygon(struct Plot *plot,struct PlotMatrix *matrix,
                    float x,float y,float w,float h,int fill,
                    unsigned int color,unsigned char mask,
                    float width,struct PlotDash *dash,
                    struct PolygonData *poly,int type) {
  float *ptr;
  float *px=NULL,*py=NULL;
  int *t=NULL;
  int i,j;

  if (poly==NULL) return;
  
  for (i=0;i<poly->polnum;i++) {  
    if (poly->type[i] !=type) continue;
    px=malloc(sizeof(float)*poly->num[i]);
    py=malloc(sizeof(float)*poly->num[i]);
    t=malloc(sizeof(int)*poly->num[i]);
    for (j=0;j<poly->num[i];j++) {
      ptr=(float *) PolygonRead(poly,i,j);
      px[j]=ptr[0]*w;
      py[j]=ptr[1]*h;
      t[j]=0;
    } 
    PlotPolygon(plot,matrix,x,y,poly->num[i],px,py,t,fill,
                 color,mask,width,dash);    
    free(px);
    free(py);
    free(t);
  }  
}

void MapPlotOpenPolygon(struct Plot *plot,struct PlotMatrix *matrix,
                    float x,float y,float w,float h,
                    unsigned int color,unsigned char mask,
                    float width,struct PlotDash *dash,
                    struct PolygonData *poly,int type) {
  float *ptr;
  int i,j;
  float ox,oy,nx,ny;
  
  if (poly==NULL) return;

  for (i=0;i<poly->polnum;i++) { 
    if ((type !=-1) && (poly->type[i] !=type)) continue;
    ptr=(float *) PolygonRead(poly,i,0);
    ox=x+ptr[0]*w;
    oy=y+ptr[1]*h;
    if (matrix !=NULL) PlotMatrixTransform(matrix,ox,oy,&ox,&oy);

    for (j=1;j<poly->num[i];j++) {
      ptr=(float *) PolygonRead(poly,i,j);
      nx=x+ptr[0]*w;
      ny=y+ptr[1]*h;
      if (matrix !=NULL) PlotMatrixTransform(matrix,nx,ny,&nx,&ny);
      PlotLine(plot,ox,oy,nx,ny,color,mask,width,dash);
      ox=nx;
      oy=ny;
    }   
  }
}

