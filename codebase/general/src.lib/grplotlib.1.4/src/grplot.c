/* rplotg.c
   ========
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
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "rfbuffer.h"
#include "iplot.h"
#include "polygon.h"
#include "grplotraw.h"
#include "grplotstd.h"
#include "grplot.h"

struct Grplot *GrplotMake(float wdt,float hgt,
                          int xnum,int ynum,float lpad,float rpad,
                          float bpad,float tpad,float xoff,float yoff) {
  struct Grplot *plt;

  plt=malloc(sizeof(struct Grplot));
  if (plt==NULL) return NULL;
 
  plt->plot=NULL;
  plt->xnum=xnum;
  plt->ynum=ynum;
  plt->xoff=xoff;
  plt->yoff=yoff;
  plt->tpad=tpad;
  plt->bpad=bpad;
  plt->lpad=lpad;
  plt->rpad=rpad;

  plt->major_wdt=5;
  plt->minor_wdt=3;
  plt->major_hgt=5;
  plt->minor_hgt=3;

  plt->ttl_wdt=12;
  plt->ttl_hgt=12;
  plt->ttl_xor=0;
  plt->ttl_yor=1; 

  plt->lbl_wdt=4;
  plt->lbl_hgt=4; 

  plt->lbl_xor=0;
  plt->lbl_yor=0; 

  plt->box_hgt=hgt/plt->ynum-plt->tpad-plt->bpad;
  plt->box_wdt=wdt/plt->xnum-plt->lpad-plt->rpad;
  plt->xoff+=plt->lpad;
  plt->yoff+=plt->tpad;

  plt->text.box=NULL;
  plt->text.data=NULL;

  return plt;
}


void GrplotFree(struct Grplot *ptr) {
  if (ptr==NULL) return;
  free(ptr);
}


int GrplotSetPlot(struct Grplot *plt,struct Plot *rplot) {
  if (plt==NULL) return -1;
  plt->plot=rplot;
  return 0;
}

int GrplotSetTextBox(struct Grplot *plt,
                   int  (*text)(char *,float,int,char *,float *,void *),
                   void *data) {
  if (plt==NULL) return -1;
  plt->text.box=text;
  plt->text.data=data;
  return 0;
}


int GrplotPanel(struct Grplot *plt,int num,unsigned int color,
              unsigned char mask,float width) {

  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return GrplotRawPanel(plt->plot,ox,oy,plt->box_wdt,plt->box_hgt,
                      color,mask,width);

}

int GrplotClipPanel(struct Grplot *plt,int num) {

  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;

  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);
  return GrplotRawClipPanel(plt->plot,ox,oy,plt->box_wdt,plt->box_hgt);

}




int GrplotXaxis(struct Grplot *plt,int num,
              double xmin,double xmax,
              double tick_major,double tick_minor,
	      int tick_flg,
	      unsigned int color,unsigned char mask,float width) {

  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return GrplotRawXaxis(plt->plot,ox,oy,plt->box_wdt,plt->box_hgt,
		      xmin,xmax,tick_major,tick_minor,
                      plt->major_hgt,plt->minor_hgt,
                      tick_flg,color,mask,width);

  

}
  
int GrplotYaxis(struct Grplot *plt,int num,
	      double ymin,double ymax,
              double tick_major,double tick_minor,
	      int tick_flg,
	      unsigned int color,unsigned char mask,float width) {
  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return GrplotRawYaxis(plt->plot,ox,oy,plt->box_wdt,plt->box_hgt,
			 ymin,ymax,tick_major,tick_minor,
                         plt->major_wdt,plt->minor_wdt,
                         tick_flg,color,mask,width); 

}

int GrplotXaxisLog(struct Grplot *plt,int num,
		 double xmin,double xmax,
                 double tick_major,double tick_minor,
	         int tick_flg,
                 unsigned int color,unsigned char mask,float width) {

  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return GrplotRawXaxisLog(plt->plot,ox,oy,plt->box_wdt,plt->box_hgt,
			 xmin,xmax,tick_major,tick_minor,
                         plt->major_hgt,plt->minor_hgt,
                         tick_flg,color,mask,width);

}

int GrplotYaxisLog(struct Grplot *plt,int num,
		 double ymin,double ymax,
                 double tick_major,double tick_minor,
	         int tick_flg,
		 unsigned int color,unsigned char mask,float width) {

  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return GrplotRawYaxisLog(plt->plot,ox,oy,plt->box_wdt,plt->box_hgt,
			 ymin,ymax,tick_major,tick_minor,
                         plt->major_wdt,plt->minor_wdt,
                         tick_flg,color,mask,width);

}



int GrplotXaxisValue(struct Grplot *plt,int num,
	           double xmin,double xmax,double value,
	           float tick_hgt,int tick_flg,
		   unsigned int color,unsigned char mask,float width) {
  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return GrplotRawXaxisValue(plt->plot,ox,oy,plt->box_wdt,plt->box_hgt,
			   xmin,xmax,value,
                           tick_hgt,tick_flg,color,mask,width);

}
 
int GrplotYaxisValue(struct Grplot *plt,int num,
		   double ymin,double ymax,double value,
	           float tick_hgt,int tick_flg,
		   unsigned int color,unsigned char mask,float width) {

  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return GrplotRawYaxisValue(plt->plot,ox,oy,plt->box_wdt,plt->box_hgt,
				ymin,ymax,value,
                                tick_hgt,tick_flg,color,mask,width);

}

int GrplotXzero(struct Grplot *plt,int num,
	      double xmin,double xmax,
              unsigned int color,unsigned char mask,float width,
	      struct PlotDash *dash) {
  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return GrplotRawXzero(plt->plot,ox,oy,plt->box_wdt,plt->box_hgt,
                      xmin,xmax,color,mask,width,
                      dash);


}

int GrplotYzero(struct Grplot *plt,int num,
	      double ymin,double ymax,
              unsigned int color,unsigned char mask,float width,
	      struct PlotDash *dash) {
  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return GrplotRawYzero(plt->plot,ox,oy,plt->box_wdt,plt->box_hgt,
                      ymin,ymax,color,mask,width,
                      dash);

}

int GrplotXaxisLabel(struct Grplot *plt,int num,
	           double xmin,double xmax,double step,
		   int txt_flg,
                   char * (*make_text)(double,double,double,void *),
                   void *textdata,
                   char *fntname,float fntsze,
		   unsigned int color,unsigned char mask) {
  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return GrplotRawXaxisLabel(plt->plot,ox,oy,plt->box_wdt,plt->box_hgt,
                                xmin,xmax,step,
                                plt->lbl_hgt,txt_flg,plt->lbl_xor,
                                plt->text.box,
                                plt->text.data,make_text,textdata,
                                fntname,fntsze,
                                color,mask);

}

int GrplotYaxisLabel(struct Grplot *plt,int num,
	           double ymin,double ymax,double step,
	           int txt_flg,
                   char * (*make_text)(double,double,double,void *),
                   void *textdata,
                   char *fntname,float fntsze,
		   unsigned int color,unsigned char mask) {

  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return GrplotRawYaxisLabel(plt->plot,ox,oy,plt->box_wdt,plt->box_hgt,
                                ymin,ymax,step,
                                plt->lbl_wdt,txt_flg,plt->lbl_yor,
                                plt->text.box,
                                plt->text.data,make_text,textdata,
                                fntname,fntsze,
                                color,mask);

}

int GrplotXaxisLabelValue(struct Grplot *plt,int num,
	                double xmin,double xmax,double value,
			int txt_flg,
                        int sze,char *text,
                        char *fntname,float fntsze,
		        unsigned int color,unsigned char mask) {
  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return GrplotRawXaxisLabelValue(plt->plot,ox,oy,plt->box_wdt,plt->box_hgt,
				xmin,xmax,value,
                                plt->lbl_hgt,txt_flg,plt->lbl_xor,
                                plt->text.box,
                                plt->text.data,sze,text,fntname,fntsze,
                                color,mask);

}

int GrplotYaxisLabelValue(struct Grplot *plt,int num,
	                double ymin,double ymax,double value,
		        int txt_flg,
                        int sze,char *text,
                        char *fntname,float fntsze,
		        unsigned int color,unsigned char mask) {
  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return GrplotRawYaxisLabelValue(plt->plot,ox,oy,plt->box_wdt,plt->box_hgt,
                                ymin,ymax,value,
                                plt->lbl_wdt,txt_flg,plt->lbl_yor,
                                plt->text.box,
                                plt->text.data,sze,text,fntname,fntsze,
                                color,mask);

}

int GrplotXaxisTitle(struct Grplot *plt,int num,
		   int txt_flg,
                   int sze,char *text,
                   char *fntname,float fntsze,
                   unsigned int color,unsigned char mask) {
  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return GrplotRawXaxisTitle(plt->plot,ox,oy,plt->box_wdt,plt->box_hgt,
                           plt->ttl_hgt,txt_flg,plt->ttl_xor,plt->text.box,
                           plt->text.data,sze,text,fntname,fntsze,
                           color,mask);

}


int GrplotXaxisTitleOffset(struct Grplot *plt,int num,
		   int txt_flg,
		   int sze,char *text,float offset,
                   char *fntname,float fntsze,
                   unsigned int color,unsigned char mask) {
  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad)+offset;
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return GrplotRawXaxisTitle(plt->plot,ox,oy,plt->box_wdt,plt->box_hgt,
                           plt->ttl_hgt,txt_flg,plt->ttl_xor,plt->text.box,
                           plt->text.data,sze,text,fntname,fntsze,
                           color,mask);

}


int GrplotYaxisTitle(struct Grplot *plt,int num,
		   int txt_flg,
                   int sze,char *text,
                   char *fntname,float fntsze,
                   unsigned int color,unsigned char mask) {

  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return GrplotRawYaxisTitle(plt->plot,ox,oy,plt->box_wdt,plt->box_hgt,
                           plt->ttl_wdt,txt_flg,plt->ttl_yor,plt->text.box,
                           plt->text.data,sze,text,fntname,fntsze,
                           color,mask);

}


int GrplotYaxisTitleOffset(struct Grplot *plt,int num,
		   int txt_flg,
                   int sze,char *text,float offset,
                   char *fntname,float fntsze,
                   unsigned int color,unsigned char mask) {

  int xnm,ynm;
  float ox,oy;

  if (plt==NULL) return -1;
 
  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad)+offset;

  return GrplotRawYaxisTitle(plt->plot,ox,oy,plt->box_wdt,plt->box_hgt,
                           plt->ttl_wdt,txt_flg,plt->ttl_yor,plt->text.box,
                           plt->text.data,sze,text,fntname,fntsze,
                           color,mask);

}


int GrplotFitImage(struct Grplot *plt,int num,
                 struct FrameBuffer *img,
		 unsigned char mask) {

  int xnm,ynm;
  float ox,oy;

  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);
  return GrplotRawFitImage(plt->plot,ox,oy,plt->box_wdt,plt->box_hgt,
                      img,mask); 

}

int GrplotFitImageName(struct Grplot *plt,int num,
		  char *name,int iwdt,int ihgt,int depth,
                  unsigned char mask) {

  int xnm,ynm;
  float ox,oy;

  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);
  return GrplotRawFitImageName(plt->plot,ox,oy,plt->box_wdt,plt->box_hgt,
			     name,iwdt,ihgt,depth,mask); 

}

int GrplotLine(struct Grplot *plt,int num,
             double ax,double ay,double bx,double by,
	     double xmin,double xmax,
             double ymin,double ymax,
             unsigned int color,unsigned char mask,float width,
             struct PlotDash *dash) {

  int xnm,ynm;
  float ox,oy;

  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return GrplotRawLine(plt->plot,ox,oy,plt->box_wdt,plt->box_hgt,
                     ax,ay,bx,by,xmin,xmax,ymin,ymax,color,mask,width,
                     dash);
}

int GrplotRectangle(struct Grplot *plt,int num,
		  struct PlotMatrix *matrix,
                  double x,double y,double w,double h,
	          double xmin,double xmax,
                  double ymin,double ymax,
                  int fill,unsigned int color,unsigned char mask,float width,
                  struct PlotDash *dash) {

  int xnm,ynm;
  float ox,oy;

  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return GrplotRawRectangle(plt->plot,matrix,ox,oy,plt->box_wdt,plt->box_hgt,
                     x,y,w,h,xmin,xmax,ymin,ymax,fill,color,mask,width,
                     dash);
}




int GrplotEllipse(struct Grplot *plt,int num,
		  struct PlotMatrix *matrix,
                  double x,double y,double w,double h,
	          double xmin,double xmax,
                  double ymin,double ymax,
                  int fill,unsigned int color,unsigned char mask,float width,
                  struct PlotDash *dash) {

  int xnm,ynm;
  float ox,oy;

  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return GrplotRawEllipse(plt->plot,matrix,ox,oy,plt->box_wdt,plt->box_hgt,
                     x,y,w,h,xmin,xmax,ymin,ymax,fill,color,mask,width,
                     dash);
}


int GrplotPolygon(struct Grplot *plt,int num,
		struct PlotMatrix *matrix,
		double x,double y,int n,double *px,double *py,int *t,
	        double xmin,double xmax,
                double ymin,double ymax,
                int fill,unsigned int color,unsigned char mask,float width,
                struct PlotDash *dash) {

  int xnm,ynm;
  float ox,oy;

  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return GrplotRawPolygon(plt->plot,matrix,ox,oy,plt->box_wdt,plt->box_hgt,
                       x,y,n,px,py,t,
                       xmin,xmax,ymin,ymax,fill,color,mask,width,
                       dash);
}


int GrplotText(struct Grplot *plt,int num,
	     struct PlotMatrix *matrix,char *fontname,
             float fontsize,double x,double y,
             int n,char *txt,float dx,float dy,
	     double xmin,double xmax,
             double ymin,double ymax,
             unsigned int color,unsigned char mask) {

  int xnm,ynm;
  float ox,oy;

  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return GrplotRawText(plt->plot,matrix,ox,oy,plt->box_wdt,plt->box_hgt,
                     fontname,fontsize,x,y,n,txt,dx,dy,
                     xmin,xmax,ymin,ymax,color,mask);
}


int GrplotImage(struct Grplot *plt,int num,
	     struct PlotMatrix *matrix,
	      struct FrameBuffer *img,float dx,float dy,
             double x,double y,
	     double xmin,double xmax,
             double ymin,double ymax,
             unsigned char mask) {

  int xnm,ynm;
  float ox,oy;

  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return GrplotRawImage(plt->plot,matrix,ox,oy,plt->box_wdt,plt->box_hgt,
                     img,dx,dy,x,y,
                     xmin,xmax,ymin,ymax,mask);
}



int GrplotImageName(struct Grplot *plt,int num,
	          struct PlotMatrix *matrix,
                  char *name,int iwdt,int ihgt,int depth,
                  float dx,float dy,
                  double x,double y,
	          double xmin,double xmax,
                  double ymin,double ymax,
                  unsigned char mask) {

  int xnm,ynm;
  float ox,oy;

  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return GrplotRawImageName(plt->plot,matrix,ox,oy,plt->box_wdt,plt->box_hgt,
                          name,iwdt,ihgt,depth,dx,dy,x,y,
                          xmin,xmax,ymin,ymax,mask);
}






int GrplotBezier(struct Grplot *plt,int num,
             double ax,double ay,double bx,double by,
	     double cx,double cy,double dx,double dy,
             double xmin,double xmax,
             double ymin,double ymax,
             unsigned int color,unsigned char mask,float width,
             struct PlotDash *dash) {

  int xnm,ynm;
  float ox,oy;

  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return GrplotRawBezier(plt->plot,ox,oy,plt->box_wdt,plt->box_hgt,
                     ax,ay,bx,by,cx,cy,dx,dy,xmin,xmax,ymin,ymax,
                     color,mask,width,
                     dash);
}





int GrplotFill(struct Grplot *plt,int num,
             double ax,double ay,double bx,double by,
	     double xmin,double xmax,
             double ymin,double ymax,int or,
	     unsigned int color,unsigned char mask) {

  int xnm,ynm;
  float ox,oy;

  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return GrplotRawFill(plt->plot,ox,oy,plt->box_wdt,plt->box_hgt,
                     ax,ay,bx,by,xmin,xmax,ymin,ymax,or,color,mask);


}


int GrplotBar(struct Grplot *plt,int num,
	    double ax,double ay,double w,double o,
	    double xmin,double xmax,
	    double ymin,double ymax,int or,int fill,
	    unsigned int color,unsigned char mask,float width,
            struct PlotDash *dash) {

  int xnm,ynm;
  float ox,oy;

  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return GrplotRawBar(plt->plot,ox,oy,plt->box_wdt,plt->box_hgt,
                    ax,ay,w,o,xmin,xmax,ymin,ymax,or,fill,color,mask,
                    width,dash);


}


int GrplotGetPoint(struct Grplot *plt,int num,
             double x,double y,
	     double xmin,double xmax,
             double ymin,double ymax,float *px,float *py) {

  int xnm,ynm;
  float ox,oy;

  xnm=num % plt->xnum;
  ynm=num / plt->xnum;

  ox=plt->xoff+xnm*(plt->box_wdt+plt->lpad+plt->rpad);
  oy=plt->yoff+ynm*(plt->box_hgt+plt->tpad+plt->bpad);

  return GrplotRawGetPoint(ox,oy,plt->box_wdt,plt->box_hgt,
                         x,y,xmin,xmax,ymin,ymax,px,py);
}
