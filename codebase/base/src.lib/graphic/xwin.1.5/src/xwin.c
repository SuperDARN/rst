/* xwin.c
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




int XwinCloseDisplay(struct XwinDisplay *ptr) {
  if (ptr==NULL) return -1;
  if (ptr->display !=NULL) XCloseDisplay(ptr->display);
  free(ptr);
  return 0;
}



int XwinFreeWindow(struct XwinWindow *ptr) { 
  if (ptr==NULL) return -1;
  if (ptr->size_hints !=NULL) XFree(ptr->size_hints);
  if (ptr->class_hints !=NULL) XFree(ptr->class_hints);
  if (ptr->wm_hints !=NULL) XFree(ptr->wm_hints);
  

  if (ptr->windowName.value !=NULL) XFree(ptr->windowName.value);
  if (ptr->windowName.value !=NULL) XFree(ptr->iconName.value);

  if (ptr->xi !=NULL) XDestroyImage(ptr->xi);
  if (ptr->gc !=NULL) XFreeGC((ptr->dp)->display,ptr->gc);

  if (ptr->win !=0) XDestroyWindow((ptr->dp)->display,ptr->win);
  free(ptr);
  return 0;
}

struct XwinDisplay *XwinOpenDisplay(char *display_name,int *flg) {

  struct XwinDisplay *ptr;
 
  *flg=0;

  ptr=malloc(sizeof(struct XwinDisplay));
  if (ptr==NULL) return NULL;

  memset(ptr,0,sizeof(struct XwinDisplay));   
 
  if ((ptr->display=XOpenDisplay(display_name))==NULL) {
     XwinCloseDisplay(ptr);
     *flg=1;
     return NULL;
  }

  ptr->screen_num=DefaultScreen(ptr->display);
  ptr->default_visual=DefaultVisual(ptr->display,ptr->screen_num);
  ptr->default_depth=DefaultDepth(ptr->display,ptr->screen_num);
   
  if (ptr->default_depth<24) { 

   /* we only support true color visuals for now */
   XwinCloseDisplay(ptr);
   *flg=2;
   return NULL;
  }

  ptr->wdt=DisplayWidth(ptr->display,ptr->screen_num);
  ptr->hgt=DisplayHeight(ptr->display,ptr->screen_num);

  return ptr;
}


struct XwinWindow *XwinMakeWindow(int x,int y,int wdt,int hgt,int border,
                           struct XwinDisplay *display,
                           char *window_name,char *icon_name,
                           char *res_name,char *res_class,int argc,
                           char *argv[],
                           int *flg) {
  XGCValues values;
  unsigned long valuemask=0;
 
  struct XwinWindow *ptr;
  int status=0;  

  *flg=status;
  ptr=malloc(sizeof(struct XwinWindow));
  if (ptr==NULL) return NULL;

  memset(ptr,0,sizeof(struct XwinWindow));   

  ptr->dp=display;

  ptr->wdt=wdt;
  ptr->hgt=hgt;

  if ((ptr->size_hints=XAllocSizeHints())==NULL) status=1;
  if ((ptr->wm_hints=XAllocWMHints())==NULL) status=1;
  if ((ptr->class_hints=XAllocClassHint())==NULL) status=1;
  if (XStringListToTextProperty(&window_name,1,&ptr->windowName)==0) status=1; 
  if (XStringListToTextProperty(&icon_name,1,&ptr->iconName)==0) status=1;
 
  if (status !=0) {
     *flg=status;
     XwinFreeWindow(ptr);
     return NULL;
  }


  /* create the window */

  ptr->win=XCreateSimpleWindow(display->display,
                               RootWindow(display->display,
                               display->screen_num),
			       x,y,wdt,hgt,border,
			       BlackPixel(display->display,
                                          display->screen_num),
                               WhitePixel(display->display,
                                          display->screen_num));
 
  (ptr->size_hints)->flags=PPosition | PSize | PMinSize | PMaxSize;
  (ptr->size_hints)->min_width=wdt;
  (ptr->size_hints)->min_height=hgt;
  (ptr->size_hints)->max_width=wdt;
  (ptr->size_hints)->max_height=hgt;
 
  (ptr->wm_hints)->flags=StateHint | InputHint;
  (ptr->wm_hints)->initial_state=NormalState;
  (ptr->wm_hints)->input=True;
 
  (ptr->class_hints)->res_name=res_name;
  (ptr->class_hints)->res_class=res_class;
  
  XSetWMProperties(display->display,ptr->win,&ptr->windowName,
                   &ptr->iconName,argv,argc,
                   ptr->size_hints,ptr->wm_hints,ptr->class_hints);

  
  ptr->buffer=malloc(4*wdt*hgt);

  if (ptr->buffer==NULL) {
    *flg=5;
    XwinFreeWindow(ptr);
    return NULL;
  }

  ptr->xi=XCreateImage(display->display,display->default_visual,24,ZPixmap,0,
                       ptr->buffer,wdt,hgt,8,0);

  if (ptr->xi==NULL) {
    *flg=5;
    XwinFreeWindow(ptr);
    return NULL;
  }

  ptr->gc=XCreateGC(display->display,ptr->win,valuemask,&values);

  if (ptr->gc==NULL) {
    *flg=6;
    XwinFreeWindow(ptr);
    return NULL;
  }


  /* Trap the close window requests from the Window Manager */

  ptr->atom.proto = XInternAtom(display->display, "WM_PROTOCOLS", False);
  ptr->atom.delete = XInternAtom(display->display, "WM_DELETE_WINDOW", False);

  if ((ptr->atom.proto != None) && (ptr->atom.delete != None))
    XChangeProperty(display->display, ptr->win, ptr->atom.proto, XA_ATOM, 32,
		    PropModeReplace, (unsigned char *)&ptr->atom.delete, 1);

  return ptr;

}
 
int XwinFrameBufferToXimage(struct FrameBuffer *ptr,XImage *xi) {

  /* This should be done properly by finding out the visual depth.
   * However for now we assume that we are running on a TrueColor 
   * display. 
   *
   */
  
 int x,y,o;
 char *a;

 if (ptr==NULL) return -1;
 if (xi==NULL) return -1;
  
 o=ptr->wdt*ptr->hgt;
 for (x=0;x<ptr->wdt;x++) {
   for (y=0;y<ptr->hgt;y++) {
     a=xi->data+y*xi->bytes_per_line+(x*xi->bits_per_pixel/8)+xi->xoffset;
     if (xi->bitmap_bit_order==LSBFirst) {
       a[2]=ptr->img[y*ptr->wdt+x];
       if (ptr->depth !=8) {
         a[1]=ptr->img[o+y*ptr->wdt+x];
         a[0]=ptr->img[2*o+y*ptr->wdt+x];
       } else {
         a[1]=a[2];
         a[0]=a[2];
       }
     } else {
       a[1]=ptr->img[y*ptr->wdt+x];
       if (ptr->depth !=8) {
         a[2]=ptr->img[o+y*ptr->wdt+x];
         a[3]=ptr->img[2*o+y*ptr->wdt+x];
       } else {
         a[2]=a[2];
         a[3]=a[2];
       }
     }

   }
 } 
 return 0;

}


int XwinFrameBufferWindow(struct FrameBuffer *ptr,struct XwinWindow *win) {
  int s=0;
  s=XwinFrameBufferToXimage(ptr,win->xi);
  if (s !=0) return s;
  XPutImage((win->dp)->display,win->win,win->gc,win->xi,0,0,0,0,win->wdt,
            win->hgt);
  return 0;
}


int XwinShowWindow(struct XwinWindow *win) {
  XSelectInput((win->dp)->display,win->win,
               ButtonPressMask | ExposureMask | StructureNotifyMask);
  XMapWindow((win->dp)->display,win->win);
  return 0;
}

int XwinDisplayEvent(struct XwinDisplay *display,int wnum,
                   struct XwinWindow **win,int mode,struct timeval *tmout) {

  /* Displays a graphics image on the screen until the user
   * hits the exit button.
   */
  int status=0;

  fd_set read_set;
  
  int loop=1;
  int i;
  union {
    XEvent              event;
    XAnyEvent           any;
    XButtonEvent        button;
    XKeyEvent           key;
    XConfigureEvent     configure;
    XExposeEvent        expose;
    XMotionEvent        motion;
    XResizeRequestEvent resize;
    XClientMessageEvent message;
  } event;

  

  FD_ZERO(&read_set);
  FD_SET(ConnectionNumber(display->display), &read_set);

  
  while (loop) {
   status=0;
   while ((XPending(display->display)) && (status==0)) {
 
      /* eat all of the pending events */
      XNextEvent(display->display,&event.event);

      switch (event.event.type) {
      case Expose:
      
        for (i=0;(i<wnum) && (event.expose.window !=win[i]->win);i++);

        if (i<wnum) XPutImage(display->display,win[i]->win,win[i]->gc,
                         win[i]->xi,event.expose.x, event.expose.y,
                         event.expose.x,event.expose.y,
                         event.expose.width, event.expose.height);
        break;
      case ButtonPress:
        if (mode !=0) status=2; /* leave event loop on button press */
        break;
      case ConfigureNotify:
        break;
      case ClientMessage:
          for (i=0;(i<wnum) && (event.message.window !=win[i]->win);i++);
          if ((i<wnum) &&
            (event.message.data.l[0]==win[i]->atom.delete)) {
            loop=0;
            status=1;
	  }
	  break;

      default:
          break;
      }
   }
   if (status !=0) break;
   
    status=select(ConnectionNumber(display->display)+1,&read_set,0,0,
                  tmout); 
    if (status==0) loop=0;
  }   
      
  return status;

}



  
