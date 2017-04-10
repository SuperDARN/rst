/* polygon.c
   ========= 
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




int PlotPolygon(struct Plot *ptr,struct PlotMatrix *matrix,
                 float x,float y,
                 int num,float *px,float *py,int *t,int fill,
                 unsigned int color,unsigned char mask,float width,
                 struct PlotDash *dash) {

  if (ptr==NULL) return -1;
  if (ptr->polygon.func==NULL) return 0;
  return (ptr->polygon.func)(ptr->polygon.data,matrix,x,y,num,px,py,t,
                             fill,color,mask,width,dash);
} 

