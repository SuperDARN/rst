/* PolygonClip.c
   =============
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




#include <stdio.h>
#include <stdlib.h>
#include "rtypes.h"
#include "rxml.h"
#include "option.h"
#include "rfbuffer.h"
#include "rplot.h"
#include "polygon.h"
#include "rmap.h"

struct PolygonData *poly;
struct PolygonData *clip;
struct PolygonData *clipped;


float fx[256],fy[256];
int t[256];

int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
} 

int main(int argc,char *argv[]) {

  struct Rplot *rplot=NULL;

  float wdt=400,hgt=400;

  unsigned int bgcol=0xffffffff;
  unsigned int fgcol=0xff000000;

  int i,j;
 
  float pnt[2]; 
  float *ptr;

  poly=PolygonMake(sizeof(float)*2,PolygonXYbbox);
  clip=PolygonMake(sizeof(float)*2,PolygonXYbbox);
 
  PolygonAddPolygon(poly,1);

  pnt[0]=0.1;
  pnt[1]=0.1;
  PolygonAdd(poly,pnt,0);

  pnt[0]=0.9;
  pnt[1]=0.1;
  PolygonAdd(poly,pnt,0);

  pnt[0]=0.9;
  pnt[1]=0.9;
  PolygonAdd(poly,pnt,0);

  pnt[0]=0.1;
  pnt[1]=0.9;
  PolygonAdd(poly,pnt,0);

  PolygonAddPolygon(clip,0);

  pnt[0]=0.0;
  pnt[1]=0.5;
  PolygonAdd(clip,pnt,0);

  pnt[0]=0.5;
  pnt[1]=0.0;
  PolygonAdd(clip,pnt,0);

  pnt[0]=1.0;
  pnt[1]=0.5;
  PolygonAdd(clip,pnt,0);

  pnt[0]=0.5;
  pnt[1]=1.0;
  PolygonAdd(clip,pnt,0);

  clipped=PolygonClip(poly,clip);
 
  rplot=RplotMake();
  RplotSetText(rplot,stream,stdout);   
  RplotMakeDocument(rplot,"MapPlotPolygon","1",wdt,hgt,24);
  RplotMakePlot(rplot,"MapPlotPolygon",wdt,hgt,24);

  RplotRectangle(rplot,NULL,0,0,wdt,hgt,
                  1,bgcol,0x0f,0,NULL);

  for (i=0;i<clipped->polnum;i++) {
  
    if (clipped->num[i]==0) continue;
    
    for (j=0;j<clipped->num[i];j++) {
      ptr=(float *) PolygonRead(clipped,i,j);
      fx[j]=ptr[0]*wdt;
      fy[j]=ptr[1]*hgt;
      t[j]=0;
    }
    RplotPolygon(rplot,NULL,0,0,clipped->num[i],fx,fy,t,
                  1,fgcol,0x0f,0,NULL);
  }

  RplotEndPlot(rplot);
  RplotEndDocument(rplot);
  return 0;
}
