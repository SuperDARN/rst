/* plot.c
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



int PlotPlotStart(struct Plot *ptr,char *name,
                  float wdt,float hgt,int depth) {
  if (ptr==NULL) return -1;
  if (ptr->plot.start.func==NULL) return 0;
  return (ptr->plot.start.func)(ptr->plot.start.data,name,
                                  wdt,hgt,depth);
}

int PlotPlotEnd(struct Plot *ptr) {
  if (ptr==NULL) return -1;
  if (ptr->plot.end.func==NULL) return 0;
  return (ptr->plot.end.func)(ptr->plot.end.data);
}



