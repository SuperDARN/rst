/* plot_excluded.c
   =============== 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "aacgm.h"
#include "rfbuffer.h"
#include "iplot.h"
#include "calcvector.h"
#include "cnvgrid.h"




void plot_excluded(struct Plot *plot,
              struct CnvGrid *ptr,float latmin,int magflg,
	      float xoff,float yoff,float wdt,float hgt,float sf,float rad,
              int (*trnf)(int,void *,int,void *,void *data),void *data,
              unsigned int color,float width) {

  int i,s;
  double olon,olat,lon,lat,vazm;
  float map[2],pnt[2];
  float ax,ay;

  for (i=0;i<ptr->num;i++) {
    
    olon=ptr->lon[i];
    olat=ptr->lat[i];
    vazm=ptr->azm[i];

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

    if (fabs(lat)>=fabs(latmin)) continue;  
    map[0]=lat;
    map[1]=lon;
   
    s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
  
    if (s==-1) continue;
    ax=xoff+pnt[0]*wdt;
    ay=yoff+pnt[1]*hgt;    
   
    PlotEllipse(plot,NULL,ax,ay,
                 rad,rad,1,color,0x0f,0,NULL);
     
  } 
}
