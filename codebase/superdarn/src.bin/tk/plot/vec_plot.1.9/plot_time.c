/* plot_time.c
   ===========
   Author: R.J.Barnes
*/

/*
  Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/ 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "rmath.h"
#include "rfbuffer.h"
#include "iplot.h"
#include "rmap.h"
#include "text_box.h"




void plot_time(struct Plot *plot,
               float xoff,float yoff,float wdt,float hgt,
               unsigned int color,unsigned char mask,
               char *fontname,float fontsize,
               void *txtdata) {
  float txbox[3];
  char txt[16];
  int i;

  float px,py;
  float lon;
  float rad=wdt/2;
  double x,y;
  float xtxt=0,ytxt=0;
  
  for (i=0;i<24;i+=6) {  
     sprintf(txt,"%.2d",i);
    
     lon=i*360/24;
   
     x=sin(lon*PI/180.0);
     y=cos(lon*PI/180.0);
     
     px=(xoff+wdt/2)+x*rad;
     py=(yoff+hgt/2)+y*rad; 

     txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);

     ytxt=0;
     xtxt=0;
     
     if (i==0) ytxt=txbox[2];
     else if (i==12) ytxt=txbox[1];
     else if (i==18) xtxt=txbox[0]; 
    
     px=(xoff+wdt/2)+x*(rad+2+xtxt);
     py=(yoff+hgt/2)+y*(rad+2+ytxt); 

     if ((i % 12)==0) px=px-txbox[0]/2;
     else py=py+(txbox[2]-txbox[1])/2; 

     PlotText(plot,NULL,fontname,fontsize,px,py,strlen(txt),txt,
               color,mask,1);

  }
}















