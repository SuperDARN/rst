/* image.c
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
#include "iplot.h"




int PlotImage(struct Plot *ptr,
	      struct PlotMatrix *matrix,
              struct FrameBuffer *img,
	      unsigned  char mask,
              float x,float y,int sflg) {

  if (ptr==NULL) return -1;
  if (ptr->image.func==NULL) return 0;
  return (ptr->image.func)(ptr->image.data,matrix,img,mask,x,y,sflg);
} 

