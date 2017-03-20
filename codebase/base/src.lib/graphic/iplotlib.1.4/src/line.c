/* line.c
   ====== 
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




int PlotLine(struct Plot *ptr,
              float ax,float ay,float bx,float by,
              unsigned int color,unsigned char mask,float width,
              struct PlotDash *dash) {

  if (ptr==NULL) return -1;
  if (ptr->line.func==NULL) return 0;
  return (ptr->line.func)(ptr->line.data,ax,ay,bx,by,color,mask,width,dash);
} 



