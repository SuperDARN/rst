/* plot_circle.c
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


void plot_circle(struct Plot *plot,
                 float *arg,float start,float stop,float step,
	         float xoff,float yoff,float wdt,float hgt,
                 unsigned int color,unsigned char mask,float width,
                 struct PlotDash *dash) {

  float map[2]={0,0};
  float pnt[2];
  
  float px,py;
  float rad;
  float lat;
  
  for (lat=start;lat<=stop;lat+=step) {
    map[0]=lat;
    MapStereographic(2*sizeof(float),map,2*sizeof(float),pnt,arg); 
    rad=wdt*sqrt((pnt[0]-0.5)*(pnt[0]-0.5)+(pnt[1]-0.5)*(pnt[1]-0.5));

    px=(xoff+wdt/2);
    py=(yoff+hgt/2);    
 
    PlotEllipse(plot,NULL,px,py,rad,rad,0,color,mask,width,dash);
  }
}






















