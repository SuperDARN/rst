/* make.c
   ====== 
   Author:R.J.Barnes
*/


/*
   See license.txt
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rfbuffer.h"
#include "fontdb.h"
#include "rps.h"
#include "iplot.h"
#include "splot.h"


int SplotFrameBufferDash(struct Splot *ptr,struct PlotDash *dash) {

  int *p=NULL;
  int i;  

  if (ptr==NULL) return -1;
  if (dash==NULL) {
    if (ptr->fb.dash !=NULL) FrameBufferFreeDash(ptr->fb.dash);
    ptr->fb.dash=NULL;
    return 0;
  }

  p=malloc(sizeof(int)*dash->sze);
  if (p==NULL) return -1;
  for (i=0;i<dash->sze;i++) p[i]=dash->p[i];


  if ((ptr->fb.dash !=NULL) &&
      ((ptr->fb.dash)->sze==dash->sze) &&
      ((ptr->fb.dash)->ph==dash->phase) &&
      (memcmp((ptr->fb.dash)->p,p,dash->sze)==0)) return 0;

  if (ptr->fb.dash !=NULL) FrameBufferFreeDash(ptr->fb.dash);
  ptr->fb.dash=FrameBufferMakeDash(p,dash->phase,dash->sze);
  free(p);
  if (ptr->fb.dash !=NULL) return 0;
  return -1;
}


int SplotPostScriptDash(struct Splot *ptr,struct PlotDash *dash) {

  if (ptr==NULL) return -1;
  if (dash==NULL) {
    if (ptr->ps.dash !=NULL) PostScriptFreeDash(ptr->ps.dash);
    ptr->ps.dash=NULL;
    return 0;
  }

  if ((ptr->ps.dash !=NULL) && 
      ((ptr->ps.dash)->sze==dash->sze) && 
      ((ptr->ps.dash)->phase==dash->phase) && 
      (memcmp((ptr->ps.dash)->p,dash->p,dash->sze)==0)) return 0;

  if (ptr->ps.dash !=NULL) PostScriptFreeDash(ptr->ps.dash);
  ptr->ps.dash=PostScriptMakeDash(dash->p,dash->phase,dash->sze);
  if (ptr->ps.dash !=NULL) return 0;
  return -1;  
}

unsigned int SplotPostScriptColor(unsigned int color) {
  unsigned char r,g,b,a;
  float ta,ra,ga,ba;

  a=color>>24;;
  r=color>>16;
  g=color>>8;
  b=color;

  ta=a/255.0;
  ra=(1-ta)*255+ta*r;
  ga=(1-ta)*255+ta*g;
  ba=(1-ta)*255+ta*b;

  r=ra;
  g=ga;
  b=ba;

  return (r<<16) | (g<<8) | b;
}


int SplotSetPlot(struct Plot *plot,struct Splot *ptr) {
  if (plot==NULL) return -1;
  if (ptr==NULL) return -1;

  plot->plot.start.func=(void *) SplotPlotStart;
  plot->plot.start.data=ptr;  
  plot->plot.end.func=(void *) SplotPlotEnd;
  plot->plot.end.data=ptr;  

  plot->document.start.func=(void *) SplotDocumentStart;
  plot->document.start.data=ptr;  
  plot->document.end.func=(void *) SplotDocumentEnd;
  plot->document.end.data=ptr;  

  plot->clip.func=(void *) SplotClip;
  plot->clip.data=ptr;  

  plot->line.func=(void *) SplotLine;
  plot->line.data=ptr;  

  plot->bezier.func=(void *) SplotBezier;
  plot->bezier.data=ptr;  


  plot->rectangle.func=(void *) SplotRectangle;
  plot->rectangle.data=ptr;  

  plot->ellipse.func=(void *) SplotEllipse;
  plot->ellipse.data=ptr;  

  plot->polygon.func=(void *) SplotPolygon;
  plot->polygon.data=ptr;  

  plot->text.func=(void *) SplotText;
  plot->text.data=ptr;  

  plot->image.func=(void *) SplotImage;
  plot->image.data=ptr;  

  return 0;
}

int SplotSetFrameBuffer(struct Splot *ptr,struct FrameBuffer **img,
                        struct FrameBufferFontDB *fontdb,
                        int (*trigger)(struct FrameBuffer *,void *),void *data) {


  if (ptr==NULL) return -1;
  ptr->fb.ptr=img;
  ptr->fb.fontdb=fontdb;
  ptr->fb.trigger=trigger;
  ptr->fb.data=data;
  return 0;
}

int SplotSetPostScript(struct Splot *ptr,struct PostScript *ps,
                       int land,float xpoff,float ypoff) {
  if (ptr==NULL) return -1;
  ptr->ps.ptr=ps;
  ptr->ps.land=land;
  ptr->ps.xpoff=xpoff;
  ptr->ps.ypoff=ypoff;
  return 0;
}


struct Splot *SplotMake() {
  struct Splot *ptr;
  ptr=malloc(sizeof(struct Splot));
  if (ptr==NULL) return ptr;
  memset(ptr,0,sizeof(struct Splot));
  return ptr;
}

void SplotFree(struct Splot *ptr) {
  if (ptr==NULL) return;
  free(ptr);
};



