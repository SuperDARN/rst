/* plot_imf_delay.c
   ================= 
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



void plot_imf_delay(struct Plot *plot,
		    float xoff,float yoff,float rad,
		    int delay,
                    unsigned int color,unsigned char mask,
                    char *fontname,float fontsize,
                    void *txtdata) {

  char txt[256];
  float txbox[3];

  sprintf(txt,"(-%.2d min)",delay);
  txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);
 
  PlotText(plot,NULL,fontname,fontsize,xoff-rad-4-txbox[0],yoff-0.4*rad,
            strlen(txt),txt,color,mask,1);
}
  
