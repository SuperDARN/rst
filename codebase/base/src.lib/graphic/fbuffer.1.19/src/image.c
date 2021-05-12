/* image.c
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


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rfbuffer.h"



int FrameBufferImage(struct FrameBuffer *ptr,
			 struct FrameBufferMatrix *matrix,
                         struct FrameBuffer *img,
		         unsigned char m,
                         int x,int y,int sflg,
                         struct FrameBufferClip *clip) {
  unsigned char r,g,b,a;
 
  int s=0;
  int iw,ih,isze;
  int i;
  float ma=1,mb=0,mc=0,md=1;
  float determ;
  float ia,ib,ic,id;
  int tr;

  int btm=0,lft=0,rgt=0,top=0;
  int ix[4]={0,0,0,0},iy[4]={0,0,0,0};
  int tix=0,tiy=0;
  int tx,ty;
  int sze;
  int flg;
 
  if (ptr==NULL) return -1;
  if (img==NULL) return -1;
  if (ptr->wdt==0) return -1;
  if (ptr->hgt==0) return -1;
  if (ptr->img==0) return -1;
 
  if (img->wdt==0) return -1;
  if (img->hgt==0) return -1;
  if (img->img==0) return -1;

  sze=ptr->wdt*ptr->hgt;

  if (matrix!=NULL) {
    ma=matrix->a;
    mb=matrix->b;
    mc=matrix->c;
    md=matrix->d;
  }

  /* find the inverse matrix to map from render space to
   * the image space. */

  determ=1/(ma*md-mb*mc);
  ia=md*determ;
  ib=-mb*determ;
  ic=-mc*determ;
  id=ma*determ;

  /* work out the limits of the plot in image space */

  ix[1]=img->wdt;
  ix[2]=img->wdt;
  iy[2]=img->hgt;
  iy[3]=img->hgt;

  lft=ptr->wdt;
  btm=ptr->hgt;

  for (tr=0;tr<4;tr++) {
    tx=x+ix[tr]*ma+iy[tr]*mb;
    ty=y+ix[tr]*mc+iy[tr]*md;
    if (tx<lft) lft=tx;
    if (ty<btm) btm=ty;
    if (tx>rgt) rgt=tx;
    if (ty>top) top=ty;
  } 

  if (lft<0) lft=0;
  if (btm<0) btm=0;
  if (rgt>ptr->wdt) rgt=ptr->wdt;
  if (top>ptr->hgt) top=ptr->hgt;

  iw=img->wdt;
  ih=img->hgt;
  isze=iw*ih;
 

  ty=btm;
  while (ty !=top) {
    tx=lft;
    while (tx !=rgt) {
      /* find image space coordinates */
      flg=1;
      
   
      ix[0]=(tx-x)*ia+(ty-y)*ib;
      iy[0]=(tx-x)*ic+(ty-y)*id;
          
      if (ix[0]<0) flg=0;
      if (iy[0]<0) flg=0;
      if (ix[0]>=img->wdt) flg=0;
      if (iy[0]>=img->hgt) flg=0;
 
      if (flg) {
        if (sflg) {
          int cr=0,cg=0,cb=0,ca=0,c=0;
          int ilft=0,ibtm=0,irgt=0,itop=0;
          ix[1]=(tx+1-x)*ia+(ty-y)*ib;
          iy[1]=iy[0];
          ix[2]=ix[1];
          iy[2]=(tx-x)*ic+(ty+1-y)*id;
          ix[3]=ix[0];
          iy[3]=iy[2];
          ilft=img->wdt;
          ibtm=img->hgt;
          for (i=0;i<4;i++) {
            if (ix[i]<0) ix[i]=0;
            if (iy[i]<0) iy[i]=0;
            if (ix[i]>img->wdt) ix[i]=img->wdt;
            if (iy[i]>img->hgt) iy[i]=img->hgt;
            if (ix[i]<ilft) ilft=ix[i];
            if (iy[i]<ibtm) ibtm=iy[i];
            if (ix[i]>irgt) irgt=ix[i];
            if (iy[i]>itop) itop=iy[i];
	  }
          if ((ilft==irgt) || (itop==ibtm)) {
	    if (ilft==irgt) {
	      tix=ilft;
              tiy=(ibtm+itop)/2;

	    }
            if (ibtm==itop) {
	      tix=(ilft+irgt)/2;
              tiy=ibtm;
	    }
            cr=img->img[tiy*iw+tix];
            ca=img->msk[tiy*iw+tix];
            if (img->depth !=8) {
              cg=img->img[tiy*iw+tix+isze];
              cb=img->img[tiy*iw+tix+2*isze];
            }
            c=1;
	  } else {
             for (tix=ilft;tix<irgt;tix++) {
              for (tiy=ibtm;tiy<itop;tiy++) {
                cr+=img->img[tiy*iw+tix];
                ca+=img->msk[tiy*iw+tix];
                if (img->depth !=8) {
                  cg+=img->img[tiy*iw+tix+isze];
                  cb+=img->img[tiy*iw+tix+2*isze];
                }
                c++;
	      }
	    }
	  }
          if (img->depth==8) {
            cg=cr;
            cb=cr;
	  }

          r=cr/c;
          g=cg/c;
          b=cb/c;
          a=ca/c;

          s=FrameBufferRawPixel(ptr,tx,ty,sze,r,g,b,a,m,clip);
          if (s !=0) break;
	} else {
          tix=ix[0];
          tiy=iy[0];       
 
          r=img->img[tiy*iw+tix];
          if (img->depth !=8) {
            g=img->img[tiy*iw+tix+isze];
            b=img->img[tiy*iw+tix+2*isze];
          } else {
            g=r;
            b=r;
          }
          a=img->msk[tiy*iw+tix]; 
          s=FrameBufferRawPixel(ptr,tx,ty,sze,r,g,b,a,m,clip);
          if (s !=0) break;
	}
      }
  
      tx++;
    }
    if (s !=0) break;
    ty++;
  }
 
  return s;
}








