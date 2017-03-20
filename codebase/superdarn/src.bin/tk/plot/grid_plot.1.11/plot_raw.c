/* plot_raw.c
   ========== 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include "rtypes.h"
#include "aacgm.h"
#include "rfbuffer.h"
#include "iplot.h"
#include "rfile.h"
#include "calcvector.h"
#include "griddata.h"


#define VEL_MAX 25000

void plot_raw(struct Plot *plot,
              struct GridData *ptr,float latmin,int magflg,
              float xoff,float yoff,float wdt,float hgt,float sf,float rad,
              int (*trnf)(int,void *,int,void *,void *data),void *data,
              unsigned int(*cfn)(double,void *),void *cdata,
              float width) {

  int i,s;
  double olon,olat,lon,lat,vazm;
  float map[2],pnt[2];
  unsigned int color=0;
  float ax,ay,bx,by;

  for (i=0;i<ptr->vcnum;i++) {
    
    if (! isfinite(ptr->data[i].vel.median)) continue;
    if (fabs(ptr->data[i].vel.median)>VEL_MAX) continue;

    olon=ptr->data[i].mlon;
    olat=ptr->data[i].mlat;
    vazm=ptr->data[i].azm;

    lat=olat;
    lon=olon;
    if (!magflg) {
      double mlat,mlon,glat,glon,r;
      int s;
      mlat=lat;
      mlon=lon;
      s=AACGMConvert(mlat,mlon,150,&glat,&glon,&r,1);
      lat=glat;
      lon=glon;
    }

    if (fabs(lat)<fabs(latmin)) continue;
    if (cfn !=NULL) color=(*cfn)(ptr->data[i].vel.median,cdata);
    
    map[0]=lat;
    map[1]=lon;
   
    s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
  
    if (s==-1) continue;
    ax=xoff+pnt[0]*wdt;
    ay=yoff+pnt[1]*hgt;    
    
    RPosCalcVector(olat,olon,ptr->data[i].vel.median*sf,vazm,&lat,&lon);

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
    if (s==-1) continue;
    bx=xoff+pnt[0]*wdt;
    by=yoff+pnt[1]*hgt;    
    
    if (rad>0) PlotEllipse(plot,NULL,ax,ay,
                 rad,rad,1,color,0x0f,0,NULL);
    
    PlotLine(plot,ax,ay,bx,by,color,0x0f,width,NULL);    
    
  } 
}
