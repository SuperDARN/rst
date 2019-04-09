/* plot_radial.c
   =============
   Author: R.J.Barnes
*/

/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
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






















