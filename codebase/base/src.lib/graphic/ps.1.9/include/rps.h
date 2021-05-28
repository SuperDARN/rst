/* rps.h
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




#ifndef _RPS_H
#define _RPS_H

#define STROKE_COUNT 100

struct PostScriptDash {
  float *p;
  int sze;
  float phase;
};

struct PostScriptClip {
  int num;
  float *px;
  float *py;
  int *t;
};

struct PostScript { 
  int cnt;
  float x,y,wdt,hgt;
  int land;
  int num;
  int pagenum;

  float width;
  unsigned int color;
  struct PostScriptDash *dash;
  struct PostScriptClip *clip;
  float px,py;


  struct {
    int (*func)(char *,int,void *);
    void *data;
  } text;
};




struct PostScriptMatrix {
  float a,b,c,d;
};

struct PostScript *PostScriptMake();
void PostScriptFree(struct PostScript *ptr);
int PostScriptSetText(struct PostScript *ptr,
                      int (*text)(char *,int,void *),void *data);

struct PostScriptMatrix *PostScriptMatrixString(char *str);
int PostScriptScaleMatrix(struct PostScriptMatrix *ptr,
			  float xscale,float yscale);
int PostScriptRotateMatrix(struct PostScriptMatrix *ptr,
			   float angle);
unsigned int PostScriptColor(int r,int g,int b);



int PostScriptDocumentStart(struct PostScript *ptr,
                       float x,float y,float wdt,float hgt,
                       int land);
int PostScriptPlotStart(struct PostScript *ptr);
int PostScriptPlotEnd(struct PostScript *ptr);
int PostScriptDocumentEnd(struct PostScript *ptr);

struct PostScriptClip *PostScriptMakeClip(float x,float y,float wdt,float hgt,
                                          int num,float *px,float *py,int *t);
void PostScriptFreeClip(struct PostScriptClip *ptr);
int PostScriptClip(struct PostScript *ptr,struct PostScriptClip *clip);

struct PostScriptDash *PostScriptMakeDash(float *p,float phase,int sze);
struct PostScriptDash *PostScriptMakeDashString(char *str);
void PostScriptFreeDash(struct PostScriptDash *ptr);


int PostScriptState(struct PostScript *ptr,
                    unsigned int color,float width,
                    struct PostScriptDash *dash,
                    struct PostScriptClip *clip);

int PostScriptLine(struct PostScript *ptr,
              float ax,float ay,float bx,float by,
              unsigned int color,float width,
		   struct PostScriptDash *dash,
                   struct PostScriptClip *clip);

int PostScriptBezier(struct PostScript *ptr,
                     float x1,float y1,float x2,float y2,float x3,float y3,
                     float x4,float y4,
                     unsigned int color,float width,
                     struct PostScriptDash *dash,
                     struct PostScriptClip *clip);

int PostScriptPolygon(struct PostScript *ptr,
                   struct PostScriptMatrix *matrix,
                   float x,float y,
                   int num,float *px,float *py,int *t,int fill,
                   unsigned int color,float width,
		   struct PostScriptDash *dash,
                   struct PostScriptClip *clip);

int PostScriptEllipse(struct PostScript *ptr,
                      struct PostScriptMatrix *matrix,
                      float x,float y,float w,float h,
                      int fill,unsigned int color,
                      float width,
                      struct PostScriptDash *dash,
                      struct PostScriptClip *clip);

int PostScriptRectangle(struct PostScript *ptr,
                 struct PostScriptMatrix *matrix,
                 float x,float y,float w,float h,
                 int fill,unsigned int color,
                 float width,
			struct PostScriptDash *dash,
                        struct PostScriptClip *clip);

int PostScriptText(struct PostScript *ptr,
	           struct PostScriptMatrix *matrix,
                   char *fname,float fsize,
                   float x,float y,int num,char *txt,
		   unsigned int color,
                   struct PostScriptClip *clip);

int PostScriptImage(struct PostScript *ptr,
	           struct PostScriptMatrix *matrix,
		   struct FrameBuffer *img,unsigned char mask,
                   float x,float y,
                   struct PostScriptClip *clip);

#endif
