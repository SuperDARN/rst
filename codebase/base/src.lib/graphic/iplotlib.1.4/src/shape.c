/* shape.c
   ======= 
   Author:R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rfbuffer.h"
#include "iplot.h"




int PlotRectangle(struct Plot *ptr,
	           struct PlotMatrix *matrix,
                   float x,float y,float w,float h,
                   int fill,unsigned int color,unsigned  char mask,
                   float width,
		   struct PlotDash *dash) {

  if (ptr==NULL) return -1;
  if (ptr->rectangle.func==NULL) return 0;
  return (ptr->rectangle.func)(ptr->rectangle.data,matrix,x,y,w,h,
                               fill,color,mask,width,dash);

} 




int PlotEllipse(struct Plot *ptr,
	           struct PlotMatrix *matrix,
                   float x,float y,float w,float h,
                   int fill,unsigned int color,unsigned char mask,
                   float width,
		   struct PlotDash *dash) {

  if (ptr==NULL) return -1;
  if (ptr->ellipse.func==NULL) return 0;
  return (ptr->ellipse.func)(ptr->ellipse.data,matrix,x,y,w,h,
                               fill,color,mask,width,dash);

} 



