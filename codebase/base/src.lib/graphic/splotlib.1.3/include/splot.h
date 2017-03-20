/* splot.h
   ======= 
   Author: R.J.Barnes
*/

/*
   See license.txt
*/




#ifndef _SPLOT_H
#define _SPLOT_H

struct Splot { 

  struct {
    struct PostScript *ptr;
    struct PostScriptClip *clip;
    struct PostScriptDash *dash;
    int land;
    float xpoff,ypoff;
  } ps;
     
  struct {
    struct FrameBuffer **ptr;
    struct FrameBufferFontDB *fontdb;
    unsigned int bgcolor;
    struct FrameBufferClip *clip;
    struct FrameBufferDash *dash;
    int (*trigger)(struct FrameBuffer *,void *data);
    void *data;
  } fb;
};

int SplotFrameBufferDash(struct Splot *ptr,struct PlotDash *dash);
int SplotPostScriptDash(struct Splot *ptr,struct PlotDash *dash);


unsigned int SplotPostScriptColor(unsigned int color);

int SplotSetPlot(struct Plot *plot,struct Splot *ptr);

struct Splot *SplotMake();
void SplotFree(struct Splot *ptr);
int SplotSetFrameBuffer(struct Splot *ptr,struct FrameBuffer **img,
                        struct FrameBufferFontDB *fontdb,
                        int (*trigger)(struct FrameBuffer *,void *),void *data);

int SplotSetPostScript(struct Splot *ptr,struct PostScript *ps,
                       int land,float xpoff,float ypoff);


int SplotPlotStart(struct Splot *ptr,char *name,float wdt,float hgt,int depth);
int SplotPlotEnd(struct Splot *ptr);

int SplotDocumentStart(struct Splot *ptr,char *name,char *pages,float wdt,float hgt,int depth);
int SplotDocumentEnd(struct Splot *ptr);

int SplotLine(struct Splot *ptr,
              float ax,float ay,float bx,float by,
              unsigned int color,unsigned char mask,float width,
              struct PlotDash *dash);
int SplotBezier(struct Splot *ptr,
                float x1,float y1,float x2,float y2,
                float x3,float y3,float x4,float y4,
                unsigned int color,unsigned char mask,float width,
                struct PlotDash *dash);

int SplotRectangle(struct Splot *ptr,
	           struct PlotMatrix *matrix,
                   float,float y,float w,float h,
                   int fill,unsigned int color,unsigned  char mask,
                   float width,
		   struct PlotDash *dash);

int SplotEllipse(struct Splot *ptr,
	           struct PlotMatrix *matrix,
                   float,float y,float w,float h,
                   int fill,unsigned int color,unsigned  char mask,
                   float width,
		   struct PlotDash *dash);

int SplotPolygon(struct Splot *ptr,struct PlotMatrix *matrix,
                 float x,float y,
                 int num,float *px,float *py,int *t,int fill,
                 unsigned int color,unsigned char mask,float width,
                 struct PlotDash *dash);

int SplotText(struct Splot *ptr,
                    struct PlotMatrix *matrix,
                    char *fname,float fsize,float x,float y,
                    int num,char *txt,unsigned int color,unsigned char m,
                    int sflg);

int SplotImage(struct Splot *ptr,
	       struct PlotMatrix *matrix,
	       struct FrameBuffer *img,
	       unsigned  char mask,
	       float x,float y,int sflg);


int SplotClip(struct Splot *ptr,
                     int num,float *px,float *py,int *t);
#endif
