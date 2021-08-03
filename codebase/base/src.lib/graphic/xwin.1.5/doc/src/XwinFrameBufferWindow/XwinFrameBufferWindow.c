/* XwinFrameBufferWindow.c
   ======================= 
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



#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#include "rfbuffer.h"
#include "xwin.h"


struct FrameBuffer *fbuf=NULL;

int main(int argc,char *argv[]) {

  struct XwinDisplay *dp;
  struct XwinWindow *win;

  int wdt=300,hgt=300;
  unsigned int border=0;
  int flg;


  char *display_name=NULL;
 
  char *progname;
  int status=0;

  struct timeval tmout;

  unsigned int cv;
  unsigned char *array=NULL;
  int x,y,i,imax;
  float ix,iy,cx,cy,tx,xmin,xmax,ymin,ymax;

  xmin=-2.25;
  xmax=-2.25+3;
  ymin=-1.5;
  ymax=1.5;

  imax=255;

  array=malloc(hgt);
 
  progname=argv[0];

  dp=XwinOpenDisplay(display_name,&flg);
  win=XwinMakeWindow(0,0,wdt,hgt,border,
                     dp,"Test Window",
                    "Test Window","Test Window",
                    "basic",argc,argv,&flg);
                           

  if ((fbuf=FrameBufferMake("Test",wdt,hgt,24))==NULL) {
     fprintf(stderr,"failed to make frame buffer.");
     exit(-1);
  }


  XwinFrameBufferWindow(fbuf,win);

  XwinShowWindow(win);
  

  for (x=0;x<wdt;x++) {
    for (y=0;y<hgt;y++) {
       cx=xmin+(xmax-xmin)*x/wdt;
       cy=ymin+(ymax-ymin)*y/hgt;
       ix=0;
       iy=0;
       for (i=0;i<imax;i++) {
         tx=ix*ix-iy*iy+cx;
         iy=2*ix*iy+cy;
         ix=tx;
         if (sqrt(ix*ix+iy*iy)>2.0) break;
       }
       array[y]=i;
    }

    for (y=0;y<hgt;y++) {
      cv=FrameBufferColor(16*array[y],16*array[y],255-16*array[y],255);
      FrameBufferRectangle(fbuf,NULL,x,y,1,1,1,cv,0x0f,0,NULL,NULL);
      
    }

    XwinFrameBufferWindow(fbuf,win);
    tmout.tv_sec=0;
    tmout.tv_usec=0;
    status=XwinDisplayEvent(dp,1,&win,0,&tmout);
    if (status !=0) break;
  }
  XwinFreeWindow(win);

  XwinCloseDisplay(dp);

  return 0;
}





  
