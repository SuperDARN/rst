/* bezier.c
   ======== 
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rfbuffer.h"
#include "iplot.h"

int PlotBezier(struct Plot *ptr,
                float x1,float y1,float x2,float y2,
                float x3,float y3,float x4,float y4,
                unsigned int color,unsigned char mask,float width,
                struct PlotDash *dash) {

  if (ptr==NULL) return -1;
  if (ptr->bezier.func==NULL) return 0;
  return (ptr->bezier.func)(ptr->bezier.data,x1,y1,x2,y2,x3,y3,x4,y4,
                     color,mask,width,dash);
} 

