/* plot_time.c
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
#include <ctype.h>

#include "rfbuffer.h"
#include "iplot.h"
#include "rtime.h"
#include "text_box.h"



void plot_time(struct Plot *plot,
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
  
