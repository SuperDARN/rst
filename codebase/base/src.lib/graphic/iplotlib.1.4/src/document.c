/* document.c
   ========== 
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

int PlotDocumentStart(struct Plot *ptr,char *name,
                      char *pages,float wdt,float hgt,int depth) {

  if (ptr==NULL) return -1;
  if (ptr->document.start.func==NULL) return 0;
  return (ptr->document.start.func)(ptr->document.start.data,name,pages,
                                  wdt,hgt,depth);
}

int PlotDocumentEnd(struct Plot *ptr) {

  if (ptr==NULL) return -1;
  if (ptr->document.end.func==NULL) return 0;
  return (ptr->document.end.func)(ptr->document.end.data);
}



