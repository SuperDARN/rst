/* plot_logo.c
   =========== 
   Author: R.J.Barnes

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

#include "rfbuffer.h"
#include "iplot.h"
#include "rtime.h"
#include "text_box.h"

#define CREDIT "JHU/APL.Software by R.J.Barnes"

void plot_logo(struct Plot *plot, float xoff,float yoff,float wdt,float hgt,
               unsigned int color,unsigned char mask, char *fontname,
               float fontsize, void *txtdata)
{
  char txt[256];
  float txbox[3];
  
  sprintf(txt,"SuperDARN");
  txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);
 
  PlotText(plot,NULL,fontname,fontsize,xoff,yoff+hgt+txbox[1],
            strlen(txt),txt,color,mask,1);
}
  
void plot_aacgm(struct Plot *plot, float xoff,float yoff,float wdt,float hgt,
              unsigned int color,unsigned char mask, char *fontname,
              float fontsize, void *txtdata, int old)
{
  char txt[256];
  float txbox[3];
   
	if (old) sprintf(txt,"AACGM");
	else     sprintf(txt,"AACGM-v2");
  txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);
 
  PlotText(plot,NULL,fontname,fontsize,xoff+wdt-txbox[0],yoff+hgt+txbox[1],
            strlen(txt),txt,color,mask,1);
}
  
void plot_web(struct Plot *plot, float xoff,float yoff,float wdt,float hgt,
              unsigned int color,unsigned char mask, char *fontname,
              float fontsize, void *txtdata)
{
  char txt[256];
  float txbox[3];
   
  sprintf(txt,"http://superdarn.jhuapl.edu");
  txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);
 
  PlotText(plot,NULL,fontname,fontsize,xoff+wdt-txbox[0],yoff+hgt+txbox[1],
            strlen(txt),txt,color,mask,1);
}
  
void plot_credit(struct Plot *plot, float xoff,float yoff,float wdt,float hgt,
                 unsigned int color,unsigned char mask, char *fontname,
                 float fontsize, void *txtdata)
{
  char txt[256];
  float txbox[3];
 
  sprintf(txt,CREDIT);
  txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);
 
  PlotText(plot,NULL,fontname,fontsize,xoff,yoff+hgt+txbox[1],
            strlen(txt),txt,color,mask,1);
}
  
