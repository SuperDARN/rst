/* clip.c
   ====== 
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
#include <string.h>
#include "rfbuffer.h"



int FrameBufferClipCmp(const void *a,const void  *b) {
  int x,y;
  x=*((int *) a);
  y=*((int *) b);
  if (x>y) return 1;  
  if (x<y) return -1;
  return 0; 
}

int FrameBufferClipIsect(int hgt,int ax,int ay,
                         int bx,int by,int *in,int **ix) {
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




struct FrameBufferClip *FrameBufferMakeClip(int wdt,int hgt,int num,
                                           int *px,
                                           int *py,int *t) {
  struct FrameBufferClip *ptr=NULL;
  int **ix=NULL;
  int *in=NULL;

  int ai,bi,ci,di;
  int l,r;
  int flag=0;
  int rs;
  int x,y;
  int i,n,s=0;
  if (wdt==0) return NULL;
  if (hgt==0) return NULL;
  if (px==NULL) return NULL;
  if (py==NULL) return NULL;
  if (t==NULL) return NULL;
  if (num==0) return NULL;

  in=malloc(sizeof(int)*hgt);
  if (in==NULL) return NULL;
  ix=malloc(sizeof(int *)*hgt);
  if (ix==NULL) {
    free(in);
    return NULL;
  }

  for (i=0;i<hgt;i++) ix[i]=NULL;
  for (i=0;i<hgt;i++) in[i]=0;


  ptr=malloc(sizeof(struct FrameBufferClip));
  if (ptr==NULL) {
    free(ix);
    free(in);
  }
  ptr->clp=malloc(wdt*hgt);
  if (ptr->clp==0) {
    free(ix);
    free(in);
    free(ptr);
    return NULL;
  }
  ptr->wdt=wdt;
  ptr->hgt=hgt;

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
        s=FrameBufferClipIsect(hgt,ox,oy,nx,ny,in,ix); 
        if (s !=0) break;
        ox=nx;
        oy=ny;
      }
      s=FrameBufferClipIsect(hgt,ox,oy,x4,y4,in,ix); 
      if (s !=0) break;
    } else {
      int ax,ay,bx,by;
      bi=n-1;    
      ax=px[ai];
      ay=py[ai];
      bx=px[bi];
      by=py[bi];
      s=FrameBufferClipIsect(hgt,ax,ay,bx,by,in,ix); 
    }
    n++;
  }
 
  memset(ptr->clp,0,wdt*hgt); 

  if (s==0) {
    for (y=0;y<hgt;y++) {
      if (in[y]==0) continue;
      qsort( (void *) ix[y],in[y],sizeof(int),FrameBufferClipCmp);
      flag=1;
      for (rs=0;rs<in[y]-1;rs++) {
        if (flag==1) {
          l=ix[y][rs];
          r=ix[y][rs+1];
          if (r<0) continue;
          if (l>=wdt) continue;
          if (l<0) l=0;
          if (r>=wdt) r=wdt;
          for (x=l;x<r;x++) ptr->clp[y*wdt+x]=0xff;
	}
        flag=(flag==1) ? 0 :1;    
      }
    }
  }

  if (in !=NULL) free(in);
  if (ix !=NULL) for (i=0;i<ptr->hgt;i++) if (ix[i] !=NULL) free(ix[i]);
  if (ix !=NULL) free(ix);
  if (s !=0) {
    free(ptr->clp);
    free(ptr);
    return NULL;
  } 
  
  return ptr;
}

void FrameBufferFreeClip(struct FrameBufferClip *ptr) {
  if (ptr==NULL) return;
  if (ptr->clp !=NULL) free(ptr->clp);
  free(ptr);
  return;
}

int FrameBufferClip(struct FrameBufferClip *ptr,int x,int y) {
  if (ptr==NULL) return 1; 
  if (x<0) return 0;
  if (y<0) return 0;
  if (x>=ptr->wdt) return 0;
  if (y>=ptr->hgt) return 0;
  return (ptr->clp[y*ptr->wdt+x] !=0);
}

                     
