/* MapTransverseMercator.c
   =======================
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

struct PolygonData *map;
struct PolygonData *nmap;
struct PolygonData *wmap;
struct PolygonData *pmap;

struct PolygonData *clip;

int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
} 

int main(int argc,char *argv[]) {

  struct Rplot *rplot=NULL;

  float wdt=400,hgt=200;

  float src[2],dst[2];

  unsigned int lndcol=0xffa0ffa0;
  unsigned int seacol=0xffa0a0ff;
  unsigned int grdcol=0xffa00000;
 
  MapTransform  tfunc;
  float marg[3];
  int s;  
  char *envstr;
  FILE *mapfp;

  envstr=getenv("MAPDATA");

  mapfp=fopen(envstr,"r");
  map=MapFread(mapfp);
  fclose(mapfp);   

  marg[0]=0.0;
  marg[1]=0.0;
  marg[2]=1.0;

  tfunc=MapTransverseMercator;

  clip=MapSquareClip(10); 

  nmap=PolygonTransform(map,2*sizeof(float),PolygonXYbbox,
                        tfunc,marg);
  wmap=MapWraparound(nmap);
  pmap=PolygonClip(clip,wmap); 
 
  rplot=RplotMake();
  RplotSetText(rplot,stream,stdout);   

  RplotMakeDocument(rplot,"MapTransverseMercator","1",wdt,hgt,24);
  RplotMakePlot(rplot,"MapTransverseMercator",wdt,hgt,24);

  RplotRectangle(rplot,NULL,0,0,wdt,hgt,
                  1,seacol,0x0f,0,NULL);

  MapPlotPolygon(rplot,NULL,0,0,wdt,hgt,1,lndcol,0x0f,0,NULL,
                   pmap,1);

  MapPlotPolygon(rplot,NULL,0,0,wdt,hgt,1,lndcol,0x0f,0,NULL,
                   pmap,3);
  
  MapPlotPolygon(rplot,NULL,0,0,wdt,hgt,1,seacol,0x0f,0,NULL,
                   pmap,0);

  src[0]=53.2;
  src[1]=160.0;

  s=MapTransverseMercator(2*sizeof(float),src,2*sizeof(float),dst,marg);

  if (s==0) 
    RplotEllipse(rplot,NULL,wdt*dst[0],hgt*dst[1],8,8,
                  1,grdcol,0x0f,0,NULL);


  RplotEndPlot(rplot);
  return 0;
}
