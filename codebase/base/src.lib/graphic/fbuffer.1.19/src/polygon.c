/* polygon.c
   ========= 
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rfbuffer.h"



int FrameBufferPcmp(const void *a,const void  *b) {
  int x,y;
  x=*((int *) a);
  y=*((int *) b);
  if (x>y) return 1;  
  if (x<y) return -1;
  return 0; 
}

int FrameBufferPIsect(int hgt,int ax,int ay,int bx,int by,int *in,int **ix) {
  int *tmp;
  int yb,yt,ys;
  int ib,it,il,ir;
  int i;
  if (ay<by) {
    ib=ay;
    it=by;
    il=ax;
    ir=bx;
  } else {
    ib=by;
    it=ay;
    il=bx;
    ir=ax;
  }

  yb=ib;
  yt=it;

  if ((yb<0) && (yt<0)) return 0;
  if ((yb>=hgt) && (yt>=hgt)) return 0;

  if (yb<0) yb=0;
  if (yt>hgt) yt=hgt;

  if (yb != yt) {
    for (ys=yb;ys<yt;ys++) {
      i=in[ys];
      if (ix[ys]==NULL) ix[ys]=malloc(sizeof(int));
      else {
        tmp=realloc(ix[ys],sizeof(int)*(i+1));
        if (tmp==NULL) break;
        ix[ys]=tmp;
      }
      if (ix[ys]==NULL) break;
      ix[ys][i]=il+((ir-il)*(ys-ib))/(it-ib);
      in[ys]=in[ys]+1;
    }
    if (ys !=yt) return -1;
    
  } 
  return 0;
}

int FrameBufferPolygon(struct FrameBuffer *ptr,
                       struct FrameBufferMatrix *matrix,
                       int x,int y,
                       int num,int *px,int *py,int *t,
                       int fill,unsigned int color,
                       unsigned char m,int width,
                       struct FrameBufferDash *dash,
                       struct FrameBufferClip *clip) {

  int s=0;
  int sze;
  int i,n;
  unsigned char r,g,b,a;
  int flag,rs;


  int ai,bi,ci,di;

  int **ix=NULL;
  int *in=NULL;

  if (ptr==NULL) return -1;

  sze=ptr->wdt*ptr->hgt;
  a=color>>24;;
  r=color>>16;
  g=color>>8;
  b=color;


  if (fill==1) {  
    in=malloc(sizeof(int)*ptr->hgt);
    if (in==NULL) return -1;

    ix=malloc(sizeof(int *)*ptr->hgt);
    if (ix==NULL) {
      free(in);
      return -1;
    }

    for (i=0;i<ptr->hgt;i++) ix[i]=NULL;
    for (i=0;i<ptr->hgt;i++) in[i]=0;
  }
  n=1;
  while (n<=num) {
    ai=(n==num) ? 0 : n;
   
    if (t[ai]==1) { /* bezier */
      int x1,x2,x3,x4; 
      int y1,y2,y3,y4;
      float ax,bx,cx;
      float ay,by,cy;
      float step=0.01;
      float t;

      int ox,oy,nx,ny;

      n+=2;
      if (n>num) break;
      ai=(n==num) ? 0 : n;
      bi=n-1;
      ci=n-2;
      di=n-3;
 
      x1=px[di];
      x2=px[ci];
      x3=px[bi];
      x4=px[ai];

      y1=py[di];
      y2=py[ci];
      y3=py[bi];
      y4=py[ai];

      if (matrix !=NULL) {
        FrameBufferTransform(matrix,x1,y1,&x1,&y1);
        FrameBufferTransform(matrix,x2,y2,&x2,&y2);
        FrameBufferTransform(matrix,x3,y3,&x3,&y3);
        FrameBufferTransform(matrix,x4,y4,&x4,&y4);
      }

      x1=x1+x;
      y1=y1+y;
      x2=x2+x;
      y2=y2+y;
      x3=x3+x;
      y3=y3+y;
      x4=x4+x;
      y4=y4+y;

       
      cx=(x2-x1)*3.0;  
      bx=(x3-x2)*3.0-cx;
      ax=x4-x1-bx-cx;
  
      cy=(y2-y1)*3.0;  
      by=(y3-y2)*3.0-cy;
      ay=y4-y1-by-cy;

      oy=y1;
      ox=x1;
      for (t=step;t<1;t+=step) {
        nx=ax*t*t*t+bx*t*t+cx*t+x1;
        ny=ay*t*t*t+by*t*t+cy*t+y1;

        if (fill==1) {
          s=FrameBufferPIsect(ptr->hgt,ox,oy,nx,ny,in,ix); 
          if (s !=0) break;
	} else {
          s=FrameBufferRawLine(ptr,width,ox,oy,nx,ny,sze,r,g,b,a,m,dash,clip);
          if (s !=0) break;
	}
        ox=nx;
        oy=ny;
      }
    
      if (fill==1) s=FrameBufferPIsect(ptr->hgt,ox,oy,x4,y4,in,ix); 
      else s=FrameBufferRawLine(ptr,width,ox,oy,x4,y4,sze,r,g,b,a,m,dash,clip);
        
    } else {
      int ax,ay,bx,by;
      bi=n-1;    
      ax=px[ai];
      ay=py[ai];
      bx=px[bi];
      by=py[bi];

      if (matrix !=NULL) {
        FrameBufferTransform(matrix,ax,ay,&ax,&ay);
        FrameBufferTransform(matrix,bx,by,&bx,&by);
      }
      ax=x+ax;
      ay=y+ay;
      bx=x+bx;
      by=y+by;
      
      if (fill==1) {
        s=FrameBufferPIsect(ptr->hgt,ax,ay,bx,by,in,ix); 
        if (s !=0) break;
      } else {
        s=FrameBufferRawLine(ptr,width,ax,ay,bx,by,sze,r,g,b,a,m,dash,clip);
        if (s !=0) break;
      }

    }
    n++;
  }
  


  if ((s==0) && (fill==1)) {
    for (y=0;y<ptr->hgt;y++) {
     
      if (in[y]==0) continue;
      qsort( (void *) ix[y],in[y],sizeof(int),FrameBufferPcmp);
      flag=1;
      for (rs=0;rs<in[y]-1;rs++) {
        if (flag==1) 
          s=FrameBufferRawLineSegment(ptr,0,ix[y][rs],ix[y][rs+1],
                                      y,sze,r,g,b,a,m,NULL,clip);
	if (s !=0) break;
        flag=(flag==1) ? 0 :1;    
      }
    }
  }


  if (in !=NULL) free(in);
  if (ix !=NULL) for (i=0;i<ptr->hgt;i++) if (ix[i] !=NULL) free(ix[i]);
  if (ix !=NULL) free(ix);
  
  return s;
}









