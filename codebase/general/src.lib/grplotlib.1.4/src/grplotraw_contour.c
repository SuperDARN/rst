/* grplotraw_contour.c
   =================== 
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
#include "rmath.h"
#include "rfbuffer.h"
#include "iplot.h"
#include "polygon.h"




int GrplotRawContour(struct Plot *plot,struct PolygonData *ptr,
                  float xoff,float yoff,float wdt,float hgt,
                  float step,float smooth,float lwdt,
                  int lstart,int lstep,
                  int sze,char *text,
                  unsigned int color,char mask,float width,
                  struct PlotDash *dash,
                  char *fntname,float fntsze,
                  int  (*text_box)(char *,float,int,char *,float *,void *),
                         void *boxdata) {

  struct PlotMatrix m;

  float txtbx[3];
 
  float x,y,u,v;
  float x0=0,y0=0;
  float x1,y1;
  float x2,y2;
  float x3,y3;
 
  int i,k,n,pn; 
  int lflg=0;
  float ang;
  float tang;
  float twdt;
  float tx,ty;
  float tw,th;
  float lw;
  int s=0;

  if (ptr==NULL) return -1;
  if (ptr->pntnum==0) return -1;
  if (fntname==NULL) return -1;
 
  if (text !=NULL) {
    if (text_box==NULL) return -1;
    s=(*text_box)(fntname,fntsze,sze,text,txtbx,boxdata);
    if (s !=0) return s;
  }
 
  if (step<0.01) step=0.01;
  if (step>0.5) step=0.5;
  if (smooth<0.1) smooth=0.1;
  

  for (n=0;n<ptr->polnum;n++) {
    pn=ptr->num[n];
    if (ptr->type[n] !=1) pn--;
    if (pn<2) continue;
    for (i=0;i<pn;i++) {

      lflg=((lstep !=0) && (text !=NULL) && 
	  (i-lstart) > 0 && (((i-lstart) % lstep)==0));

    
      x=((float *)((char *) ptr->data+ptr->off[n]*ptr->sze))[2*i]*wdt+xoff;
      y=((float *)((char *) ptr->data+ptr->off[n]*ptr->sze))[2*i+1]*hgt+yoff;
          
   
      k=(i<(ptr->num[n]-1)) ? (i+1) : 0;
  
      u=((float *)((char *) ptr->data+ptr->off[n]*ptr->sze))[2*k]*wdt+xoff;
      v=((float *)((char *) ptr->data+ptr->off[n]*ptr->sze))[2*k+1]*hgt+yoff;
   
      if (i==0) {    
        x0=(x+u)/2;
        y0=(y+v)/2;   
      } else  {
        if (lflg==0) {
          x3=(x+u)/2;
          y3=(y+v)/2;
          x1=x0+(x-x0)*smooth;
          y1=y0+(y-y0)*smooth;
          x2=x3+(x-x3)*smooth;
          y2=y3+(y-y3)*smooth;
          PlotBezier(plot,x0,y0,x1,y1,x2,y2,x3,y3,color,mask,width,dash);
          x0=x3;
          y0=y3;
        } else { 
          ang=atan2(v-y,u-x);
          tang=ang;
          if (tang>(0.5*PI)) tang=tang-PI;
          if (tang<(-0.5*PI)) tang=tang+PI;
          twdt=sqrt((v-y)*(v-y)+(u-x)*(u-x));    
          if (twdt<lwdt) lw=twdt;
	  else lw=lwdt; 
          x3=(x+u-twdt*cos(ang))/2;
          y3=(y+v-twdt*sin(ang))/2;
          x1=x0+(x-x0)*smooth;
          y1=y0+(y-y0)*smooth;
          x2=x3+(x-x3)*smooth;
          y2=y3+(y-y3)*smooth;
          PlotBezier(plot,x0,y0,x1,y1,x2,y2,x3,y3,color,mask,width,dash);
          x0=x3+lw*cos(ang);
          y0=y3+lw*sin(ang);
	  
          tx=(x+u)/2.0;
          ty=(y+v)/2.0;
          tw=txtbx[0]/2.0;
          th=(txtbx[1]+txtbx[2])/2;
          m.a=cos(tang);
          m.b=-sin(tang);
          m.c=sin(tang);
          m.d=cos(tang);

          PlotText(plot,&m,fntname,fntsze,
                    tx-tw*cos(tang)-th*sin(tang),
                    ty-tw*sin(tang)+th*cos(tang),
		    sze,text,color,0x0f,1);
	  /* draw_text(txt,tang*180/PI);*/
	 
	          
        }
      }
    }
  

    /* complete the segment */

    if (ptr->type[n]==1) { /* closed contour */

      x=((float *)((char *) ptr->data+ptr->off[n]*ptr->sze))[0]*wdt+xoff;
      y=((float *)((char *) ptr->data+ptr->off[n]*ptr->sze))[1]*hgt+yoff;
 
      u=((float *)((char *) ptr->data+ptr->off[n]*ptr->sze))[2]*wdt+xoff;
      v=((float *)((char *) ptr->data+ptr->off[n]*ptr->sze))[3]*hgt+yoff;
 
      x3=(x+u)/2;
      y3=(y+v)/2;
       
      x1=x0+(x-x0)*smooth;
      y1=y0+(y-y0)*smooth;
      x2=x3+(x-x3)*smooth;
      y2=y3+(y-y3)*smooth;
      PlotBezier(plot,x0,y0,x1,y1,x2,y2,x3,y3,color,mask,width,dash);
    }
  }
  return 0;
}




int GrplotRawContourPolygon(struct Plot *plot,struct PolygonData *ptr,
                  float xoff,float yoff,float wdt,float hgt,
                  float step,float smooth,
		       unsigned int color,char mask,int fill,
                  float width,
                  struct PlotDash *dash) {

 
  float x,y,u,v;
  float x0=0,y0=0;
  float x1,y1;
  float x2,y2;
  float x3,y3;
  
  float *px=NULL,*py=NULL;
  int *t=NULL;
  void *tmp;
  
  int i,k,n,pn; 
  int nn;
  if (ptr==NULL) return -1;
  if (ptr->pntnum==0) return -1;
  if (step<0.01) step=0.01;
  if (step>0.5) step=0.5;
  if (smooth<0.1) smooth=0.1;
  

  for (n=0;n<ptr->polnum;n++) {
    pn=ptr->num[n];
    if (ptr->type[n] !=1) continue; /* open - can't fill */
    if (pn<2) continue;
    nn=0;
    
     if (px==NULL) px=malloc(sizeof(float)*(3*pn+1));
     else {
       tmp=realloc(px,sizeof(float)*(3*pn+1));
       if (tmp==NULL) free(px);
       px=tmp;
     }
     if (px==NULL) return -1;

     if (py==NULL) py=malloc(sizeof(float)*(3*pn+1));
     else {
       tmp=realloc(py,sizeof(float)*(3*pn+1));
       if (tmp==NULL) free(py);
       py=tmp;
     }
     if (py==NULL) return -1;

     if (t==NULL) t=malloc(sizeof(float)*(3*pn+1));
     else {
       tmp=realloc(t,sizeof(float)*(3*pn+1));
       if (tmp==NULL) free(t);
       t=tmp;
     }
     if (t==NULL) return -1;


    
    for (i=0;i<pn;i++) {
    
      x=((float *)((char *) ptr->data+ptr->off[n]*ptr->sze))[2*i]*wdt+xoff;
      y=((float *)((char *) ptr->data+ptr->off[n]*ptr->sze))[2*i+1]*hgt+yoff;
          
   
      k=(i<(ptr->num[n]-1)) ? (i+1) : 0;
  
      u=((float *)((char *) ptr->data+ptr->off[n]*ptr->sze))[2*k]*wdt+xoff;
      v=((float *)((char *) ptr->data+ptr->off[n]*ptr->sze))[2*k+1]*hgt+yoff;
   
      if (i==0) {    
        x0=(x+u)/2;
        y0=(y+v)/2;   
        px[nn]=x0;
        py[nn]=y0;
        t[nn]=0;
        nn++;
      } else  {
        x3=(x+u)/2;
        y3=(y+v)/2;
        x1=x0+(x-x0)*smooth;
        y1=y0+(y-y0)*smooth;
        x2=x3+(x-x3)*smooth;
        y2=y3+(y-y3)*smooth;
        px[nn]=x1;
        py[nn]=y1;
        t[nn]=2;
        px[nn+1]=x2;
        py[nn+1]=y2;
        t[nn+1]=2;
        px[nn+2]=x3;
        py[nn+2]=y3;
        t[nn+2]=2;
        nn+=3;
        x0=x3;
        y0=y3;
      }
    }
  
    /* complete the segment */

    if (ptr->type[n]==1) { /* closed contour */

      x=((float *)((char *) ptr->data+ptr->off[n]*ptr->sze))[0]*wdt+xoff;
      y=((float *)((char *) ptr->data+ptr->off[n]*ptr->sze))[1]*hgt+yoff;
 
      u=((float *)((char *) ptr->data+ptr->off[n]*ptr->sze))[2]*wdt+xoff;
      v=((float *)((char *) ptr->data+ptr->off[n]*ptr->sze))[3]*hgt+yoff;
 
      x3=(x+u)/2;
      y3=(y+v)/2;       
      x1=x0+(x-x0)*smooth;
      y1=y0+(y-y0)*smooth;
      x2=x3+(x-x3)*smooth;
      y2=y3+(y-y3)*smooth;

      px[nn]=x1;
      py[nn]=y1;
      t[nn]=2;
      px[nn+1]=x2;
      py[nn+1]=y2;
      t[nn+1]=2;
      px[nn+2]=x3;
      py[nn+2]=y3;
      t[nn+2]=2;
      nn+=3;
    }
    PlotPolygon(plot,NULL,0,0,nn,px,py,t,fill,color,mask,width,dash);
  }

  if (px !=NULL) free(px);
  if (py !=NULL) free(py);
  if (t !=NULL) free(t);


  return 0;
}














