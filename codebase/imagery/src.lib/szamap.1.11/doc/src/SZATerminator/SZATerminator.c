/* SZATerminator.c
   ===============
   Author: R.J.Barnes
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
#include "rtypes.h"
#include "rxml.h"
#include "option.h"
#include "rfbuffer.h"
#include "rplot.h"
#include "rtime.h"
#include "polygon.h"
#include "rmap.h"
#include "contour.h"
#include "sza.h"
#include "szamap.h"

struct PolygonData *map;
struct PolygonData *nmap;
struct PolygonData *pmap;



struct PolygonData *trm;
struct PolygonData *ntrm;
struct PolygonData *ptrm;

struct PolygonData *clip;

int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
} 

int main(int argc,char *argv[]) {

  int yr,mo,dy,hr,mt,sc;

  struct Rplot *rplot=NULL;

  float wdt=400,hgt=400;

  unsigned int bgcol=0xffffffff;
  unsigned int lndcol=0xffa0ffa0;
  unsigned int seacol=0xffa0a0ff;
  unsigned int grdcol=0xffa0a0a0;
 
  MapTransform  tfunc;
  float marg[3];
  char *envstr;
  FILE *mapfp;

  yr=2002;
  mo=31;
  dy=26;
  hr=22;
  mt=0;
  sc=0;

  envstr=getenv("MAPDATA");

  mapfp=fopen(envstr,"r");
  map=MapFread(mapfp);
  fclose(mapfp);   

  trm=SZATerminator(yr,mo,dy,hr,mt,sc,1,0,1.0,105.0);


  marg[0]=90.0;
  marg[1]=0.0;
  marg[2]=1.0;

  tfunc=MapStereographic;

  clip=MapCircleClip(10); 

  nmap=PolygonTransform(map,2*sizeof(float),PolygonXYbbox,
                        tfunc,marg);

  pmap=PolygonClip(clip,nmap); 

  ntrm=PolygonTransform(trm,2*sizeof(float),PolygonXYbbox,
                        tfunc,marg);

  ptrm=PolygonClip(clip,ntrm); 



  rplot=RplotMake();
  RplotSetText(rplot,stream,stdout);   
  RplotMakeDocument(rplot,"SZATerminator","1",wdt,hgt,24);

  RplotMakePlot(rplot,"SZATerminator",wdt,hgt,24);

  RplotRectangle(rplot,NULL,0,0,wdt,hgt,
                  1,bgcol,0x0f,0,NULL);
  RplotEllipse(rplot,NULL,wdt/2,hgt/2,wdt/2,hgt/2,
                  1,seacol,0x0f,0,NULL);

  MapPlotPolygon(rplot,NULL,0,0,wdt,hgt,1,lndcol,0x0f,0,NULL,
                   pmap,1);

  MapPlotPolygon(rplot,NULL,0,0,wdt,hgt,1,lndcol,0x0f,0,NULL,
                   pmap,3);
  
  MapPlotPolygon(rplot,NULL,0,0,wdt,hgt,1,seacol,0x0f,0,NULL,
                   pmap,0);

  MapPlotPolygon(rplot,NULL,0,0,wdt,hgt,1,grdcol,0x0f,0,NULL,
                 ptrm,1);

 


  RplotEndPlot(rplot);
  RplotEndDocument(rplot);
  return 0;
}
