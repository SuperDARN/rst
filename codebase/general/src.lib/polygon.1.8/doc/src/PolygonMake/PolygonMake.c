/* PolygonMake.c
   =============
   Author: R.J.Barnes
*/


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
  int type;
 
  float pnt[2]; 
  float *ptr;

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

 
  rplot=RplotMake();
  RplotSetText(rplot,stream,stdout);   
  RplotMakeDocument(rplot,"PolygonMake","1",wdt,hgt,24);

  RplotMakePlot(rplot,"PolygonMake",wdt,hgt,24);

  RplotRectangle(rplot,NULL,0,0,wdt,hgt,
                  1,bgcol,0x0f,0,NULL);

  for (i=0;i<poly->polnum;i++) {
    type=PolygonReadType(poly,i);
    if (type==1) fgcol=RplotColor(255,0,0,255);
    if (type==2) fgcol=RplotColor(0,255,0,255);

    for (j=0;j<poly->num[i];j++) {
      ptr=(float *) PolygonRead(poly,i,j);
      fx[j]=ptr[0]*wdt;
      fy[j]=ptr[1]*hgt;
      t[j]=0;
    }
    RplotPolygon(rplot,NULL,0,0,poly->num[i],fx,fy,t,
                  1,fgcol,0x0f,0,NULL);
  }

  RplotEndPlot(rplot);
  RplotEndDocument(rplot);
  return 0;
}
