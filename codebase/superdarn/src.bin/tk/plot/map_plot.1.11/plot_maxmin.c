/* plot_maxmin.c
   ============= 
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
#include "cnvgrid.h"




void plot_maxmin(struct Plot *plot,
                 struct CnvGrid *ptr,int magflg,
		 float xoff,float yoff,float wdt,float hgt,float sze,
                 int (*trnf)(int,void *,int,void *,void *data),void *data,
                 unsigned int color,char mask,float width,
                 struct PlotDash *dash) {

  int i,s;

  double min=1e10;
  double max=-1e10;
  float max_lon=0,max_lat=0;
  float min_lon=0,min_lat=0;
  float map[2],pnt[2];
  float px,py;

  for (i=0;i<ptr->num;i++) {
    if (ptr->mag[i]>max) {
       max_lon=ptr->lon[i];
       max_lat=ptr->lat[i];
       max=ptr->mag[i];
    }
    if (ptr->mag[i]<min) {
       min_lon=ptr->lon[i];
       min_lat=ptr->lat[i];
       min=ptr->mag[i];
    }
  }  
  
  if (!magflg) {
    double mlat,mlon,glat,glon,r;
    int s;
    mlat=max_lat;
    mlon=max_lon;
    s=AACGMConvert(mlat,mlon,150,&glat,&glon,&r,1);
    max_lat=glat;
    max_lon=glon;
    mlat=min_lat;
    mlon=min_lon;
    s=AACGMConvert(mlat,mlon,150,&glat,&glon,&r,1);
    min_lat=glat;
    min_lon=glon;
  }

   
  map[0]=max_lat;
  map[1]=max_lon;
   
  s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
  
  if (s==0) {
    px=xoff+pnt[0]*wdt;
    py=yoff+pnt[1]*hgt;       
    PlotLine(plot,px-sze,py,px+sze,py,color,mask,width,dash);   
    PlotLine(plot,px,py-sze,px,py+sze,color,mask,width,dash);   
 
  } 

  map[0]=min_lat;
  map[1]=min_lon;
   
  s=(*trnf)(2*sizeof(float),map,2*sizeof(float),pnt,data);
  
  if (s==0) {
    px=xoff+pnt[0]*wdt;
    py=yoff+pnt[1]*hgt;       
    PlotLine(plot,px-sze,py+sze,px+sze,py-sze,color,mask,width,dash);  
    PlotLine(plot,px-sze,py-sze,px+sze,py+sze,color,mask,width,dash);  
  } 

}
