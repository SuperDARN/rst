/* plot_cell.c
   =========== 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>
#include "rmath.h"
#include "rtypes.h"
#include "aacgm.h"
#include "rfbuffer.h"
#include "iplot.h"
#include "rfile.h"
#include "calcvector.h"
#include "griddata.h"



int cell_convert(float xoff,float yoff,float wdt,float hgt,
                 float lat,float lon,float *px,float *py,int magflg,
                 int (*trnf)(int,void *,int,void *,void *data),void *data) {
  int s;
  float map[2],pnt[2];

   if (!magflg) {
      double mlat,mlon,glat,glon,r;
      int s;
      mlat=lat;
      mlon=lon;
      s=AACGMConvert(mlat,mlon,150,&glat,&glon,&r,1);
      lat=glat;
      lon=glon;
    }
    map[0]=lat;
    map[1]=lon;
    s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
    if (s !=0) return -1;
    *px=xoff+wdt*pnt[0];
    *py=yoff+hgt*pnt[1]; 
    return 0;
}

void plot_cell(struct Plot *plot,
               struct GridData *ptr,float latmin,int magflg,
               float xoff,float yoff,float wdt,float hgt,
               int (*trnf)(int,void *,int,void *,void *data),void *data,
               unsigned int(*cfn)(double,void *),void *cdata,
               int cprm) {


  int i,s,nlon;
  double lon,lat,lstp;
 
  unsigned int color=0;
  float px[4],py[4];
  int t[4]={0,0,0,0};

  for (i=0;i<ptr->vcnum;i++) {
    if (cfn !=NULL) {
      if (cprm==0) color=(*cfn)(ptr->data[i].pwr.median,cdata);
      else color=(*cfn)(ptr->data[i].wdt.median,cdata);
    }
    lon=ptr->data[i].mlon;
    lat=ptr->data[i].mlat;
    if (lat<latmin) continue;
    nlon=(int) (360*cos((lat-0.5)*PI/180)+0.5);
    lstp=360.0/nlon; 
    s=cell_convert(xoff,yoff,wdt,hgt,lat-0.5,lon-lstp/2,&px[0],&py[0],
                 magflg,trnf,data);
    if (s !=0) continue;
    s=cell_convert(xoff,yoff,wdt,hgt,lat-0.5,lon+lstp/2,&px[1],&py[1],
                 magflg,trnf,data);
    if (s !=0) continue;
    s=cell_convert(xoff,yoff,wdt,hgt,lat+0.5,lon+lstp/2,&px[2],&py[2],
                 magflg,trnf,data);
    if (s !=0) continue;
    s=cell_convert(xoff,yoff,wdt,hgt,lat+0.5,lon-lstp/2,&px[3],&py[3],
                 magflg,trnf,data);
    if (s !=0) continue;   
    PlotPolygon(plot,NULL,0,0,4,px,py,t,1,color,0x0f,0,NULL);
    
        
    
  } 

}
