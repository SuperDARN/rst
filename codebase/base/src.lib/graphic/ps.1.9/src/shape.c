/* shape.c
   ======= 
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rfbuffer.h"
#include "rps.h"



int PostScriptEllipse(struct PostScript *ptr,
	              struct PostScriptMatrix *matrix,
                      float x,float y,float w,float h,
                      int fill,unsigned int color,
                      float width,
		      struct PostScriptDash *dash,
                      struct PostScriptClip *clip) {

  int i;
  float sf=0.55;
  float px[12];
  float py[12];
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

  return PostScriptPolygon(ptr,matrix,x,y,12,
                            px,py,t,fill,color,width,dash,clip); 



}

int PostScriptRectangle(struct PostScript *ptr,
	         struct PostScriptMatrix *matrix,
                 float x,float y,float w,float h,
			int fill,unsigned int color,
                 float width,
		 struct PostScriptDash *dash,struct PostScriptClip *clip) {

  float px[4]={0,0,0,0};
  float py[4]={0,0,0,0};
  int t[4]={0,0,0,0};

  px[1]=w;
  px[2]=w;
  py[2]=h;
  py[3]=h;

  return PostScriptPolygon(ptr,matrix,x,y,4,px,py,t,fill,color,
                           width,dash,clip); 
}

