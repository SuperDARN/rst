/* plot_logo.c
   =========== 
   Author: R.J.Barnes
*/


/*
   See license.txt
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
  
