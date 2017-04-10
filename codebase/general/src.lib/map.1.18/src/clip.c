/* clip.c
   ====== 
   Author: R.J.Barnes
*/

/*
   See license.txt
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> 
#include <sys/types.h>
#include "rtypes.h"
#include "rfbuffer.h"
#include "iplot.h"
#include "polygon.h"
#include "rmath.h"
#include "rmap.h"



struct PolygonData *MapCircleClip(float step) {
  float p[2],r;
  struct PolygonData *clip;
  
  clip=PolygonMake(2*sizeof(float),PolygonXYbbox);
  if (clip==NULL) return NULL;
  PolygonAddPolygon(clip,1);
  if (step<1) step=1; 
  if (step>45) step=45;
  for (r=0;r<360;r+=step) {
    p[0]=0.5+0.5*cosd(r);
    p[1]=0.5+0.5*sind(r);
    PolygonAdd(clip,p);
  } 
  return clip;
}

struct PolygonData *MapSquareClip() {
  int n=4;
  int i;
  float p[]={0,0, 1,0,  1,1, 0,1};
  struct PolygonData *clip;
  
  clip=PolygonMake(2*sizeof(float),PolygonXYbbox);
  if (clip==NULL) return NULL;
  PolygonAddPolygon(clip,1);

  for (i=0;i<n;i++) PolygonAdd(clip,&p[2*i]);
  return clip;
}

