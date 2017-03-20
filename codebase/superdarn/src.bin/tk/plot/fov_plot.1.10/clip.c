/* clip.c
   ====== 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/



#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "rfbuffer.h"




struct FrameBufferClip *fbclip(int wdt,int hgt) {

  int i;
  float sf=0.55;
  int px[12];
  int py[12];
  int t[12];

  px[0]=0.5*wdt;
  py[0]=0;
  px[1]=0.5*wdt;
  py[1]=0.5*hgt*sf;
  px[2]=0.5*wdt*sf;
  py[2]=0.5*hgt;
  px[3]=0;
  py[3]=0.5*hgt;
  px[4]=-0.5*wdt*sf;
  py[4]=0.5*hgt;
  px[5]=-0.5*wdt;
  py[5]=0.5*hgt*sf;
  px[6]=-0.5*wdt;
  py[6]=0;
  px[7]=-0.5*wdt;
  py[7]=-0.5*hgt*sf;
  px[8]=-0.5*wdt*sf;
  py[8]=-0.5*hgt;
  px[9]=0;
  py[9]=-0.5*hgt;
  px[10]=0.5*wdt*sf;
  py[10]=-0.5*hgt;
  px[11]=0.5*wdt;
  py[11]=-0.5*hgt*sf;

  for (i=0;i<12;i++) {
    t[i]=1;
    px[i]+=0.5*wdt;
    py[i]+=0.5*hgt;
  }
  return FrameBufferMakeClip(wdt,hgt,12,px,py,t);
}

