/* plot_vec.c
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
#include "aacgm.h"
#include "rfbuffer.h"
#include "iplot.h"
#include "calcvector.h"
#include "text_box.h"




void plot_vec(struct Plot *plot,float px,float py,int or,double max,
              int magflg,
              float xoff,float yoff,float wdt,float hgt,float sf,float rad,
              int (*trnf)(int,void *,int,void *,void *data),void *data,
              unsigned int color,char mask,float width,
              char *fontname,float fontsize,
              void *txtdata) {

  int s;
  char txt[256];
  double lon,lat;
  float mag;
  float map[2],pnt[2];
  float ax,ay,bx,by;
  float txbox[3];

  lat=90.0;
  lon=0.0;

  if (data !=NULL) {
    float *arg;
    arg=(float *) data;
    if (arg[0]>90) lat=90.0;
    else lat=-90.0;
  }

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
  if (s==-1) return;
  ax=xoff+pnt[0]*wdt;
  ay=yoff+pnt[1]*hgt;    
      

  RPosCalcVector(lat,0.0,max*sf,0.0,&lat,&lon);

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
  if (s==-1) return;

  bx=xoff+pnt[0]*wdt;
  by=yoff+pnt[1]*hgt;    
    
  mag=sqrt((bx-ax)*(bx-ax)+(by-ay)*(by-ay));

  if (rad>0) PlotEllipse(plot,NULL,px,py,
               rad,rad,1,color,mask,0,NULL);
   
  if (or==0) PlotLine(plot,px,py,px+mag,py,color,0x0f,width,NULL);    
  else if (or==1) PlotLine(plot,px,py,px-mag,py,color,0x0f,width,NULL);    
  else if (or==2) PlotLine(plot,px,py-mag,px,py,color,0x0f,width,NULL);    
  else if (or==3) PlotLine(plot,px,py,px,py+mag,color,0x0f,width,NULL);    
  
  sprintf(txt,"%g m/s",max);
  txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);
  
  if (or==0) PlotText(plot,NULL,fontname,fontsize,px-4,py-(txbox[2]-txbox[1]),
                       strlen(txt),txt,color,mask,1);
  

 
}
