/* bezier.c
   ======== 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rfbuffer.h"



int FrameBufferBezier(struct FrameBuffer *ptr,
                      int x1,int y1,int x2,int y2,
                      int x3,int y3,int x4,int y4,float step,
                      unsigned int color,unsigned char m,int width,
                      struct FrameBufferDash *dash,
                      struct FrameBufferClip *clip) {


  float ax,bx,cx;
  float ay,by,cy;
  int ox,oy,nx,ny;

  int s=0;
  int sze;
  float t=0;
  unsigned char r,g,b,a;
 
  if (ptr==NULL) return -1;

  sze=ptr->wdt*ptr->hgt;
  a=color>>24;;
  r=color>>16;
  g=color>>8;
  b=color;

  if (step>0.5) step=0.5;
  if (step<0.01) step=0.01;


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
    s=FrameBufferRawLine(ptr,width,ox,oy,nx,ny,sze,r,g,b,a,m,dash,clip);
    if (s !=0) break;
    ox=nx;
    oy=ny;
  } 
  s=FrameBufferRawLine(ptr,width,ox,oy,x4,y4,sze,r,g,b,a,m,dash,clip);
  return s;
}
