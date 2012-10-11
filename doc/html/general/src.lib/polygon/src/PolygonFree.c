/* PolygonFree.c
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
 
  rplot=RplotMake();
  RplotSetText(rplot,stream,stdout); 

  RplotMakeDocument(rplot,"PolygonFree","1",wdt,hgt,24);

  RplotMakePlot(rplot,"PolygonFree",wdt,hgt,24);

  RplotRectangle(rplot,NULL,0,0,wdt,hgt,
                  1,bgcol,0x0f,0,NULL);

  for (i=0;i<poly->polnum;i++) {
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

  PolygonFree(poly);

  return 0;
}
