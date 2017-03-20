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
#include "rfbuffer.h"
#include "iplot.h"




int PlotClip(struct Plot *ptr,
              int num,float *px,float *py,int *t) {
  if (ptr==NULL) return -1;
  if (ptr->clip.func==NULL) return 0;
  return (ptr->clip.func)(ptr->clip.data,num,px,py,t);
} 






