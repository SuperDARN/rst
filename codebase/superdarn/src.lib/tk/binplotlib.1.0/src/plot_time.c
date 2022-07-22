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
#include <ctype.h>

#include "rfbuffer.h"
#include "iplot.h"
#include "rtime.h"
#include "rmath.h"
#include "rmap.h"
#include "text_box.h"


void plot_time(struct Plot *plot,
               float xoff,float yoff,float wdt,float hgt,
               double time,
               unsigned int color,unsigned char mask,
               char *fontname,float fontsize,
               void *txtdata) {
  int i;
  char txt[256];
  float txbox[3];
  char *month[]={"Jan","Feb","Mar","Apr","May","Jun",
                 "Jul","Aug","Sep","Oct","Nov","Dec",0};
  char *tmeA="00:00:00 UT";
  char *tmeC="0";
  float cwdt;
  float x,y;
  int yr,mo,dy,hr,mt;
  double sec;

  txtbox(fontname,fontsize,strlen(tmeC),tmeC,txbox,txtdata);
  cwdt=txbox[0];

  TimeEpochToYMDHMS(time,&yr,&mo,&dy,&hr,&mt,&sec);

  sprintf(txt,"%.2d %s %d",dy,month[mo-1],yr);
  txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);
  x=xoff;
  y=yoff+txbox[2];
  for (i=0;txt[i] !=0;i++) {
    txtbox(fontname,fontsize,1,txt+i,txbox,txtdata);
    PlotText(plot,NULL,fontname,fontsize,x,
             y,1,txt+i,color,mask,1);
    if (isdigit(txt[i])) x+=cwdt;
    else x+=txbox[0];
  }

  sprintf(txt,"%.2d:%.2d UT",hr,mt);

  txtbox(fontname,fontsize,strlen(tmeA),tmeA,txbox,txtdata);

  x=xoff+wdt-txbox[0];
  y=yoff+txbox[2];
  for (i=0;txt[i] !=0;i++) {
    txtbox(fontname,fontsize,1,txt+i,txbox,txtdata);
    PlotText(plot,NULL,fontname,fontsize,x,
             y,1,txt+i,color,mask,1);
    if (isdigit(txt[i])) x+=cwdt;
    else x+=txbox[0];
  }
}


void plot_time_range(struct Plot *plot,
                     float xoff,float yoff,float wdt,float hgt,
                     double stime,double etime,
                     unsigned int color,unsigned char mask,
                     char *fontname,float fontsize,
                     void *txtdata) {
  int i;
  char txt[256];
  float txbox[3];
  char *month[]={"Jan","Feb","Mar","Apr","May","Jun",
                 "Jul","Aug","Sep","Oct","Nov","Dec",0};
  char *tmeA="00:00:00 UT";
  char *tmeB="00:00:00 - 00:00:00 UT";
  char *tmeC="0";
  float cwdt;
  float x,y;
  int yr,mo,dy,shr,smt,ssc,ehr,emt,esc;
  double sec;

  txtbox(fontname,fontsize,strlen(tmeC),tmeC,txbox,txtdata);
  cwdt=txbox[0];

  TimeEpochToYMDHMS(etime,&yr,&mo,&dy,&ehr,&emt,&sec);
  esc=sec;
  TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&shr,&smt,&sec);
  ssc=sec;

  sprintf(txt,"%.2d %s %d",dy,month[mo-1],yr);
  txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);
  x=xoff;
  y=yoff+txbox[2];
  for (i=0;txt[i] !=0;i++) {
    txtbox(fontname,fontsize,1,txt+i,txbox,txtdata);
    PlotText(plot,NULL,fontname,fontsize,x,
             y,1,txt+i,color,mask,1);
    if (isdigit(txt[i])) x+=cwdt;
    else x+=txbox[0];
  }

  if (stime==etime) sprintf(txt,"%.2d:%.2d:%.2d UT",shr,smt,ssc);
  else sprintf(txt,"%.2d:%.2d:%.2d - %.2d:%.2d:%.2d UT",
               shr,smt,ssc,ehr,emt,esc);

  if (stime==etime) txtbox(fontname,fontsize,strlen(tmeA),tmeA,txbox,txtdata);
  else txtbox(fontname,fontsize,strlen(tmeB),tmeB,txbox,txtdata);

  x=xoff+wdt-txbox[0];
  y=yoff+txbox[2];
  for (i=0;txt[i] !=0;i++) {
    txtbox(fontname,fontsize,1,txt+i,txbox,txtdata);
    PlotText(plot,NULL,fontname,fontsize,x,
             y,1,txt+i,color,mask,1);
    if (isdigit(txt[i])) x+=cwdt;
    else x+=txbox[0];
  }
}


void plot_field_time(struct Plot *plot,
                     float xoff,float yoff,float wdt,float hgt,int flg,
                     double stime,double etime,
                     unsigned int color,unsigned char mask,
                     char *fontname,float fontsize,
                     void *txtdata) {
  int i;
  char txt[256];
  float txbox[3];
  char *month[]={"Jan","Feb","Mar","Apr","May","Jun",
                 "Jul","Aug","Sep","Oct","Nov","Dec",0};
  char *tmeC="0";
  float cwdt;
  float x,y;
  int yr,mo,dy,shr,smt,ssc,ehr,emt,esc;
  double sec;

  txtbox(fontname,fontsize,strlen(tmeC),tmeC,txbox,txtdata);
  cwdt=txbox[0];

  TimeEpochToYMDHMS(etime,&yr,&mo,&dy,&ehr,&emt,&sec);
  esc=sec;
  TimeEpochToYMDHMS(stime,&yr,&mo,&dy,&shr,&smt,&sec);
  ssc=sec;

  if (flg==2) {
    sprintf(txt,"%.2d %s %d %.4d%.2d%.2d %.2d:%.2d:%.2d - %.2d:%.2d:%.2d UT",
            dy,month[mo-1],yr,yr,mo,dy,
            shr,smt,ssc,ehr,emt,esc);
    txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);
    x=xoff+wdt-txbox[0]*0.90;
    y=yoff;
    for (i=0;txt[i] !=0;i++) {
      txtbox(fontname,fontsize,1,txt+i,txbox,txtdata);
      PlotText(plot,NULL,fontname,fontsize,x,
               y,1,txt+i,color,mask,1);
      if (isdigit(txt[i])) x+=cwdt;
      else x+=txbox[0];
    }
    return;
  }

  if (flg==0) {
    sprintf(txt,"%.2d %s %d",dy,month[mo-1],yr);
    txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);
    x=xoff;
    y=yoff+txbox[2];
    for (i=0;txt[i] !=0;i++) {
      txtbox(fontname,fontsize,1,txt+i,txbox,txtdata);
      PlotText(plot,NULL,fontname,fontsize,x,
               y,1,txt+i,color,mask,1);
      if (isdigit(txt[i])) x+=cwdt;
      else x+=txbox[0];
    }
  }

  if (stime==etime) sprintf(txt,"%.2d:%.2d:%.2d0UT",shr,smt,ssc);
  else sprintf(txt,"%.2d:%.2d:%.2d0-0%.2d:%.2d:%.2d0UT",
               shr,smt,ssc,ehr,emt,esc);

  txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);

  if (stime==etime) sprintf(txt,"%.2d:%.2d:%.2d UT",shr,smt,ssc);
  else sprintf(txt,"%.2d:%.2d:%.2d - %.2d:%.2d:%.2d UT",
               shr,smt,ssc,ehr,emt,esc);

  if (txbox[0]<wdt) x=xoff+wdt-txbox[0]*0.90;
  else x=xoff;
  y=yoff+txbox[2];
  for (i=0;txt[i] !=0;i++) {
    txtbox(fontname,fontsize,1,txt+i,txbox,txtdata);
    PlotText(plot,NULL,fontname,fontsize,x,
             y,1,txt+i,color,mask,1);
    if (isdigit(txt[i])) x+=cwdt;
    else x+=txbox[0];
  }
}


void plot_vec_time(struct Plot *plot,
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

