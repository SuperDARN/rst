/* line.c
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
#include "rfbuffer.h"





int FrameBufferRawPixel(struct FrameBuffer *ptr,int x,int y,int sze,
                        unsigned char r,unsigned char g,unsigned char b,
                        unsigned char a,unsigned char m,
                        struct FrameBufferClip *clip) {

  /* lowest of the low - renders a pixel */

  int off;
  float c,ca,cb,t,ta,tb;

  if (ptr==NULL) return -1;
  if (ptr->wdt==0) return -1;
  if (ptr->hgt==0) return -1;
  if (ptr->img==NULL) return -1;
  if (y<0) return 0;
  if (x<0) return 0;
  if (x>=ptr->wdt) return 0;
  if (y>=ptr->hgt) return 0;

  if (FrameBufferClip(clip,x,y)==0) return 0;

  sze=ptr->wdt*ptr->hgt;
  off=y*ptr->wdt+x;
  if (ptr->user.pixel !=NULL) 
    return (ptr->user.pixel) (ptr->wdt,ptr->hgt,ptr->img,ptr->msk,
                              x,y,ptr->depth,
                              off,sze,r,g,b,a,ptr->user.data);


  /* direct over painting */


  if (m & 0x40) ptr->img[off]=r;
  if (m & 0x80) ptr->msk[off]=a;
  if (ptr->depth !=8) {
    if (m & 0x20) ptr->img[off+sze]=g;
    if (m & 0x10) ptr->img[off+2*sze]=b;
  }

  if ((m &0x0f)==0) return 0; /* nothing else to do */

  /* alpha channel to max, so simple replace */

  if (a==255) {
    if (m & 0x04) ptr->img[off]=r;
    if (m & 0x08) ptr->msk[off]=a;
    if (ptr->depth !=8) {
      if (m & 0x02) ptr->img[off+sze]=g;
      if (m & 0x01) ptr->img[off+2*sze]=b;
    }
    return 0;
  }
    
  /* simple porter-duff over compositing */

  tb=ptr->msk[off]/255.0;
  ta=a/255.0;

  t=ta+(tb-ta)*tb;

  if (m & 0x08) ptr->msk[off]=(unsigned char) (255.0*t);

   if (m & 0x04) {
    cb=ptr->img[off]/255.0;
    ca=r/255.0;
    c=ca*ta+cb*tb*(1-ta);
    ptr->img[off]= (unsigned char) (255.0*c);
  }

  if (ptr->depth==8) return 0;

  if (m & 0x02) {
    cb=ptr->img[off+sze]/255.0;
    ca=g/255.0;
    c=ca*ta+cb*tb*(1-ta);
    ptr->img[off+sze]= (unsigned char) (255.0*c);
  }

  if (m & 0x01) {
    cb=ptr->img[off+2*sze]/255.0;
    ca=b/255.0;
    c=ca*ta+cb*tb*(1-ta);
    ptr->img[off+2*sze]=(unsigned char) (255.0*c);
  }

  return 0;
} 

int FrameBufferRawDot(struct FrameBuffer *ptr,int width,int x,int y,int sze,
                      unsigned char r,unsigned g,unsigned b,
                      unsigned char a,unsigned char m,
                      struct FrameBufferDash *dash,
                      struct FrameBufferClip *clip) {
  int ax,ay,bx,by;
  int tx,ty;
  int s=0;

  if (ptr==NULL) return -1;
  if (ptr->wdt==0) return -1;
  if (ptr->hgt==0) return -1;
  if (ptr->img==NULL) return -1;
   
  if ((dash !=NULL) && (FrameBufferDashState(dash)==0)) return 0;

  if (width<1) {
    s=FrameBufferRawPixel(ptr,x,y,sze,r,g,b,a,m,clip);
  } else {
    ax=x-width;
    bx=x+width;
    ay=y-width;
    by=y+width;
    
    if (bx<0) return 0;
    if (by<0) return 0;
    if (ax>=ptr->wdt) return 0;
    if (ay>=ptr->hgt) return 0;

    if (ax<0) ax=0;
    if (ay<0) ay=0;
    if (bx>ptr->wdt) bx=ptr->wdt;
    if (by>ptr->hgt) by=ptr->hgt;

    for (tx=ax;tx<bx;tx++) {
      for (ty=ay;ty<by;ty++) {
	s=FrameBufferRawPixel(ptr,tx,ty,sze,r,g,b,a,m,clip);
        if (s !=0) break;
      }
      if (s !=0) break; 
    }
  }
  return s;
}

int FrameBufferRawLineSegment(
                      struct FrameBuffer *ptr,int width,
                      int ax,int bx,int y,int sze,
                      unsigned char r,unsigned g,unsigned b,
                      unsigned char a,unsigned char m,
                      struct FrameBufferDash *dash,
                      struct FrameBufferClip *clip) {
  int s=0;
  int xs,xe;
  int x;

  if (y<0) return 0;
  if (y>=ptr->hgt) return 0;

  xs=ax;
  xe=bx;
  if (bx<xs) {
    xs=bx;
    xe=ax;
  }



  if ((xs>=ptr->wdt) && (xe>=ptr->wdt)) return 0;
  if ((xs<0) && (xe<0)) return 0;
  if (xs<0) xs=0;
  if (xe>ptr->wdt) xe=ptr->wdt;
  x=xs;
  while (x !=xe) {
    if ((dash !=NULL) && (FrameBufferDashState(dash)==0)) {
      x++;
    } else {
      if (width<1) 
      s=FrameBufferRawPixel(ptr,x,y,sze,r,g,b,a,m,clip);
      else s=FrameBufferRawDot(ptr,width,x,y,sze,r,g,b,a,m,NULL,clip);
      x++;
      if (s !=0) break;
    } 
  }
  return s;
}

int FrameBufferRawLine(struct FrameBuffer *ptr,int width,
                       int ax,int ay,int bx,int by,int sze,
                       unsigned char r,unsigned g,unsigned b,
                       unsigned char a,unsigned char m,
                       struct FrameBufferDash *dash,
                       struct FrameBufferClip *clip) {
  int s=0;
  int nx,ox;
  int ys,ye;
  int xs,xe;
  int y;
  if (ay==by) {
    if (ax==bx) s=FrameBufferRawDot(ptr,width,ax,ay,sze,r,g,b,a,m,dash,clip);
    else s=FrameBufferRawLineSegment(ptr,width,ax,bx,ay,sze,r,g,b,a,m,
                                     dash,clip);
  } else {
    ys=ay;
    ye=by;
    xs=ax;
    xe=bx;
    if (by<ay) {
      ys=by;
      ye=ay;
      xs=bx;
      xe=ax;
    }
    ox=xs;    
 
    if ((ys<0) && (ye<0)) return 0;
    if ((ye>=ptr->hgt) && (ys>=ptr->hgt)) return 0;
    if (ys<0) ys=0;
    if (ye>ptr->hgt) ye=ptr->hgt;
    y=ys+1;
    while(y<=ye) {
      nx=xs+(xe-xs)*(y-ys)/(ye-ys);
      if (ox==nx) s=FrameBufferRawDot(ptr,width,nx,y,sze,r,g,b,a,m,dash,
                                      clip);
      else s=FrameBufferRawLineSegment(ptr,width,ox,nx,y,sze,r,g,b,a,m,dash,
                                       clip);
      if (s !=0) break;
      y++;
      ox=nx;
    }  
  }
  return s;
}


int FrameBufferLine(struct FrameBuffer *ptr,
                    int ax,int ay,int bx,int by,
                    unsigned int color,unsigned char m,int width,
                    struct FrameBufferDash *dash,
                    struct FrameBufferClip *clip) {
  unsigned char r,g,b,a;
  int sze;
 
  if (ptr==NULL) return -1;

  sze=ptr->wdt*ptr->hgt;
  a=color>>24;;
  r=color>>16;
  g=color>>8;
  b=color;

  return FrameBufferRawLine(ptr,width,ax,ay,bx,by,sze,r,g,b,a,m,dash,clip);
}
