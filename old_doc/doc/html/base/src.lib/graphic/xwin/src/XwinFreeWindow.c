/* XwinFreeWindow.c
   ================ 
   Author: R.J.Barnes
*/

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
  int i;

  float x,y,z,r,g,b;
  unsigned int cv;

  char *display_name=NULL;
 
  char *progname;
  int status=0;
 
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

  for (i=0;i<200;i++) {

    r=((float) rand())/RAND_MAX;
    g=((float) rand())/RAND_MAX;
    b=((float) rand())/RAND_MAX;
 
    x=((float) rand())/RAND_MAX;
    y=((float) rand())/RAND_MAX;
    z=((float) rand())/RAND_MAX;

    cv=FrameBufferColor(255*r,255*g,255*b,255);

    FrameBufferEllipse(fbuf,NULL,wdt*x,hgt*y,z*100,z*100,1,
                         cv,0x0f,0,NULL,NULL);
  }

  XwinFrameBufferWindow(fbuf,win);

  XwinShowWindow(win);
  
  status=XwinDisplayEvent(dp,1,&win,0,NULL);
  
  XwinFreeWindow(win);

  XwinCloseDisplay(dp);

  return 0;
}





  
