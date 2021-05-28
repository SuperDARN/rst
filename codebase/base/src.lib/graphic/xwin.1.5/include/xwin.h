/* xwin.h
   ====== 
   Author: R.J.Barnes
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
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
 

