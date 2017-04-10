/* shape.c
   ======= 
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rfbuffer.h"




int FrameBufferEllipse(struct FrameBuffer *ptr,
	              struct FrameBufferMatrix *matrix,
                      int x,int y,int w,int h,
                      int fill,unsigned int color,unsigned  char m,
                      int width,
                      struct FrameBufferDash *dash,
                      struct FrameBufferClip *clip) {
  int i;
  float sf=0.55;
  int px[12];
  int py[12];
  int t[12];

  px[0]=w;
  py[0]=0;

  px[1]=w;
  py[1]=h*sf;
  px[2]=w*sf;
  py[2]=h;
  px[3]=0;
  py[3]=h;

  px[4]=-w*sf;
  py[4]=h;
  px[5]=-w;
  py[5]=h*sf;
  px[6]=-w;
  py[6]=0;

  px[7]=-w;
  py[7]=-h*sf;
  px[8]=-w*sf;
  py[8]=-h;
  px[9]=0;
  py[9]=-h;

  px[10]=w*sf;
  py[10]=-h;
  px[11]=w;
  py[11]=-h*sf;

  for (i=0;i<12;i++) t[i]=1;

  return FrameBufferPolygon(ptr,matrix,x,y,12,
                            px,py,t,fill,color,m,width,dash,clip); 

}

int FrameBufferRectangle(struct FrameBuffer *ptr,
	         	 struct FrameBufferMatrix *matrix,
                        int x,int y,int w,int h,
                        int fill,unsigned int color,unsigned  char m,
                        int width,
                        struct FrameBufferDash *dash,
                        struct FrameBufferClip *clip) {
  
  int px[4]={0,0,0,0};
  int py[4]={0,0,0,0};
  int t[4]={0,0,0,0};

  px[1]=w;
  px[2]=w;
  py[2]=h;
  py[3]=h;

  return FrameBufferPolygon(ptr,matrix,x,y,4,px,py,t,fill,color,m,width,
                            dash,clip); 
}

