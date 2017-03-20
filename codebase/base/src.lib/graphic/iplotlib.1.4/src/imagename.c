/* imagename.c
   =========== 
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




int PlotImageName(struct Plot *ptr,
	      struct PlotMatrix *matrix,
	      char *name,int w,int h,int d,
	      unsigned  char mask,
              float x,float y,int sflg) {

  if (ptr==NULL) return -1;
  if (ptr->imagename.func==NULL) return 0;
  return (ptr->imagename.func)(ptr->imagename.data,matrix,name,w,h,d,
                               mask,x,y,sflg);
} 

