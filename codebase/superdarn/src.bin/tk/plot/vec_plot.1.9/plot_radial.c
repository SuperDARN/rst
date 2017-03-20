/* plot_radial.c
   =============
   Author: R.J.Barnes
*/

/*
   See license.txt
*/



#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "rfbuffer.h"
#include "iplot.h"
#include "rmap.h"

void plot_radial(struct Plot *plot,float *arg,float lat,
	         float start,float stop,float step,
	         float xoff,float yoff,float wdt,float hgt,
                 unsigned int color,unsigned char mask,float width,
                 struct PlotDash *dash) {

  float map[2]={0.0,0.0};
  float pnt[2];
  
  float ax,ay,bx,by;
  float lon;

  for (lon=start;lon<=stop;lon+=step) { 
    if (lat>0) map[0]=90.0;
    else map[0]=-90.0;
    map[1]=lon;
    MapStereographic(2*sizeof(float),map,2*sizeof(float),pnt,arg); 
    ax=pnt[0]*wdt+xoff;
    ay=pnt[1]*hgt+yoff;
    map[0]=lat;
    MapStereographic(2*sizeof(float),map,2*sizeof(float),pnt,arg); 
    bx=pnt[0]*wdt+xoff;
    by=pnt[1]*hgt+yoff;
    PlotLine(plot,ax,ay,bx,by,color,mask,width,dash);
  }
}






















