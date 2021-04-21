/* plot.h
   ======= 
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

#ifndef _PLOT_H
#define _PLOT_H


struct PlotDash {
  float *p;
  int sze;
  float phase;
};

struct PlotMatrix {
  float a,b,c,d;
};


struct Plot { 

  struct {
    struct {
      int (*func)(void *);
      void *data;
    } start;

    struct {
      int (*func)(void *,char *buf,int sze);
      void *data;
    } text;

    struct {
      int (*func)(void *);
      void *data;
    } end;
  } info;

  struct {
    struct {
      int (*func)(void *,char *,float,float,int);
      void *data;
    } start;
    struct {
      int (*func)(void *);
      void *data;
    } end;
  } plot;

  struct {
    struct {
      int (*func)(void *,char *,char *,float,float,int);
      void *data;
    } start;
    struct {
      int (*func)(void *);
      void *data;
    } end;
  } document;

  struct {
    struct {
      int (*func)(void *,struct PlotMatrix *,float,float);
      void *data;
    } start;
    struct {
      int (*func)(void *);
      void *data;
    } end;
  } embed;


  struct {
    int (*func)(void *,char *name);
    void *data;
  } include;

  struct {
    int (*func)(void *,int, float *,float *,int *);
    void *data;
  } clip;

  struct {
    int (*func)(void *,float,float,float,float,
		unsigned int,unsigned char,float,
		struct PlotDash *);
    void *data;
  } line;


  struct {
    int (*func)(void *,float,float,float,float,
                float,float,float,float,
                unsigned int,unsigned char,float,
                struct PlotDash *);
    void *data;
  } bezier;


  struct {
    int (*func)(void *,struct PlotMatrix *,
		float,float,float,float,
		int,unsigned int,unsigned char,
		float,struct PlotDash *);
    void *data;
  } rectangle;


  struct {
    int (*func)(void *,struct PlotMatrix *,
                float,float,float,float,
                int,unsigned int,unsigned char,
                float,struct PlotDash *);
    void *data;
  } ellipse;


  struct {
    int (*func)(void *,struct PlotMatrix *,
                float,float,int,float *,float *,int *,int,
		unsigned int,unsigned char,float,struct PlotDash *);
    void *data;
  } polygon;


  struct {
    int (*func)(void *,struct PlotMatrix *,
		char *,float,float,float,
		int,char *,unsigned int,unsigned char,int);
    void *data;
  } text;



  struct {
    int (*func)(void *,struct PlotMatrix *,
                struct FrameBuffer *,
                unsigned  char,float,float,int);
    void *data;
  } image;

  struct {
    int (*func)(void *,struct PlotMatrix *,
		char *,int,int,int,
		unsigned  char,float,float,int);
    void *data;
  } imagename;



};


struct PlotDash *PlotMakeDash(float *p,float phase,int sze);
struct PlotDash *PlotMakeDashString(char *str);
void PlotFreeDash(struct PlotDash *ptr);

struct PlotMatrix *PlotMatrixString(char *str);
int PlotScaleMatrix(struct PlotMatrix *ptr,
                     float xscale,float yscale);
int PlotRotateMatrix(struct PlotMatrix *ptr,
                      float angle);

int PlotMatrixTransform(struct PlotMatrix *ptr,float ix,float iy,
                         float *ox,float *oy);


unsigned int PlotColor(int r,int g,int b,int a);
unsigned int PlotColorStringRGB(char *txt);
unsigned int PlotColorStringRGBA(char *txt);

struct Plot *PlotMake();
void PlotFree(struct Plot *ptr);

int PlotPlotStart(struct Plot *ptr,char *name,float wdt,float hgt,int depth);
int PlotPlotEnd(struct Plot *ptr);

int PlotDocumentStart(struct Plot *ptr,char *name,char *pages,
                      float wdt,float hgt,int depth);
int PlotDocumentEnd(struct Plot *ptr);

int PlotLine(struct Plot *ptr,
              float ax,float ay,float bx,float by,
              unsigned int color,unsigned char mask,float width,
              struct PlotDash *dash);
int PlotBezier(struct Plot *ptr,
                float x1,float y1,float x2,float y2,
                float x3,float y3,float x4,float y4,
                unsigned int color,unsigned char mask,float width,
                struct PlotDash *dash);

int PlotRectangle(struct Plot *ptr,
	           struct PlotMatrix *matrix,
                   float,float y,float w,float h,
                   int fill,unsigned int color,unsigned  char mask,
                   float width,
		   struct PlotDash *dash);

int PlotEllipse(struct Plot *ptr,
	           struct PlotMatrix *matrix,
                   float,float y,float w,float h,
                   int fill,unsigned int color,unsigned  char mask,
                   float width,
		   struct PlotDash *dash);

int PlotPolygon(struct Plot *ptr,struct PlotMatrix *matrix,
                 float x,float y,
                 int num,float *px,float *py,int *t,int fill,
                 unsigned int color,unsigned char mask,float width,
                 struct PlotDash *dash);


int PlotText(struct Plot *ptr,
                    struct PlotMatrix *matrix,
                    char *fname,float fsize,float x,float y,
                    int num,char *txt,unsigned int color,unsigned char m,
                    int sflg);

int PlotImage(struct Plot *ptr,
	       struct PlotMatrix *matrix,
	       struct FrameBuffer *img,
	       unsigned  char mask,
	       float x,float y,int sflg);

int PlotImageName(struct Plot *ptr,
	       struct PlotMatrix *matrix,
	       char *name,int w,int h,int depth,
	       unsigned  char mask,
	       float x,float y,int sflg);


int PlotEmbedStart(struct Plot *ptr,
               struct PlotMatrix *matrix,
               float x,float y);

int PlotInclude(struct Plot *ptr,char *name);


int PlotEmbedEnd(struct Plot *ptr);

int PlotClip(struct Plot *ptr,
                     int num,float *px,float *py,int *t);


int PlotInfoStart(struct Plot *ptr);
int PlotInfo(struct Plot *ptr,char *buf,int sze);
int PlotInfoEnd(struct Plot *ptr);


#endif
