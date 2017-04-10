/* text.c
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





int PlotText(struct Plot *ptr,
	      struct PlotMatrix *matrix,
	      char *fname,float fsize,
              float x,float y,int num,char *txt,
              unsigned int color,unsigned char mask,
              int sflg) {


  if (ptr==NULL) return -1;
  if (ptr->text.func==NULL) return 0;
  return (ptr->text.func)(ptr->text.data,matrix,fname,fsize,x,y,num,txt,
                          color,mask,sflg);
  
} 

