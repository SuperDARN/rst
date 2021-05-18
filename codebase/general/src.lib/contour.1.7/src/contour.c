/* contour.c
   =========
   Author: R.J.Barnes
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "rmath.h"
#include "rfbuffer.h"
#include "polygon.h"
#include "iplot.h"
#include "rmap.h"
#include "contour.h"



void ContourPoint(int x,int y,int icept,double ipnt,
		   float *cx,float *cy) {
   double ix,iy;
  ix=(icept==1)+(icept==0)*ipnt+(1-ipnt)*(icept==2);
  iy=(icept==2)+(icept==1)*ipnt+(1-ipnt)*(icept==3);    
  *cx=x+ix;
  *cy=y+iy;
}

struct PolygonData **Contour(void *zbuffer,int type,
			    int wdt,int hgt,int cnum,
		            void *cval,int smooth,int cflag,int rflag,
		            int bflag) {

  int x,y;
  int test=0;
  int c;
 
  int xwrap=0;
  int ywrap=0;
  int tick=0;

  float p[2];
  float px=0,py=0;
  double ipnt=0;
  int cx=0,cy=0,dx=0,dy=0,vx=0,vy=0;
  int isrch,icept,icalt;
  int trace=0;
  int bflg;  

  struct PolygonData **ptr=NULL;
  unsigned char *cmap=NULL;
  double pa=0,pb=0;

  double *zdouble=NULL;
  float *zfloat=NULL;
  int *zint=NULL;

  double *cvdouble=NULL;
  float *cvfloat=NULL;
  int *cvint=NULL;

  /* The array wdtxhgt contains (wdt-1)*(hgt-1) cells.
   * If wraparound occurs then it contains wdtxhgt cells 
   * The algorithm searches on a cell by cell basis.   
   *
   */     

  

  if (type==contour_DOUBLE) zdouble=(double *) zbuffer;
  else if (type==contour_FLOAT) zfloat=(float *) zbuffer;
  else zint=(int *) zbuffer;

  if (type==contour_DOUBLE) cvdouble=(double *) cval;
  else if (type==contour_FLOAT) cvfloat=(float *) cval;
  else cvint=(int *) cval;



  if (rflag & 0x01) xwrap=1;
  if (rflag & 0x02) ywrap=1;

  ptr=malloc(sizeof(struct PolygonData *)*cnum);
  if (ptr==NULL) return NULL;

  cmap=malloc(wdt*hgt);
 
  memset(cmap,0,wdt*hgt);

  /* search for a cell that crosses a contour line */

  for (c=0;c<cnum;c++) {
    
    ptr[c]=PolygonMake(sizeof(float)*2,NULL);
    tick=0;    

  
    for (x=0;x<wdt-1+xwrap;x++) {
      for (y=0;y<hgt-1+ywrap;y++) {
        
        /* ignore this cell if we've already searched it */


        if (cmap[y*wdt+x]==(c+1)) continue;

        /* check the top edge */
 
       if (zint !=NULL) pa=zint[y*wdt+x];
       if (zfloat !=NULL) pa=zfloat[y*wdt+x];
       if (zdouble !=NULL) pa=zdouble[y*wdt+x];

       if (pa==1e31) continue;
       cx=x+1;
       if (cx==wdt) cx=0; /* wrap around */

       if (zint !=NULL) pb=zint[y*wdt+cx];
       if (zfloat !=NULL) pb=zfloat[y*wdt+cx];
       if (zdouble !=NULL) pb=zdouble[y*wdt+cx];

       if (pb==1e31) continue;
       if (cvdouble !=NULL)
         test=((pa<=cvdouble[c]) && (pb>cvdouble[c])) || ((pb<=cvdouble[c]) && 
               (pa>cvdouble[c]));
       if (cvfloat !=NULL)
         test=((pa<=cvfloat[c]) && (pb>cvfloat[c])) || ((pb<=cvfloat[c]) && 
               (pa>cvfloat[c]));
       if (cvint !=NULL)
         test=((pa<=cvint[c]) && (pb>cvint[c])) || ((pb<=cvint[c]) && 
               (pa>cvint[c]));



       if (test==0) { /* check left edge */
         cy=y+1;
         if (cy==hgt) cy=0; /* wrap around */
         if (zint !=NULL) pb=zint[y*wdt+x];
         if (zfloat !=NULL) pb=zfloat[y*wdt+x];
         if (zdouble !=NULL) pb=zdouble[y*wdt+x];

         if (pb==1e31) continue;


         if (cvdouble !=NULL) test=((pa<=cvdouble[c]) && (pb>cvdouble[c])) || 
              ((pb<=cvdouble[c]) && (pa>cvdouble[c]));
         if (cvfloat !=NULL) test=((pa<=cvfloat[c]) && (pb>cvfloat[c])) || 
              ((pb<=cvfloat[c]) && (pa>cvfloat[c]));
         if (cvint !=NULL) test=((pa<=cvint[c]) && (pb>cvint[c])) || 
              ((pb<=cvint[c]) && (pa>cvint[c]));

       }

       if (test==0)  continue;
 
       /* okay start the contour trace */
       tick=0;
       trace=2;
       icept=-1;
       icalt=-1;
       dx=0; /* cell offset 0<x+dx<(wdt-1) or for wraparound 0<dx<wdt */
       dy=0;

       while (trace>0) {
         cmap[(y+dy)*wdt+(x+dx)]=c+1;
         cx=x+dx;
         cy=y+dy;
         if (cx==wdt) cx=0;
         if (cy==hgt) cy=0;
         if (zint !=NULL) pa=zint[cy*wdt+cx];
         if (zfloat !=NULL) pa=zfloat[cy*wdt+cx];
         if (zdouble !=NULL) pa=zdouble[cy*wdt+cx];   
         for (isrch=0;isrch<4;isrch++) {
	   /* find the intersection between the contour and the current cell */
           vx=(isrch<2);
           vy=((isrch>0) && (isrch<3));
           cx=x+dx+vx;
           cy=y+dy+vy;
           if (cx==wdt) cx=0;
           if (cy==hgt) cy=0;

           if (zint !=NULL) pb=zint[cy*wdt+cx];
           if (zfloat !=NULL) pb=zfloat[cy*wdt+cx];
           if (zdouble !=NULL) pb=zdouble[cy*wdt+cx];

           
           if (cvdouble !=NULL) test=((pb != 1e31) && (pa !=1e31)) &&
                (((pa<=cvdouble[c]) && (pb>cvdouble[c])) || 
                ((pb<=cvdouble[c]) && (pa>cvdouble[c])));
           if (cvfloat !=NULL) test=((pb != 1e31) && (pa !=1e31)) &&
                (((pa<=cvfloat[c]) && (pb>cvfloat[c])) || 
                ((pb<=cvfloat[c]) && (pa>cvfloat[c])));
           if (cvint !=NULL) test=((pb != 1e31) && (pa !=1e31)) &&
                (((pa<=cvint[c]) && (pb>cvint[c])) || 
                ((pb<=cvint[c]) && (pa>cvint[c])));




           if ((icept !=isrch) && (test==1)) break;
           pa=pb;
         
	 }
          
         if (isrch==4) { 
       
            /* cant escape from the cell so try other way */
            trace=trace-1;
            if (cflag !=0) PolygonRemove(ptr[c]);
	    tick=0;
            dx=0;
	    dy=0;
            icept=icalt;
            continue;
	 }
         
         vx=(isrch % 2)-2*(isrch==3);
         vy=((isrch+1) % 2)-2*(isrch==0);

         /* mark the edge that the contour enters the cell 
          * so that we don't try and leave on it and get caught in a loop */

          icept=(isrch+2) % 4; 

          if (cvdouble !=NULL) ipnt=(cvdouble[c]-pa)/(pb-pa);
          if (cvfloat !=NULL) ipnt=(cvfloat[c]-pa)/(pb-pa);
          if (cvint !=NULL) ipnt=(cvint[c]-pa)/(pb-pa);

          ContourPoint(x+dx,y+dy,isrch,ipnt,&px,&py);
          if (tick==0) {
	   /* first point in a contour so start the point table */    
            icalt=isrch;
            p[0]=px/wdt;
            p[1]=py/hgt;
            PolygonAddPolygon(ptr[c],1);
            PolygonAdd(ptr[c],p);
          } else if ((tick % smooth)==0) {
            int fflg=0;
            if ((bflag & 0x04) && ((y+dy)>=(hgt-1))) fflg=1;
            if ((bflag & 0x08) && ((y+dy)<=0)) fflg=1;
            if ((bflag & 0x02) && ((x+dx)>=(wdt-1))) fflg=1;
            if ((bflag & 0x01) && ((x+dx)<=0)) fflg=1;
            if (fflg==0) {
              p[0]=px/wdt;
              p[1]=py/hgt;
              PolygonAdd(ptr[c],p);
            }
          }
          tick++;
          dx+=vx;
          dy+=vy;
          bflg=0;         

        
	  if (xwrap==1) {
            if ((x+dx)>=wdt) dx=-x;
            if ((x+dx)<0) dx=wdt-1-x;
	  } else {
            if ((x+dx)>=wdt) bflg=1;
            if ((x+dx)<0) bflg=1;
	  } 

	  if (ywrap==1) {
            if ((y+dy)<0) dy=hgt-1-y;
            if ((y+dy)>=hgt) dy=-y;
	  } else {
            if ((y+dy)>=hgt) bflg=1;
            if ((y+dy)<0) bflg=1;
	  }

          if ((cmap[(y+dy)*wdt+(x+dx)]==(c+1)) && (dx==0) && (dy==0)) {
           bflg=0;
           trace=0;
           if ((ptr[c]->polnum>0) && (ptr[c]->num[ptr[c]->polnum-1]<3)) {
              PolygonRemove(ptr[c]); /* short contour */
              tick=0;
              continue;
	    }
	  } else if (cmap[(y+dy)*wdt+(x+dx)]==(c+1)) bflg=1;

          if (bflg !=0) {
           dx=0;
           dy=0;
           icept=icalt;
           trace=trace-1;
           if (cflag !=0) PolygonRemove(ptr[c]);
           tick=0;
           continue;
	 }

       }
     } 
    }
  }
      
  free(cmap);
  return ptr;
}


struct PolygonData **ContourTransform(int cnum,struct PolygonData **src,
                                      int (*trf)(int ssze,void *src,int dsze,
                                                 void *dst,void *data),
                                      void *data) {

  int i=0;
  struct PolygonData **dst=NULL;
  if (src==NULL) return NULL;
  dst=malloc(sizeof(struct PolygonData *)*cnum);
  for (i=0;i<cnum;i++) {
    dst[i]=NULL;
    if (src[i]==NULL) continue;
    dst[i]=MapTransform(src[i],sizeof(float)*2,NULL,trf,data);
  }
  return dst;
} 

void ContourModify(int cnum,struct PolygonData **src,
                                      int (*trf)(int ssze,void *src,int dsze,
                                                 void *dst,void *data),
                                      void *data) {
  int i=0;
  if (src==NULL) return;
  for (i=0;i<cnum;i++) {
    
    if (src[i] !=NULL) MapModify(src[i],trf,data);
    
  }
  
} 


void ContourFree(int cnum,struct PolygonData **ptr) {
  int i=0;
  if (ptr==NULL) return;
  for (i=0;i<cnum;i++) {
    
    if (ptr[i] !=NULL) PolygonFree(ptr[i]);
    
  }
  free(ptr);
} 
