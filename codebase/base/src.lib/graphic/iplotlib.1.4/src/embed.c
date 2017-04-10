/* embed.c
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

int PlotEmbedStart(struct Plot *ptr,
	      struct PlotMatrix *matrix,
              float x,float y) {

  if (ptr==NULL) return -1;
  if (ptr->embed.start.func==NULL) return 0;
  return (ptr->embed.start.func)(ptr->embed.start.data,matrix,x,y);
} 

int PlotEmbedEnd(struct Plot *ptr) {
  if (ptr==NULL) return -1;
  if (ptr->embed.end.func==NULL) return 0;
  return (ptr->embed.end.func)(ptr->embed.end.data);
}


