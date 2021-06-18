/* XwinDisplayEvent.c
   ================== 
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
 
  int x=30,y=40;
  int vx=3,vy=4;

  struct timeval tmout;

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

  while(status !=1) {

    tmout.tv_sec=0;
    tmout.tv_usec=0.05*1e6;

    status=XwinDisplayEvent(dp,1,&win,1,&tmout);
    x=x+vx;
    y=y+vy;

    if (x>wdt) {
      x=wdt-1;
      vx=-vx;
    }

    if (y>hgt) {
      y=hgt-1;
      vy=-vy;
    }
    if (x<0) {
      x=0;
      vx=-vx;
    }
    if (y<0) {
      y=0;
      vy=-vy;
    }

    FrameBufferEllipse(fbuf,NULL,x,y,4,4,1,
                          0xffffff,0x0f,0,NULL,NULL);

   
    XwinFrameBufferWindow(fbuf,win);
   
  }
 
  XwinFreeWindow(win);

  XwinCloseDisplay(dp);

  return 0;
}





  
