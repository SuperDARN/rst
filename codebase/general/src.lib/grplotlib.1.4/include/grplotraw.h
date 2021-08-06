/* grplotraw.h
   ===========
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



#ifndef _GRPLOTRAW_H
#define _GRPLOTRAW_H


int GrplotRawGetPoint(float ox,float oy,float wdt,float hgt,
	            double x,double y,double xmin,double xmax,double ymin,
	            double ymax,float *px,float *py);

int GrplotRawPanel(struct Plot *ptr,float x,float y,float wdt,float hgt,
                 unsigned int color,unsigned  char mask,
                 float width);

int GrplotRawClipPanel(struct Plot *ptr,float x,float y,float wdt,float hgt);



int GrplotRawXaxis(struct Plot *ptr,
                 float ox,float oy,float wdt,float hgt,
		 double xmin,double xmax,
                 double tick_major,double tick_minor,
	         float major_hgt,float minor_hgt,int tick_flg,
		 unsigned int color,unsigned char mask,float width);
  
int GrplotRawYaxis(struct Plot *ptr,
                 float ox,float oy,float wdt,float hgt,
		 double ymin,double ymax,
                 double tick_major,double tick_minor,
	         float major_wdt,float minor_wdt,int tick_flg,
		 unsigned int color,unsigned char mask,float width);

int GrplotRawXaxisLog(struct Plot *ptr,
                    float ox,float oy,float wdt,float hgt,
		    double xmin,double xmax,
                    double tick_major,double tick_minor,
	            float major_hgt,float minor_hgt,int tick_flg,
                    unsigned int color,unsigned char mask,float width);

int GrplotRawYaxisLog(struct Plot *ptr,
                     float ox,float oy,float wdt,float hgt,
		     double ymin,double ymax,
                     double tick_major,double tick_minor,
	             float major_wdt,float minor_wdt,int tick_flg,
		    unsigned int color,unsigned char mask,float width);



int GrplotRawXaxisValue(struct Plot *ptr,
                    float ox,float oy,float wdt,float hgt,
	            double xmin,double xmax,double value,
	            float tick_hgt,int tick_flg,
		    unsigned int color,unsigned char mask,float width);
 
int GrplotRawYaxisValue(struct Plot *ptr,
                     float ox,float oy,float wdt,float hgt,
		     double ymin,double ymax,double value,
	             float tick_wdt,int tick_flg,
		     unsigned int color,unsigned char mask,float width);

int GrplotRawXzero(struct Plot *ptr,
                  float ox,float oy,float wdt,float hgt,
		  double xmin,double xmax,
                  unsigned int color,unsigned char mask,float width,
		  struct PlotDash *dash);

int GrplotRawYzero(struct Plot *ptr,
                  float ox,float oy,float wdt,float hgt,
		  double ymin,double ymax,
                  unsigned int color,unsigned char mask,float width,
		  struct PlotDash *dash);

int GrplotRawXaxisLabel(struct Plot *ptr,
                      float ox,float oy,float wdt,float hgt,
	              double xmin,double xmax,double step,
		      float offset,int txt_flg,int or,
                      int  (*text_box)(char *,float,int,char *,float *,void *),
                      void *boxdata,
                      char * (*make_text)(double,double,double,void *),
                      void *textdata,
                      char *fntname,float fntsze,
                      unsigned int color,unsigned char mask);

int GrplotRawYaxisLabel(struct Plot *ptr,
                      float ox,float oy,float wdt,float hgt,
	              double ymin,double ymax,double step,
		      float offset,int txt_flg,int or,
                      int  (*text_box)(char *,float,int,char *,float *,void *),
                      void *boxdata,
                      char * (*make_text)(double,double,double,void *),
                      void *textdata,
                      char *fntname,float fntsze,
                      unsigned int color,unsigned char mask);

int GrplotRawXaxisLabelValue(struct Plot *ptr,
                      float ox,float oy,float wdt,float hgt,
	              double xmin,double xmax,double value,
			   float offset,int txt_flg,int or,
                      int  (*text_box)(char *,float,int,char *,float *,void *),
                      void *boxdata,
                      int sze,char *text,
                      char *fntname,float fntsze,
			   unsigned int color,unsigned char mask);

int GrplotRawYaxisLabelValue(struct Plot *ptr,
                      float ox,float oy,float wdt,float hgt,
	              double ymin,double ymax,double value,
			   float offset,int txt_flg,int or,
                      int  (*text_box)(char *,float,int,char *,float *,void *),
                      void *boxdata,
                      int sze,char *text,
                      char *fntname,float fntsze,
			   unsigned int color,unsigned char mask);

int GrplotRawXaxisTitle(struct Plot *ptr,
                      float ox,float oy,float wdt,float hgt,
		      float offset,int txt_flg,int or,
                      int  (*text_box)(char *,float,int,char *,float *,void *),
                      void *boxdata,
                      int sze,char *text,
                      char *fntname,float fntsze,
                      unsigned int color,unsigned char mask);



int GrplotRawYaxisTitle(struct Plot *ptr,
                      float ox,float oy,float wdt,float hgt,
		      float offset,int txt_flg,int or,
                      int  (*text_box)(char *,float,int,char *,float *,void *),
                      void *boxdata,
                      int sze,char *text,
                      char *fntname,float fntsze,
                      unsigned int color,unsigned char mask);

int GrplotRawFitImage(struct Plot *ptr,
                       float ox,float oy,float wdt,float hgt,
                       struct FrameBuffer *img,
		 unsigned char mask);

int GrplotRawFitImageName(struct Plot *ptr,
                     float ox,float oy,float wdt,float hgt,
		     char *name,int iwdt,int ihgt,int depth,
                     unsigned char mask);

int GrplotRawLine(struct Plot *ptr,
                float ox,float oy,float wdt,float hgt,
                double ax,double ay,double bx,double by,
		double xmin,double xmax,
                double ymin,double ymax,
                unsigned int color,unsigned char mask,float width,
                struct PlotDash *dash);

int GrplotRawRectangle(struct Plot *ptr,
                struct PlotMatrix *matrix,
                float ox,float oy,float wdt,float hgt,
                double x,double y,double w,double h,
		double xmin,double xmax,
                double ymin,double ymax,
                int fill,unsigned int color,unsigned char mask,float width,
	        struct PlotDash *dash);

int GrplotRawEllipse(struct Plot *ptr,
                struct PlotMatrix *matrix,
                float ox,float oy,float wdt,float hgt,
                double x,double y,double w,double h,
		double xmin,double xmax,
                double ymin,double ymax,
                int fill,unsigned int color,unsigned char mask,float width,
		   struct PlotDash *dash);

int GrplotRawPolygon(struct Plot *ptr,
                struct PlotMatrix *matrix,
                float ox,float oy,float wdt,float hgt,
		 double x,double y,int num,double *px,double *py,
                int *t,
		double xmin,double xmax,
                double ymin,double ymax,
                int fill,unsigned int color,unsigned char mask,float width,
		   struct PlotDash *dash);

int GrplotRawBezier(struct Plot *ptr,
                  float ox,float oy,float wdt,float hgt,
                  double ax,double ay,double bx,double by,
                  double cx,double cy,double dx,double dy,
                  double xmin,double xmax,
                  double ymin,double ymax,
                  unsigned int color,unsigned char mask,float width,
                  struct PlotDash *dash);

int GrplotRawText(struct Plot *ptr,
                struct PlotMatrix *matrix,
                float ox,float oy,float wdt,float hgt,
                char *fontname,float fontsize,
                double x,double y,int num,char *txt,
                float dx,float dy,
		double xmin,double xmax,
                double ymin,double ymax,
                unsigned int color,unsigned char mask);

int GrplotRawImage(struct Plot *ptr,
                struct PlotMatrix *matrix,
                float ox,float oy,float wdt,float hgt,
		struct FrameBuffer *img,
		 float dx,float dy,
	        double x,double y,
		double xmin,double xmax,
                double ymin,double ymax,
                unsigned char mask);


int GrplotRawImageName(struct Plot *ptr,
                struct PlotMatrix *matrix,
                float ox,float oy,float wdt,float hgt,
		char *name,int iwdt,int ihgt,int depth,
                float dx,float dy,double x,double y,
		double xmin,double xmax,
                double ymin,double ymax,
                unsigned char mask);



int GrplotRawFill(struct Plot *ptr,
                float ox,float oy,float wdt,float hgt,
                double ax,double ay,double bx,double by,
		double xmin,double xmax,
                double ymin,double ymax,int or,
                unsigned int color,unsigned char mask);


int GrplotRawBar(struct Plot *ptr,
                float ox,float oy,float wdt,float hgt,
	       double ax,double ay,double w,double o,
		double xmin,double xmax,
	        double ymin,double ymax,int or,int fill,
	        unsigned int color,unsigned char mask,
                float width,struct PlotDash *dash);


int GrplotRawKey(struct Plot *ptr,
               float ox,float oy,float wdt,float hgt,
               double min,double max,
               double tick_major,double tick_minor,
               float major_size,float minor_size,
               float txt_off,float ttl_off,
               int tick_flg,int txt_flg,int ttl_flg,
               int txt_or,int ttl_or,
               int or,
               int ttl_num,char *ttl_text,
               int  (*text_box)(char *,float,int,char *,float *,void *),
               void *boxdata,
               char * (*make_text)(double,double,double,void *),
               void *textdata,
               char *fntname,float fntsze,
               unsigned int color,unsigned char mask,float width,
               int ksze,unsigned char *a,
               unsigned char *r,unsigned char *g,unsigned char *b);

int GrplotRawKeyLog(struct Plot *ptr,
		  float ox,float oy,float wdt,float hgt,
		  double min,double max,
		  double tick_major,double tick_minor,
		  float major_size,float minor_size,
		  float txt_off,float ttl_off,
		  int tick_flg,int txt_flg,int ttl_flg,
		  int txt_or,int ttl_or,
		  int or,
		  int ttl_num,char *ttl_text,
		  int  (*text_box)(char *,float,int,char *,float *,void *),
		  void *boxdata,
		  char * (*make_text)(double,double,double,void *),
		  void *textdata,
		  char *fntname,float fntsze,
		  unsigned int color,unsigned char mask,float width,
		  int ksze,unsigned char *a,
                  unsigned char *r,unsigned char *g,unsigned char *b);

int GrplotRawContour(struct Plot *rplot,struct PolygonData *ptr,
                  float xoff,float yoff,float wdt,float hgt,
                  float step,float smooth,float lwdt,
                  int lstart,int lstep,
                  int sze,char *text,
                  unsigned int color,char mask,float width,
                  struct PlotDash *dash,
                  char *fntname,float fntsze,
                  int  (*text_box)(char *,float,int,char *,float *,void *),
		   void *boxdata);

int GrplotRawContourPolygon(struct Plot *rplot,struct PolygonData *ptr,
                  float xoff,float yoff,float wdt,float hgt,
                  float step,float smooth,
		       unsigned int color,char mask,int fill,
                  float width,
		  struct PlotDash *dash);

#endif
