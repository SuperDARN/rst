/* xwin.h
   ====== 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/




#ifndef _XWIN_H
#define _XWIN_H

struct XwinDisplay {
  Display *display;
  int screen_num;
  Visual *default_visual;
  int default_depth; 
  int wdt,hgt;
}; 


struct XwinWindow {
  int wdt,hgt;
  char *progname;    
  struct XwinDisplay *dp;
  Window win;
  XTextProperty windowName,iconName;
  XSizeHints *size_hints;
  XWMHints *wm_hints;
  XClassHint *class_hints;

  char *buffer;
  XImage *xi;
  GC gc;

  struct  {
    Atom proto;
    Atom delete;
  } atom;

};

int XwinCloseDisplay(struct XwinDisplay *ptr);
int XwinFreeWindow(struct XwinWindow *ptr);
struct XwinDisplay *XwinOpenDisplay(char *display_name,int *flg);
struct XwinWindow *XwinMakeWindow(int x,int y,int wdt,int hgt,int border,
                           struct XwinDisplay *display,
                           char *window_name,char *icon_name,
                           char *res_name,char *res_class,int argc,
                           char *argv[],
                           int *flg); 

int XwinFrameBufferToXimage(struct FrameBuffer *ptr,XImage *xi);
int XwinFrameBufferWindow(struct FrameBuffer *ptr,struct XwinWindow *win);

int XwinShowWindow(struct XwinWindow *win);
int XwinDisplayEvent(struct XwinDisplay *display,int wnum,
                   struct XwinWindow **win,int mode,struct timeval *tmout);

#endif
 

