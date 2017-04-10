/* plot_source.c
   ============= 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "rfbuffer.h"
#include "iplot.h"
#include "rtime.h"
#include "text_box.h"





void plot_source(struct Plot *plot,
		 float xoff,float yoff,
                 char *source,int major,int minor,
  	         unsigned int color,unsigned char mask,
                 char *fontname,float fontsize,
                 void *txtdata) {

  char txt[256];
  sprintf(txt,"(%s) %d.%d",source,major,minor);
  PlotText(plot,NULL,fontname,fontsize,xoff,yoff,
            strlen(txt),txt,color,mask,1);

  
}
  
