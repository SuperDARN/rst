/* PolygonTransform.c
   ==================
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
#include <math.h>
#include "rtypes.h"
#include "rmath.h"
#include "rxml.h"
#include "option.h"
#include "rfbuffer.h"
#include "rplot.h"
#include "polygon.h"
#include "rmap.h"

struct PolygonData *poly;
struct PolygonData *npoly;

float fx[256],fy[256];
int t[256];

int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
} 

int rotate(int ssze,void *src,int dsze,void *dst,void *data) {

  float *arg=NULL;
  float *pnt;
  float px,py;
  float rad;
  arg=(float *) data;
  rad=arg[0]*PI/180.0;
 
  pnt=(float *) src;
  px=pnt[0];
  py=pnt[1];
  pnt=(float *) dst;
  
  pnt[0]=0.5+(px-0.5)*cos(rad)-(py-0.5)*sin(rad);
  pnt[1]=0.5+(px-0.5)*sin(rad)+(py-0.5)*cos(rad);
  return 0;
}

int main(int argc,char *argv[]) {

  struct Rplot *rplot=NULL;

  float wdt=400,hgt=400;

  unsigned int bgcol=0xffffffff;
  unsigned int fgcol=0xff000000;

  int i,j;
  int type;
 
  float pnt[2]; 
  float *ptr;

  float arg[1];

  poly=PolygonMake(sizeof(float)*2,PolygonXYbbox);
 
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

  PolygonAddPolygon(poly,2);

  pnt[0]=0.2;
  pnt[1]=0.5;
  PolygonAdd(poly,pnt,0);

  pnt[0]=0.5;
  pnt[1]=0.2;
  PolygonAdd(poly,pnt,0);

  pnt[0]=0.8;
  pnt[1]=0.5;
  PolygonAdd(poly,pnt,0);

  pnt[0]=0.5;
  pnt[1]=0.8;
  PolygonAdd(poly,pnt,0);

  arg[0]=30.0;

  npoly=PolygonTransform(poly,sizeof(float)*2,PolygonXYbbox,rotate,arg);

 
  rplot=RplotMake();
  RplotSetText(rplot,stream,stdout);
  RplotMakeDocument(rplot,"PolygonTransform","1",wdt,hgt,24);
  RplotMakePlot(rplot,"PolygonTransform",wdt,hgt,24);

  RplotRectangle(rplot,NULL,0,0,wdt,hgt,
                  1,bgcol,0x0f,0,NULL);

  for (i=0;i<npoly->polnum;i++) {
    type=PolygonReadType(npoly,i);
    if (type==1) fgcol=RplotColor(255,0,0,255);
    if (type==2) fgcol=RplotColor(0,255,0,255);

    for (j=0;j<npoly->num[i];j++) {
      ptr=(float *) PolygonRead(npoly,i,j);
      fx[j]=ptr[0]*wdt;
      fy[j]=ptr[1]*hgt;
      t[j]=0;
    }
    RplotPolygon(rplot,NULL,0,0,npoly->num[i],fx,fy,t,
                  1,fgcol,0x0f,0,NULL);
  }

  RplotEndPlot(rplot);
  RplotEndDocument(rplot);
  return 0;
}
