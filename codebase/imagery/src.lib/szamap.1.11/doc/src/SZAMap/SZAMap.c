/* SZAMap.c
   ========
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
#include "gmap.h"
#include "sza.h"
#include "szamap.h"

float *zenith;

int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
} 

int main(int argc,char *argv[]) {

  char *dayname="day.ppm";
  char *nightname="night.ppm";
  FILE *fp;

  int yr,mo,dy,hr,mt,sc;

  struct Rplot *rplot=NULL;

  struct FrameBuffer *day=NULL;
  struct FrameBuffer *night=NULL;

  struct FrameBuffer *img=NULL;


  float wdt=400,hgt=400;
  int x,y,px,py,pwdt,phgt;

 
  MapTransform  tfunc;

  float marg[3];

  float *lat=NULL,*lon=NULL;
  float Z;

  int rv,gv,bv;

  yr=2002;
  mo=31;
  dy=26;
  hr=16;
  mt=0;
  sc=0;

  fp=fopen(dayname,"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  day=FrameBufferLoadPPM(fp,dayname);

  if (day==NULL) {
    fprintf(stderr,"Error loading image map.\n");
    exit(-1);
  }
  fclose(fp);


  fp=fopen(nightname,"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  night=FrameBufferLoadPPM(fp,nightname);

  if (night==NULL) {
    fprintf(stderr,"Error loading image map.\n");
    exit(-1);
  }
  fclose(fp);

  img=FrameBufferMake("dummy",wdt,hgt,24);

  marg[0]=90.0;
  marg[1]=0.0;
  marg[2]=1.0;

  tfunc=MapOrthographic;


  GeoMap(wdt,hgt,tfunc,marg,&lat,&lon);

  zenith=SZAMap(yr,mo,dy,hr,mt,sc,-90.0,
                   wdt,hgt,0,tfunc,marg);   

  pwdt=day->wdt;
  phgt=day->hgt;

  for (y=0;y<hgt;y++) {
    for (x=0;x<wdt;x++) {
      if (lat[y*(int) wdt+x]<-90) continue;
      Z=zenith[y*(int) wdt+x];

      if (Z>90) Z=(1-(Z-90)/10.0); 
      else Z=1.0;

      if (Z>1.0) Z=1.0;
      if (Z<0) Z=0;


      px=day->wdt*lon[y*(int) wdt+x]/360.0;
      py=day->hgt*(0.5-lat[y*(int) wdt+x]/180.0);

      rv=Z*day->img[py*pwdt+px]+(1-Z)*night->img[py*pwdt+px];
      gv=Z*day->img[pwdt*phgt+py*pwdt+px]+
           (1-Z)*night->img[pwdt*phgt+py*pwdt+px];
      bv=Z*day->img[2*pwdt*phgt+py*pwdt+px]+
           (1-Z)*night->img[2*pwdt*phgt+py*pwdt+px];


  
      if (rv>255) rv=255;
      if (rv<0) rv=0;

      if (gv>255) gv=255;
      if (gv<0) gv=0;

      if (bv>255) bv=255;
      if (bv<0) bv=0;

      img->img[y*(int) wdt+x]=rv;
      img->img[(int) (wdt*hgt)+y*(int) wdt+x]=gv;
      img->img[(int) (2*wdt*hgt)+y*(int) wdt+x]=bv;

    }
  }

  rplot=RplotMake();
  RplotSetText(rplot,stream,stdout);
  RplotMakeDocument(rplot,"SZAMap","1",wdt,hgt,24);

   
  RplotMakePlot(rplot,"SZAMap",wdt,hgt,24);

  RplotImage(rplot,NULL,img,0x0f,0,0,1);  


  RplotEndPlot(rplot);
  RplotEndDocument(rplot);
  return 0;
}
