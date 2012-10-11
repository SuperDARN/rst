/* MapBndFread.c
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

struct PolygonData *map;
struct PolygonData *nmap;
struct PolygonData *pmap;

struct PolygonData *bnd;
struct PolygonData *nbnd;
struct PolygonData *pbnd;

struct PolygonData *clip;

int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
} 

int main(int argc,char *argv[]) {

  struct Rplot *rplot=NULL;

  float wdt=400,hgt=400;
  float width=0.5;

  unsigned int bgcol=0xffffffff;
  unsigned int lndcol=0xffa0ffa0;
  unsigned int seacol=0xffa0a0ff;
  unsigned int grdcol=0xffa0a0a0;

  MapTransform  tfunc;
  float marg[3];
  
  char *envstr;
  FILE *mapfp;

  envstr=getenv("MAPDATA");

  mapfp=fopen(envstr,"r");
  map=MapFread(mapfp);
  fclose(mapfp);   

  envstr=getenv("BNDDATA");
  mapfp=fopen(envstr,"r");
  bnd=MapBndFread(mapfp);
  fclose(mapfp);

  marg[0]=90.0;
  marg[1]=0.0;
  marg[2]=1.0;

  tfunc=MapOrthographic;

  clip=MapCircleClip(10); 

  nmap=PolygonTransform(map,2*sizeof(float),PolygonXYbbox,
                        tfunc,marg);

  pmap=PolygonClip(clip,nmap); 

  nbnd=PolygonTransform(bnd,2*sizeof(float),PolygonXYbbox,
                      tfunc,marg);
  pbnd=PolygonClip(clip,nbnd);

 
  rplot=RplotMake();
  RplotSetText(rplot,stream,stdout); 
  RplotMakeDocument(rplot,"MapBndFread","1",wdt,hgt,24);
  RplotMakePlot(rplot,"MapBndFread",wdt,hgt,24);

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

  MapPlotOpenPolygon(rplot,NULL,0,0,wdt,hgt,grdcol,0x0f,width,NULL,
                    pbnd,-1);

  RplotEndPlot(rplot);
  RplotEndDocument(rplot);
  return 0;
}
