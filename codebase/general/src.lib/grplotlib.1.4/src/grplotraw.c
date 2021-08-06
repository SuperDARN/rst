/* grplotraw.c
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "rfbuffer.h"
#include "iplot.h"




int GrplotRawGetPoint(float ox,float oy,float wdt,float hgt,
	            double x,double y,double xmin,double xmax,double ymin,
	            double ymax,float *px,float *py) {
 
  if (px==NULL) return -1;
  if (py==NULL) return -1;

  *px=wdt*(x-xmin)/(xmax-xmin);
  *py=hgt*(y-ymin)/(ymax-ymin);

  if (*py>hgt) *py=hgt;
  if (*py<0) *py=0;
  if (*px>wdt) *px=wdt;
  if (*px<0) *px=0;
  *px=ox+*px;
  *py=oy+hgt-*py;  
  return 0;
}


int GrplotRawPanel(struct Plot *ptr,float x,float y,float wdt,float hgt,
                 unsigned int color,unsigned  char mask,
                 float width) {
  if (ptr==NULL) return -1;
  return PlotRectangle(ptr,NULL,x,y,wdt,hgt,0,color,mask,width,NULL);
}

int GrplotRawClipPanel(struct Plot *ptr,float x,float y,float wdt,float hgt) {
  

  float cx[4],cy[4];
  int ct[4]={0,0,0,0};

  cx[0]=x;
  cx[1]=x+wdt;
  cx[2]=x+wdt;
  cx[3]=x;
  cy[0]=y;
  cy[1]=y;
  cy[2]=y+hgt;
  cy[3]=y+hgt;
  return PlotClip(ptr,4,cx,cy,ct);
}


int GrplotRawXaxis(struct Plot *ptr,
                    float ox,float oy,float wdt,float hgt,
		    double xmin,double xmax,
                    double tick_major,double tick_minor,
	            float major_hgt,float minor_hgt,int tick_flg,
                    unsigned int color,unsigned char mask,float width) {
  int s=0;
 
  double x;
  float px;

  if (ptr==NULL) return -1;


  if (tick_major !=0) {
    for (x=xmin;x<xmax+(tick_major/2);x+=tick_major) {
      px=wdt*(x-xmin)/(xmax-xmin);
      if (tick_flg & 0x04) 
        s=PlotLine(ptr,ox+px,oy,ox+px,oy+hgt,color,mask,width,NULL);
      if (s !=0) break;
      if (tick_flg & 0x01) 
        s=PlotLine(ptr,ox+px,oy+hgt,ox+px,oy+hgt+major_hgt,
                        color,mask,width,NULL);
      if (s !=0) break;
      if (tick_flg & 0x02) 
        s=PlotLine(ptr,ox+px,oy,ox+px,oy-major_hgt,
                        color,mask,width,NULL);
      if (s !=0) break; 
    }
    if (s !=0) return s;
  }
  if (tick_minor !=0) { 
    for (x=xmin;x<xmax+(tick_minor/2);x+=tick_minor) {
      px=wdt*(x-xmin)/(xmax-xmin);
      if (tick_flg & 0x08) 
        s=PlotLine(ptr,ox+px,oy,ox+px,oy+hgt,color,mask,width,NULL);
      if (s !=0) break;
      if (tick_flg & 0x01) 
        s=PlotLine(ptr,ox+px,oy+hgt,ox+px,oy+hgt+minor_hgt,
                        color,mask,width,NULL);
      if (s !=0) break;
      if (tick_flg & 0x02) 
        s=PlotLine(ptr,ox+px,oy,ox+px,oy-minor_hgt,
                        color,mask,width,NULL);
      if (s !=0) break;
    }
    if (s !=0) return s;
  }
  return 0;
}



int GrplotRawYaxis(struct Plot *ptr,
                    float ox,float oy,float wdt,float hgt,
		    double ymin,double ymax,
                    double tick_major,double tick_minor,
	            float major_wdt,float minor_wdt,int tick_flg,
                    unsigned int color,unsigned char mask,float width) {
  int s=0;

  double y;
  float py;

  if (ptr==NULL) return -1;

  if (tick_major !=0) {
    for (y=ymin;y<ymax+(tick_major/2);y+=tick_major) {
      py=hgt*(y-ymin)/(ymax-ymin);
      if (tick_flg & 0x04) 
        s=PlotLine(ptr,ox,oy+hgt-py,ox+wdt,
                        oy+hgt-py,color,mask,width,NULL);
      if (s !=0) break;
      if (tick_flg & 0x01) 
        s=PlotLine(ptr,ox,oy+hgt-py,ox-major_wdt,oy+hgt-py,
                        color,mask,width,NULL);
      if (s !=0) break;
      if (tick_flg & 0x02) 
        s=PlotLine(ptr,ox+wdt,oy+hgt-py,ox+wdt+major_wdt,oy+hgt-py,
                        color,mask,width,NULL);
      if (s !=0) break; 
    }
    if (s !=0) return s;
  }
  if (tick_minor !=0) { 
    for (y=ymin;y<ymax+(tick_minor/2);y+=tick_minor) {
      py=hgt*(y-ymin)/(ymax-ymin);
      if (tick_flg & 0x08) 
        s=PlotLine(ptr,ox,oy+hgt-py,ox+wdt,oy+hgt-py,
                        color,mask,width,NULL);
      if (s !=0) break;
      if (tick_flg & 0x01) 
        s=PlotLine(ptr,ox,oy+hgt-py,ox-minor_wdt,oy+hgt-py,
                        color,mask,width,NULL);
      if (s !=0) break;
      if (tick_flg & 0x02) 
        s=PlotLine(ptr,ox+wdt,oy+hgt-py,ox+wdt+minor_wdt,oy+hgt-py,
                        color,mask,width,NULL);
      if (s !=0) break;
    }
    if (s !=0) return s;
  }
  return 0;
}




int GrplotRawXaxisLog(struct Plot *ptr,
                    float ox,float oy,float wdt,float hgt,
		    double xmin,double xmax,
                    double tick_major,double tick_minor,
	            float major_hgt,float minor_hgt,int tick_flg,
                    unsigned int color,unsigned char mask,float width) {
  int s=0;

  double x,xa;
  float px;

  if (ptr==NULL) return -1;

  if (tick_major !=0) {
    for (x=xmin;x<xmax+(tick_major/2);x+=tick_major) {
      px=wdt*(x-xmin)/(xmax-xmin);
      if (tick_flg & 0x04) 
        s=PlotLine(ptr,ox+px,oy,ox+px,oy+hgt,color,mask,width,NULL);
      if (s !=0) break;
      if (tick_flg & 0x01) 
        s=PlotLine(ptr,ox+px,oy+hgt,ox+px,oy+hgt+major_hgt,
                        color,mask,width,NULL);
      if (s !=0) break;
      if (tick_flg & 0x02) 
        s=PlotLine(ptr,ox+px,oy,ox+px,oy-major_hgt,
                        color,mask,width,NULL);
      if (s !=0) break; 
    }
    if (s !=0) return s;
  }
  if (tick_minor !=0) { 
    for (xa=xmin+tick_major;xa<xmax+tick_major;xa+=tick_major) {
      for (x=1.0/tick_minor;x<=1.0;x+=1.0/tick_minor) {
        px=wdt*(log10(x*pow(10,xa))-xmin)/(xmax-xmin); 
        if (tick_flg & 0x08) 
          s=PlotLine(ptr,ox+px,oy,ox+px,oy+hgt,color,mask,width,NULL);
        if (s !=0) break;
        if (tick_flg & 0x01) 
          s=PlotLine(ptr,ox+px,oy+hgt,ox+px,oy+hgt+minor_hgt,
                        color,mask,width,NULL);
        if (s !=0) break;
        if (tick_flg & 0x02) 
          s=PlotLine(ptr,ox+px,oy,ox+px,oy-minor_hgt,
                        color,mask,width,NULL);
        if (s !=0) break;
      }
      if (s !=0) break;
    }
    if (s !=0) return s;
  }
  return 0;
}

int GrplotRawYaxisLog(struct Plot *ptr,
                     float ox,float oy,float wdt,float hgt,
		     double ymin,double ymax,
                     double tick_major,double tick_minor,
	             float major_wdt,float minor_wdt,int tick_flg,
                     unsigned int color,unsigned char mask,float width) {
  int s=0;

  double y,ya;
  float py;

  if (ptr==NULL) return -1; 

  if (tick_major !=0) {
    for (y=ymin;y<ymax+(tick_major/2);y+=tick_major) {
      py=hgt*(y-ymin)/(ymax-ymin);
      if (tick_flg & 0x04) 
        s=PlotLine(ptr,ox,oy+hgt-py,ox+wdt,
                        oy+hgt-py,color,mask,width,NULL);
      if (s !=0) break;
      if (tick_flg & 0x01) 
        s=PlotLine(ptr,ox,oy+hgt-py,ox-major_wdt,oy+hgt-py,
                        color,mask,width,NULL);
      if (s !=0) break;
      if (tick_flg & 0x02) 
        s=PlotLine(ptr,ox+wdt,oy+hgt-py,ox+wdt+major_wdt,oy+hgt-py,
                        color,mask,width,NULL);
      if (s !=0) break; 
    }
    if (s !=0) return s;
  }
  if (tick_minor !=0) {
   for (ya=ymin+tick_major;ya<ymax+tick_major;ya+=tick_major) {
      for (y=1.0/tick_minor;y<=1.0;y+=1.0/tick_minor) {
        py=hgt*(log10(y*pow(10,ya))-ymin)/(ymax-ymin); 

        if (tick_flg & 0x08) 
          s=PlotLine(ptr,ox,oy+hgt-py,ox+wdt,oy+hgt-py,
                        color,mask,width,NULL);
        if (s !=0) break;
        if (tick_flg & 0x01) 
          s=PlotLine(ptr,ox,oy+hgt-py,ox-minor_wdt,oy+hgt-py,
                        color,mask,width,NULL);
        if (s !=0) break;
        if (tick_flg & 0x02) 
          s=PlotLine(ptr,ox+wdt,oy+hgt-py,ox+wdt+minor_wdt,oy+hgt-py,
                        color,mask,width,NULL);
        if (s !=0) break;
      }
      if (s !=0) break;
    }
    if (s !=0) return s;
  }
  return 0;
}    
       

int GrplotRawXaxisValue(struct Plot *ptr,
                    float ox,float oy,float wdt,float hgt,
	            double xmin,double xmax,double value,
	            float tick_hgt,int tick_flg,
                    unsigned int color,unsigned char mask,float width) {
  int s=0;
  float px;

  if (ptr==NULL) return -1;
  px=wdt*(value-xmin)/(xmax-xmin);
  
  if (tick_flg & 0x04) 
     s=PlotLine(ptr,ox+px,oy,ox+px,oy+hgt,color,mask,width,NULL);
  if (s !=0) return s;
  if (tick_flg & 0x01) 
    s=PlotLine(ptr,ox+px,oy+hgt,ox+px,oy+hgt+tick_hgt,
                        color,mask,width,NULL);
  if (s !=0) return s;
  if (tick_flg & 0x02) 
    s=PlotLine(ptr,ox+px,oy,ox+px,oy-tick_hgt,
                        color,mask,width,NULL);
  return s;  
}


int GrplotRawYaxisValue(struct Plot *ptr,
                     float ox,float oy,float wdt,float hgt,
		     double ymin,double ymax,double value,
	             float tick_wdt,int tick_flg,
                     unsigned int color,unsigned char mask,float width) {

  int s=0; 
  float py;
 
  if (ptr==NULL) return -1;

  py=hgt*(value-ymin)/(ymax-ymin);
  if (tick_flg & 0x04) 
    s=PlotLine(ptr,ox,oy+hgt-py,ox+wdt,
                        oy+hgt-py,color,mask,width,NULL);
  if (s !=0) return s;
  if (tick_flg & 0x01) 
    s=PlotLine(ptr,ox,oy+hgt-py,ox-tick_wdt,oy+hgt-py,
                        color,mask,width,NULL);
  if (s !=0) return s;
  if (tick_flg & 0x02) 
    s=PlotLine(ptr,ox+wdt,oy+hgt-py,ox+wdt+tick_wdt,oy+hgt-py,
                        color,mask,width,NULL);
  return s;
}


int GrplotRawXzero(struct Plot *ptr,
                  float ox,float oy,float wdt,float hgt,
		  double xmin,double xmax,
                  unsigned int color,unsigned char mask,float width,
                  struct PlotDash *dash) {

   float px;
   if (ptr==NULL) return -1;


   px=wdt*(-xmin)/(xmax-xmin);
   return PlotLine(ptr,ox+px,oy+hgt,ox+px,oy,color,mask,width,dash);
}

int GrplotRawYzero(struct Plot *ptr,
                  float ox,float oy,float wdt,float hgt,
		  double ymin,double ymax,
                  unsigned int color,unsigned char mask,float width,
                  struct PlotDash *dash) {
  float py;

  if (ptr==NULL) return -1;
  py=hgt*(-ymin)/(ymax-ymin);
  return PlotLine(ptr,ox,oy+hgt-py,ox+wdt,oy+hgt-py,color,mask,width,dash);
}


int GrplotRawXaxisLabel(struct Plot *ptr,
                      float ox,float oy,float wdt,float hgt,
	              double xmin,double xmax,double step,
		      float offset,int txt_flg,int or,
                      int  (*text_box)(char *,float,int,char *,float *,void *),
                      void *boxdata,
                      char * (*make_text)(double,double,double,void *),
                      void *textdata,
                      char *fntname,float fntsze,
                      unsigned int color,unsigned char mask) {

  float px;
  double x;
  float txtbx[3];
  char *text=NULL;
  int s=0;

  if (ptr==NULL) return -1;
  if (make_text==NULL) return -1;
  if (text_box==NULL) return -1;
  if (fntname==NULL) return -1;
  if (step==0) return 0;

  for (x=xmin;x<xmax+(step/2);x+=step) {
     text=(*make_text)(x,xmin,xmax,textdata);
     if (text==NULL) continue;
     s=(*text_box)(fntname,fntsze,strlen(text),text,txtbx,boxdata);
     if (s !=0) break;
     px=wdt*(x-xmin)/(xmax-xmin);

     if (or==0) {
       if (txt_flg & 0x01) 
         s=PlotText(ptr,NULL,fntname,fntsze,ox+px-txtbx[0]/2,
                     oy+hgt+offset+txtbx[2],strlen(text),text,
                     color,0x0f,1);
       if (s !=0) break;                     
       if (txt_flg & 0x02) 
         s=PlotText(ptr,NULL,fntname,fntsze,ox+px-txtbx[0]/2,
                     oy-offset+txtbx[1],strlen(text),text,
                     color,0x0f,1);
       if (s !=0) break;
     } else {
       struct PlotMatrix rm;
       rm.a=0;
       rm.b=1;
       rm.c=-1;
       rm.d=0;

       if (txt_flg & 0x01) 
         s=PlotText(ptr,&rm,fntname,fntsze,ox+px+(txtbx[2]+txtbx[1])/2.0,
                     oy+hgt+offset+txtbx[0],strlen(text),text,
                     color,0x0f,1);
       if (s !=0) break;                     
       if (txt_flg & 0x02) 
         s=PlotText(ptr,&rm,fntname,fntsze,ox+px+(txtbx[1]+txtbx[2])/2.0,
                     oy-offset,strlen(text),text,
                     color,0x0f,1);
       if (s !=0) break;
     }


     free(text);
     text=NULL;
  }
  if (text !=NULL) free(text);
  return s;
}

int GrplotRawYaxisLabel(struct Plot *ptr,
                      float ox,float oy,float wdt,float hgt,
	              double ymin,double ymax,double step,
		      float offset,int txt_flg,int or,
                      int  (*text_box)(char *,float,int,char *,float *,void *),
                      void *boxdata,
                      char * (*make_text)(double,double,double,void *),
                      void *textdata,
                      char *fntname,float fntsze,
                      unsigned int color,unsigned char mask) {

  float py;
  double y;
  float txtbx[3];
  char *text=NULL;
  int s=0;

  if (ptr==NULL) return -1;
  if (make_text==NULL) return -1;
  if (text_box==NULL) return -1;
  if (fntname==NULL) return -1;
  if (step==0) return 0;

  for (y=ymin;y<ymax+(step/2);y+=step) {
     text=(*make_text)(y,ymin,ymax,textdata);
     if (text==NULL) continue;
     s=(*text_box)(fntname,fntsze,strlen(text),text,txtbx,boxdata);
     if (s !=0) break;
     py=hgt*(y-ymin)/(ymax-ymin);

     if (or==0) {
       if (txt_flg & 0x01) 
         s=PlotText(ptr,NULL,fntname,fntsze,ox-offset-txtbx[0],
                     oy+(txtbx[1]+txtbx[2])/2.0+hgt-py,strlen(text),text,
                     color,0x0f,1);
       if (s !=0) break;                     
       if (txt_flg & 0x02) 
         s=PlotText(ptr,NULL,fntname,fntsze,ox+wdt+offset,
                     oy+(txtbx[1]+txtbx[2])/2.0+hgt-py,strlen(text),text,
                     color,0x0f,1);
       if (s !=0) break;
     } else {
       struct PlotMatrix rm;
       rm.a=0;
       rm.b=1;
       rm.c=-1;
       rm.d=0;

       if (txt_flg & 0x01) 
         s=PlotText(ptr,&rm,fntname,fntsze,ox-offset+txtbx[1],
                     oy+hgt-py+txtbx[0]/2.0,strlen(text),text,
                     color,0x0f,1);
       if (s !=0) break;                     
       if (txt_flg & 0x02) 
         s=PlotText(ptr,&rm,fntname,fntsze,ox+wdt+offset+txtbx[2],
                     oy+hgt-py+txtbx[0]/2.0,strlen(text),text,
                     color,0x0f,1);
       if (s !=0) break;
     }

     free(text);
     text=NULL;
  }
  if (text !=NULL) free(text);
  return s;
}

int GrplotRawXaxisLabelValue(struct Plot *ptr,
                      float ox,float oy,float wdt,float hgt,
	              double xmin,double xmax,double value,
		      float offset,int txt_flg,int or,
                      int  (*text_box)(char *,float,int,char *,float *,void *),
                      void *boxdata,
                      int sze,char *text,
                      char *fntname,float fntsze,
                      unsigned int color,unsigned char mask) {
  int s=0;
  float px;
  float txtbx[3];

  if (ptr==NULL) return -1;
  if (text_box==NULL) return -1;
  if (fntname==NULL) return -1;
  if (text==NULL) return -1;

  s=(*text_box)(fntname,fntsze,sze,text,txtbx,boxdata);
  if (s !=0) return s;
  px=wdt*(value-xmin)/(xmax-xmin);

  if (or==0) {
    if (txt_flg & 0x01) 
      s=PlotText(ptr,NULL,fntname,fntsze,ox+px-txtbx[0]/2,
                   oy+hgt+offset+txtbx[2],sze,text,
                   color,0x0f,1);
     if (s !=0) return s;                     
     if (txt_flg & 0x02) 
     s=PlotText(ptr,NULL,fntname,fntsze,ox+px-txtbx[0]/2,
                   oy-offset+txtbx[1],sze,text,
                   color,0x0f,1);
   } else {
     struct PlotMatrix rm;
     rm.a=0;
     rm.b=1;
     rm.c=-1;
     rm.d=0;

     if (txt_flg & 0x01) 
       s=PlotText(ptr,&rm,fntname,fntsze,ox+px+(txtbx[2]+txtbx[1])/2.0,
                   oy+hgt+offset+txtbx[0],sze,text,
                   color,0x0f,1);
     if (s !=0) return s;                     
     if (txt_flg & 0x02) 
     s=PlotText(ptr,&rm,fntname,fntsze,ox+px+(txtbx[1]+txtbx[2])/2.0,
                 oy-offset,sze,text,
                 color,0x0f,1);
     if (s !=0) return s;
   }

   return s;
}


int GrplotRawYaxisLabelValue(struct Plot *ptr,
                      float ox,float oy,float wdt,float hgt,
	              double ymin,double ymax,double value,
		      float offset,int txt_flg,int or,
                      int  (*text_box)(char *,float,int,char *,float *,void *),
                      void *boxdata,
                      int sze,char *text,
                      char *fntname,float fntsze,
                      unsigned int color,unsigned char mask) {
  int s=0;
  float py;
  float txtbx[3];

  if (ptr==NULL) return -1;
  if (text_box==NULL) return -1;
  if (fntname==NULL) return -1;
  if (text==NULL) return -1;


   s=(*text_box)(fntname,fntsze,sze,text,txtbx,boxdata);
   if (s !=0) return s;
   py=hgt*(value-ymin)/(ymax-ymin);
   if (or==0) {
     if (txt_flg & 0x01) 
         s=PlotText(ptr,NULL,fntname,fntsze,ox-offset-txtbx[0],
                   oy+(txtbx[1]+txtbx[2])/2.0+hgt-py,sze,text,
                   color,0x0f,1);
     if (s !=0) return s;                     
     if (txt_flg & 0x02) 
         s=PlotText(ptr,NULL,fntname,fntsze,ox+wdt+offset,
                   oy+(txtbx[1]+txtbx[2])/2.0+hgt-py,sze,text,
                   color,0x0f,1);
   } else {
     struct PlotMatrix rm;
     rm.a=0;
     rm.b=1;
     rm.c=-1;
     rm.d=0;

     if (txt_flg & 0x01) 
         s=PlotText(ptr,&rm,fntname,fntsze,ox-offset+txtbx[1],
                     oy+hgt-py+txtbx[0]/2.0,sze,text,
                     color,0x0f,1);
     if (s !=0) return s;                     
     if (txt_flg & 0x02) 
         s=PlotText(ptr,&rm,fntname,fntsze,ox+wdt+offset+txtbx[2],
                     oy+hgt-py+txtbx[0]/2.0,sze,text,
                     color,0x0f,1);
     if (s !=0) return s;
   }
   return s;
}


int GrplotRawXaxisTitle(struct Plot *ptr,
                      float ox,float oy,float wdt,float hgt,
		      float offset,int txt_flg,int or,
                      int  (*text_box)(char *,float,int,char *,float *,void *),
                      void *boxdata,
                      int sze,char *text,
                      char *fntname,float fntsze,
                      unsigned int color,unsigned char mask) {
  
  int s=0;
  float txtbx[3];

  if (ptr==NULL) return -1;
  if (text_box==NULL) return -1;
  if (fntname==NULL) return -1;
  if (text==NULL) return -1;

   s=(*text_box)(fntname,fntsze,sze,text,txtbx,boxdata);
   if (s !=0) return s;
  
   if (or==0) {
     if (txt_flg & 0x01) {
       if (txt_flg & 0x04) 
       s=PlotText(ptr,NULL,fntname,fntsze,ox,
                   oy+hgt+offset+txtbx[2],sze,text,
                   color,0x0f,1);
       else if (txt_flg & 0x08) 
       s=PlotText(ptr,NULL,fntname,fntsze,ox+wdt-txtbx[0],
                   oy+hgt+offset+txtbx[2],sze,text,
                   color,0x0f,1);
       else 
       s=PlotText(ptr,NULL,fntname,fntsze,ox+(wdt-txtbx[0])/2.0,
                   oy+hgt+offset+txtbx[2],sze,text,
                   color,0x0f,1);


      if (s !=0) return s;                    
     }
     if (txt_flg & 0x02) {
       if (txt_flg & 0x04) 
       s=PlotText(ptr,NULL,fntname,fntsze,ox,
                   oy-offset+txtbx[1],sze,text,
                   color,0x0f,1);
       else if (txt_flg & 0x08) 
       s=PlotText(ptr,NULL,fntname,fntsze,ox+wdt-txtbx[0],
                   oy-offset+txtbx[1],sze,text,
                   color,0x0f,1);
       else 
       s=PlotText(ptr,NULL,fntname,fntsze,ox+(wdt-txtbx[0])/2.0,
                   oy-offset+txtbx[1],sze,text,
                   color,0x0f,1);
      if (s !=0) return s;                    
     }
   } else {
     struct PlotMatrix rm;
     rm.a=0;
     rm.b=1;
     rm.c=-1;
     rm.d=0;

     if (txt_flg & 0x01) {
       if (txt_flg & 0x04) 
       s=PlotText(ptr,&rm,fntname,fntsze,ox+txtbx[2],
                   oy+hgt+offset+txtbx[0],sze,text,
                   color,0x0f,1);
       else if (txt_flg & 0x08) 
       s=PlotText(ptr,&rm,fntname,fntsze,ox+wdt+txtbx[1],
                   oy+hgt+offset+txtbx[0],sze,text,
                   color,0x0f,1);
       else 
       s=PlotText(ptr,&rm,fntname,fntsze,ox+(wdt+txtbx[1]+txtbx[2])/2.0,
                   oy+hgt+offset+txtbx[0],sze,text,
                   color,0x0f,1);
      if (s !=0) return s;                    
     }

     if (txt_flg & 0x01) {
       if (txt_flg & 0x04) 
       s=PlotText(ptr,&rm,fntname,fntsze,ox+txtbx[2],
                   oy-offset,sze,text,
                   color,0x0f,1);
       else if (txt_flg & 0x08) 
       s=PlotText(ptr,&rm,fntname,fntsze,ox+wdt+txtbx[1],
                   oy-offset,sze,text,
                   color,0x0f,1);
       else 
       s=PlotText(ptr,&rm,fntname,fntsze,ox+(wdt+txtbx[1]+txtbx[2])/2.0,
                   oy-offset,sze,text,
                   color,0x0f,1);
      if (s !=0) return s;                    
     }
   } 
   return 0;
}


int GrplotRawYaxisTitle(struct Plot *ptr,
                      float ox,float oy,float wdt,float hgt,
		      float offset,int txt_flg,int or,
                      int  (*text_box)(char *,float,int,char *,float *,void *),
                      void *boxdata,
                      int sze,char *text,
                      char *fntname,float fntsze,
                      unsigned int color,unsigned char mask) {
  
  int s=0;
  float txtbx[3];

  if (ptr==NULL) return -1;
  if (text_box==NULL) return -1;
  if (fntname==NULL) return -1;
  if (text==NULL) return -1;

   s=(*text_box)(fntname,fntsze,sze,text,txtbx,boxdata);
   if (s !=0) return s;
  
   if (or==0) {
     if (txt_flg & 0x01) {
       if (txt_flg & 0x04) 
       s=PlotText(ptr,NULL,fntname,fntsze,ox-offset-txtbx[0],
                   oy+hgt+txtbx[1],sze,text,
                   color,0x0f,1);
       else if (txt_flg & 0x08) 
       s=PlotText(ptr,NULL,fntname,fntsze,ox-offset-txtbx[0],
                   oy+txtbx[2],sze,text,
                   color,0x0f,1);
       else 
       s=PlotText(ptr,NULL,fntname,fntsze,ox-offset-txtbx[0],
                   oy+(hgt+txtbx[1]+txtbx[2])/2.0,sze,text,
                   color,0x0f,1);
      if (s !=0) return s;                    
     }
     if (txt_flg & 0x02) {
      if (txt_flg & 0x04) 
       s=PlotText(ptr,NULL,fntname,fntsze,ox+wdt+offset,
                   oy+hgt+txtbx[1],sze,text,
                   color,0x0f,1);
       else if (txt_flg & 0x08) 
       s=PlotText(ptr,NULL,fntname,fntsze,ox+wdt+offset,
                   oy+txtbx[2],sze,text,
                   color,0x0f,1);
       else 
       s=PlotText(ptr,NULL,fntname,fntsze,ox+wdt+offset,
                   oy+(hgt+txtbx[1]+txtbx[2])/2.0,sze,text,
                   color,0x0f,1);
      if (s !=0) return s;    
     }
   } else {
     struct PlotMatrix rm;
     rm.a=0;
     rm.b=1;
     rm.c=-1;
     rm.d=0;

     if (txt_flg & 0x01) {
       if (txt_flg & 0x04) 
       s=PlotText(ptr,&rm,fntname,fntsze,ox-offset+txtbx[1],
                   oy+hgt,sze,text,
                   color,0x0f,1);
       else if (txt_flg & 0x08) 
       s=PlotText(ptr,&rm,fntname,fntsze,ox-offset+txtbx[1],
                   oy+txtbx[0],sze,text,
                   color,0x0f,1);
       else 
       s=PlotText(ptr,&rm,fntname,fntsze,ox-offset,
                   oy+(hgt+txtbx[0])/2.0,sze,text,
                   color,0x0f,1);
      if (s !=0) return s;                    
     }

     if (txt_flg & 0x02) {
       if (txt_flg & 0x04) 
       s=PlotText(ptr,&rm,fntname,fntsze,ox+wdt+offset+txtbx[2],
                   oy+hgt,sze,text,
                   color,0x0f,1);
       else if (txt_flg & 0x08) 
       s=PlotText(ptr,&rm,fntname,fntsze,ox+wdt+txtbx[2],
                   oy+txtbx[0],sze,text,
                   color,0x0f,1);
       else 
       s=PlotText(ptr,&rm,fntname,fntsze,ox+wdt+offset+txtbx[2],
                   oy+(hgt+txtbx[0])/2.0,sze,text,
                   color,0x0f,1);
      if (s !=0) return s;                    
     }
   } 
   return 0;
}


int GrplotRawFitImage(struct Plot *ptr,
                       float ox,float oy,float wdt,float hgt,
		       struct FrameBuffer *img,
                       unsigned char mask) {

  struct PlotMatrix rm;
  if (ptr==NULL) return -1;
  if (img==NULL) return -1;
  if (img->wdt==0) return -1;
  if (img->hgt==0) return -1;

  rm.a=wdt/img->wdt;
  rm.b=0;
  rm.c=0;
  rm.d=hgt/img->hgt;

  return PlotImage(ptr,&rm,img,mask,ox,oy,1);
}

int GrplotRawFitImageName(struct Plot *ptr,
                     float ox,float oy,float wdt,float hgt,
		     char *name,int iwdt,int ihgt,int depth,
                     unsigned char mask) {

  struct PlotMatrix rm;
  if (ptr==NULL) return -1;
  if (iwdt==0) return -1;
  if (ihgt==0) return -1;

  rm.a=wdt/iwdt;
  rm.b=0;
  rm.c=0;
  rm.d=hgt/ihgt;

  return PlotImageName(ptr,&rm,name,iwdt,ihgt,depth,mask,ox,oy,1);
}

int GrplotRawLine(struct Plot *ptr,
                float ox,float oy,float wdt,float hgt,
                double ax,double ay,double bx,double by,
		double xmin,double xmax,
                double ymin,double ymax,
                unsigned int color,unsigned char mask,float width,
                struct PlotDash *dash) {

  float fax,fay,fbx,fby; 
 
  fax=ox+wdt*(ax-xmin)/(xmax-xmin);
  fbx=ox+wdt*(bx-xmin)/(xmax-xmin);
  fay=oy+hgt-hgt*(ay-ymin)/(ymax-ymin);
  fby=oy+hgt-hgt*(by-ymin)/(ymax-ymin);

  return PlotLine(ptr,fax,fay,fbx,fby,color,mask,width,dash);
}


int GrplotRawRectangle(struct Plot *ptr,
                struct PlotMatrix *matrix,
                float ox,float oy,float wdt,float hgt,
                double x,double y,double w,double h,
		double xmin,double xmax,
                double ymin,double ymax,
                int fill,unsigned int color,unsigned char mask,float width,
                struct PlotDash *dash) {

  float fx,fy,fw,fh;
 
  fx=ox+wdt*(x-xmin)/(xmax-xmin);
  fw=wdt*w/(xmax-xmin);
  fy=oy+hgt-hgt*(y-ymin)/(ymax-ymin);
  fh=hgt*h/(ymax-ymin);

  return PlotRectangle(ptr,matrix,fx,fy,fw,fh,fill,color,mask,width,dash);
}


int GrplotRawEllipse(struct Plot *ptr,
                struct PlotMatrix *matrix,
                float ox,float oy,float wdt,float hgt,
                double x,double y,double w,double h,
		double xmin,double xmax,
                double ymin,double ymax,
                int fill,unsigned int color,unsigned char mask,float width,
                struct PlotDash *dash) {

  float fx,fy,fw,fh;
 
  fx=ox+wdt*(x-xmin)/(xmax-xmin);
  fw=wdt*w/(xmax-xmin);
  fy=oy+hgt-hgt*(y-ymin)/(ymax-ymin);
  fh=hgt*h/(ymax-ymin);

  return PlotEllipse(ptr,matrix,fx,fy,fw,fh,fill,color,mask,width,dash);

}


int GrplotRawPolygon(struct Plot *ptr,
                struct PlotMatrix *matrix,
                float ox,float oy,float wdt,float hgt,
		 double x,double y,int num,double *px,double *py,
                int *t,
		double xmin,double xmax,
                double ymin,double ymax,
                int fill,unsigned int color,unsigned char mask,float width,
                struct PlotDash *dash) {

  int s=0;
  float fx,fy;
  float *fpx,*fpy;
  int i;
  
  fpx=malloc(sizeof(float)*num);
  if (fpx==NULL) return -1;
  fpy=malloc(sizeof(float)*num);
  if (fpy==NULL) {
    free(fpx);
    return -1;
  }
 
  fx=ox+wdt*(x-xmin)/(xmax-xmin);
  fy=oy+hgt-hgt*(y-ymin)/(ymax-ymin);


  for (i=0;i<num;i++) {
    fpx[i]=wdt*px[i]/(xmax-xmin);
    fpy[i]=hgt*py[i]/(ymax-ymin);
  }

  s=PlotPolygon(ptr,matrix,fx,fy,num,fpx,fpy,t,fill,
                           color,mask,width,dash);
 
  free(fpx);
  free(fpy);
  return s;
}


int GrplotRawBezier(struct Plot *ptr,
                float ox,float oy,float wdt,float hgt,
                double ax,double ay,double bx,double by,
	        double cx,double cy,double dx,double dy,
                double xmin,double xmax,
                double ymin,double ymax,
                unsigned int color,unsigned char mask,float width,
                struct PlotDash *dash) {

 
  float fax,fay,fbx,fby,fcx,fcy,fdx,fdy;

  fax=ox+wdt*(ax-xmin)/(xmax-xmin);
  fbx=ox+wdt*(bx-xmin)/(xmax-xmin);
  fcx=ox+wdt*(cx-xmin)/(xmax-xmin);
  fdx=ox+wdt*(dx-xmin)/(xmax-xmin);

  fay=oy+hgt-hgt*(ay-ymin)/(ymax-ymin);
  fby=oy+hgt-hgt*(by-ymin)/(ymax-ymin);
  fcy=oy+hgt-hgt*(cy-ymin)/(ymax-ymin);
  fdy=oy+hgt-hgt*(dy-ymin)/(ymax-ymin);

  return PlotBezier(ptr,fax,fay,fbx,fby,fcx,fcy,fdx,fdy,
                        color,mask,width,dash);
}

int GrplotRawText(struct Plot *ptr,
                struct PlotMatrix *matrix,
                float ox,float oy,float wdt,float hgt,
                char *fontname,float fontsize,
                double x,double y,int num,char *txt,
                float dx,float dy,
		double xmin,double xmax,
                double ymin,double ymax,
                unsigned int color,unsigned char mask) {

  float fx,fy;
 
  fx=ox+dx+wdt*(x-xmin)/(xmax-xmin);
  fy=oy-dy+hgt-hgt*(y-ymin)/(ymax-ymin);

  return PlotText(ptr,matrix,fontname,fontsize,fx,fy,num,txt,
                        color,mask,1);
}


int GrplotRawImage(struct Plot *ptr,
                struct PlotMatrix *matrix,
                float ox,float oy,float wdt,float hgt,
                struct FrameBuffer *img,
		float dx,float dy,
	        double x,double y,
		double xmin,double xmax,
                double ymin,double ymax,
                unsigned char mask) {

  float fx,fy; 

  fx=ox+dx+wdt*(x-xmin)/(xmax-xmin);
  fy=oy-dy+hgt-hgt*(y-ymin)/(ymax-ymin);
  return PlotImage(ptr,matrix,img,mask,fx,fy,1);
}


int GrplotRawImageName(struct Plot *ptr,
                     struct PlotMatrix *matrix,
                     float ox,float oy,float wdt,float hgt,
		     char *name,int iwdt,int ihgt,int depth,
                     float dx,float dy,
                     double x,double y,
		     double xmin,double xmax,
                     double ymin,double ymax,
                     unsigned char mask) {

  float fx,fy; 

  fx=ox+dx+wdt*(x-xmin)/(xmax-xmin);
  fy=oy-dy+hgt-hgt*(y-ymin)/(ymax-ymin);

  return PlotImageName(ptr,matrix,name,iwdt,ihgt,depth,mask,fx,fy,1);

}



int GrplotRawFill(struct Plot *ptr,
                float ox,float oy,float wdt,float hgt,
                double ax,double ay,double bx,double by,
		double xmin,double xmax,
                double ymin,double ymax,int or,
                unsigned int color,unsigned char mask) {

  float px[4],py[4];
  int t[4]={0,0,0,0};

  float fax,fay,fbx,fby;
 
  fax=ox+wdt*(ax-xmin)/(xmax-xmin);
  fbx=ox+wdt*(bx-xmin)/(xmax-xmin);
  fay=oy+hgt-hgt*(ay-ymin)/(ymax-ymin);
  fby=oy+hgt-hgt*(by-ymin)/(ymax-ymin);
  
  if ((or==3) || (or==4)) {
    px[0]=fax;
    px[1]=fax;
    px[2]=fbx;
    px[3]=fbx;
    py[1]=fay;
    py[2]=fby;
    if (or==3) py[0]=py[3]=oy+hgt;
    else py[0]=py[3]=oy;
  } else {
    py[0]=fay;
    py[1]=fay;
    py[2]=fby;
    py[3]=fby;
    px[1]=fax;
    px[2]=fbx;
    if (or==1) px[0]=px[3]=ox;
    else px[0]=px[3]=ox+wdt;
  }
  return PlotPolygon(ptr,NULL,0,0,4,px,py,t,1,color,mask,0,NULL);



}

int GrplotRawBar(struct Plot *ptr,
                float ox,float oy,float wdt,float hgt,
	        double ax,double ay,double w,double o,
		double xmin,double xmax,
	        double ymin,double ymax,int or,int fill,
                unsigned int color,unsigned char mask,float width,
                struct PlotDash *dash) {

 
  int i;
  float px[4],py[4];
  int t[4]={0,0,0,0};

  if ((or==3) || (or==4)) {
     px[0]=wdt*(ax+o-w-xmin)/(xmax-xmin);
     px[1]=wdt*(ax+o-w-xmin)/(xmax-xmin);
     px[2]=wdt*(ax+o+w-xmin)/(xmax-xmin);
     px[3]=wdt*(ax+o+w-xmin)/(xmax-xmin);
     py[1]=hgt*(ay-ymin)/(ymax-ymin);
     py[2]=hgt*(ay-ymin)/(ymax-ymin);
     if (or==3) py[0]=py[3]=0;
     else py[0]=py[3]=hgt;
  } else {
     py[0]=hgt*(ay-o-w-ymin)/(ymax-ymin);
     py[1]=hgt*(ay-o-w-ymin)/(ymax-ymin);
     py[2]=hgt*(ay-o+w-ymin)/(ymax-ymin);
     py[3]=hgt*(ay-o+w-ymin)/(ymax-ymin);
     px[1]=wdt*(ax-xmin)/(xmax-xmin);
     px[2]=wdt*(ax-xmin)/(xmax-xmin);
     if (or==1) px[0]=px[3]=0;
     else px[0]=px[3]=wdt;
  }
  
  for (i=0;i<4;i++) {
    if (px[i]<0) px[i]=0;
    if (py[i]<0) py[i]=0;
    if (px[i]>wdt) px[i]=wdt;
    if (py[i]>hgt) py[i]=hgt;
    px[i]+=ox;
    py[i]=oy+hgt-py[i];
  }
  return PlotPolygon(ptr,NULL,0,0,4,px,py,t,fill,color,mask,width,dash);
}








