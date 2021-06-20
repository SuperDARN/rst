/* rplotg.h
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



#ifndef _GRPLOT_H
#define _GRPLOT_H

struct Grplot {
  struct Plot *plot;
  int xnum;
  int ynum;
  float xoff;
  float yoff;
  float tpad;
  float bpad;  
  float lpad;
  float rpad;

  float major_wdt;
  float minor_wdt;
  float major_hgt;
  float minor_hgt;

  float ttl_wdt;
  float ttl_hgt;
  int ttl_xor;
  int ttl_yor;
  
  float lbl_wdt;
  float lbl_hgt;
  int lbl_xor;
  int lbl_yor;
 
  float box_wdt;
  float box_hgt;

  struct {
    int  (*box)(char *fntname,float fntsize,int num,char *text,
                 float *box,void *data);
    void *data;
  } text;
};


struct Grplot *GrplotMake(float wdt,float hgt,
                          int xnum,int ynum,float lpad,float rpad,
                          float bpad,float tpad,float xoff,float yoff);
void GrplotFree(struct Grplot *ptr);
int GrplotSetPlot(struct Grplot *plt,struct Plot *rplot);
int GrplotSetTextBox(struct Grplot *plt,
                   int  (*text)(char *,float,int,char *,float *,void *),
                   void *data);

int GrplotPanel(struct Grplot *plt,int num,unsigned int color,
              unsigned char mask,float width);

int GrplotClipPanel(struct Grplot *plt,int num);

int GrplotXaxis(struct Grplot *plt,int num,
              double xmin,double xmax,
              double tick_major,double tick_minor,
	      int tick_flg,
	      unsigned int color,unsigned char mask,float width);


int GrplotYaxis(struct Grplot *plt,int num,
	      double ymin,double ymax,
              double tick_major,double tick_minor,
	      int tick_flg,
	      unsigned int color,unsigned char mask,float width);

int GrplotXaxisLog(struct Grplot *plt,int num,
		 double xmin,double xmax,
                 double tick_major,double tick_minor,
	         int tick_flg,
                 unsigned int color,unsigned char mask,float width);

int GrplotYaxisLog(struct Grplot *plt,int num,
		 double ymin,double ymax,
                 double tick_major,double tick_minor,
	         int tick_flg,
		 unsigned int color,unsigned char mask,float width);

int GrplotXaxisValue(struct Grplot *plt,int num,
	           double xmin,double xmax,double value,
	           float tick_hgt,int tick_flg,
		   unsigned int color,unsigned char mask,float width);

int GrplotYaxisValue(struct Grplot *plt,int num,
		   double ymin,double ymax,double value,
	           float tick_wdt,int tick_flg,
		   unsigned int color,unsigned char mask,float width);



int GrplotXzero(struct Grplot *plt,int num,
	      double xmin,double xmax,
              unsigned int color,unsigned char mask,float width,
	      struct PlotDash *dash);

int GrplotYzero(struct Grplot *plt,int num,
	      double ymin,double ymax,
              unsigned int color,unsigned char mask,float width,
	      struct PlotDash *dash);

int GrplotXaxisLabel(struct Grplot *plt,int num,
	           double xmin,double xmax,double step,
		   int txt_flg,
                   char * (*make_text)(double,double,double,void *),
                   void *textdata,
                   char *fntname,float fntsze,
		   unsigned int color,unsigned char mask);


int GrplotYaxisLabel(struct Grplot *plt,int num,
	           double ymin,double ymax,double step,
	           int txt_flg,
                   char * (*make_text)(double,double,double,void *),
                   void *textdata,
                   char *fntname,float fntsze,
		   unsigned int color,unsigned char mask);


int GrplotXaxisLabelValue(struct Grplot *plt,int num,
	                double xmin,double xmax,double value,
			int txt_flg,
                        int sze,char *text,
                        char *fntname,float fntsze,
		        unsigned int color,unsigned char mask);

int GrplotYaxisLabelValue(struct Grplot *plt,int num,
	                double ymin,double ymax,double value,
		        int txt_flg,
                        int sze,char *text,
                        char *fntname,float fntsze,
		        unsigned int color,unsigned char mask);


int GrplotXaxisTitle(struct Grplot *plt,int num,
		   int txt_flg,
                   int sze,char *text,
                   char *fntname,float fntsze,
                   unsigned int color,unsigned char mask);


int GrplotXaxisTitleOffset(struct Grplot *plt,int num,
		   int txt_flg,
		   int sze,char *text,float offset,
                   char *fntname,float fntsze,
                   unsigned int color,unsigned char mask);


int GrplotYaxisTitle(struct Grplot *plt,int num,
		   int txt_flg,
                   int sze,char *text,
                   char *fntname,float fntsze,
                   unsigned int color,unsigned char mask);

int GrplotYaxisTitleOffset(struct Grplot *plt,int num,
		   int txt_flg,
                   int sze,char *text,float offset,
                   char *fntname,float fntsze,
                   unsigned int color,unsigned char mask);


int GrplotFitImage(struct Grplot *plt,int num,
              struct FrameBuffer *img,
	      unsigned char mask);

int GrplotFitImageName(struct Grplot *plt,int num,
	          char *name,int iwdt,int ihgt,int depth,
                  unsigned char mask);

int GrplotLine(struct Grplot *plt,int num,
             double ax,double ay,double bx,double by,
	     double xmin,double xmax,
             double ymin,double ymax,
             unsigned int color,unsigned char mask,float width,
             struct PlotDash *dash);

int GrplotRectangle(struct Grplot *plt,int num,
		  struct PlotMatrix *matrix,
                  double x,double y,double w,double h,
	          double xmin,double xmax,
                  double ymin,double ymax,
                  int fill,unsigned int color,unsigned char mask,float width,
                  struct PlotDash *dash);

int GrplotEllipse(struct Grplot *plt,int num,
	        struct PlotMatrix *matrix,
                double x,double y,double w,double h,
	        double xmin,double xmax,
                double ymin,double ymax,
                int fill,unsigned int color,unsigned char mask,float width,
		struct PlotDash *dash);

int GrplotBezier(struct Grplot *plt,int num,
               double ax,double ay,double bx,double by,
               double cx,double cy,double dx,double dy,
               double xmin,double xmax,
               double ymin,double ymax,
               unsigned int color,unsigned char mask,float width,
               struct PlotDash *dash);

int GrplotPolygon(struct Grplot *plt,int num,
		struct PlotMatrix *matrix,
		double x,double y,int n,double *px,double *py,int *t,
	        double xmin,double xmax,
                double ymin,double ymax,
                int fill,unsigned int color,unsigned char mask,float width,
                struct PlotDash *dash);

int GrplotFill(struct Grplot *plt,int num,
             double ax,double ay,double bx,double by,
	     double xmin,double xmax,
             double ymin,double ymax,int or,
	     unsigned int color,unsigned char mask);

int GrplotText(struct Grplot *plt,int num,
	     struct PlotMatrix *matrix,char *fontname,
             float fontsize,double x,double y,
             int n,char *txt,float dx,float dy,
	     double xmin,double xmax,
             double ymin,double ymax,
             unsigned int color,unsigned char mask);

int GrplotImageName(struct Grplot *plt,int num,
	          struct PlotMatrix *matrix,
                  char *name,int iwdt,int ihgt,int depth,float dx,float dy,
                  double x,double y,
	          double xmin,double xmax,
                  double ymin,double ymax,
                  unsigned char mask);

int GrplotImage(struct Grplot *plt,int num,
	     struct PlotMatrix *matrix,
	      struct FrameBuffer *img,float dx,float dy,
             double x,double y,
	     double xmin,double xmax,
             double ymin,double ymax,
	     unsigned char mask);

int GrplotBar(struct Grplot *plt,int num,
	    double ax,double ay,double w,double o,
	    double xmin,double xmax,
	    double ymin,double ymax,int or,int fill,
	    unsigned int color,unsigned char mask,
            float width,struct PlotDash *dash);

int GrplotGetPoint(struct Grplot *plt,int num,
             double x,double y,
	     double xmin,double xmax,
	     double ymin,double ymax,float *px,float *py);


int GrplotKey(struct Grplot *plt,int num,
            float xoff,float yoff,
            float wdt,float hgt,
	    double xmin,double xmax,double step,
	    int key_flg,int ttl_num,char *ttl_txt,
            char * (*make_text)(double,double,double,void *),
            void *textdata,
            char *fntname,float fntsze,
	    unsigned int color,unsigned char mask,float width,
            int ksze,unsigned char *a,
            unsigned char *r,unsigned char *g,unsigned char *b);

int GrplotKeyLog(struct Grplot *plt,int num,
            float xoff,float yoff,
            float wdt,float hgt,
	    double xmin,double xmax,double tick_major,double tick_minor,
	    int key_flg,int ttl_num,char *ttl_txt,
            char * (*make_text)(double,double,double,void *),
            void *textdata,
            char *fntname,float fntsze,
	    unsigned int color,unsigned char mask,float width,
	    int ksze,unsigned char *a,
            unsigned char *r,unsigned char *g,unsigned char *b);


#endif

