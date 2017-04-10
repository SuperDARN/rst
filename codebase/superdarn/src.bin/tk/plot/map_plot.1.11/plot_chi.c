/* plot_chi.c
   ========== 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rfbuffer.h"
#include "iplot.h"
#include "rtime.h"
#include "text_box.h"
#include "rfile.h"
#include "griddata.h"
#include "cnvmap.h"



void plot_chi(struct Plot *plot,
                float xoff,float yoff,
                struct CnvMapData *ptr,
	        int degfree,int degfree_dat,
                unsigned int color,unsigned char mask,
                char *txtfontname,char *symfontname,float fontsize,
                void *txtdata) {

  char txt[256];
  float txbox[3];
  float lhgt;

  sprintf(txt,"Xy");
  txtbox(txtfontname,fontsize,strlen(txt),txt,txbox,txtdata);
  lhgt=4+txbox[2]-txbox[1];

  txtbox(symfontname,fontsize,strlen("c"),"c",txbox,txtdata);
 

  PlotText(plot,NULL,symfontname,fontsize,xoff,yoff,
            strlen("c"),"c",color,mask,1);
  PlotText(plot,NULL,txtfontname,fontsize*0.8,xoff+txbox[0],yoff-lhgt*0.2,
            strlen("2"),"2",color,mask,1);
  sprintf(txt,"/%d=%.1g",degfree,ptr->chi_sqr/degfree);
  PlotText(plot,NULL,txtfontname,fontsize,xoff+txbox[0]*2,yoff,
            strlen(txt),txt,color,mask,1);


  PlotText(plot,NULL,symfontname,fontsize,xoff,yoff+lhgt,
            strlen("c"),"c",color,mask,1);
  PlotText(plot,NULL,txtfontname,fontsize*0.8,xoff+txbox[0],yoff+lhgt*0.8,
            strlen("2"),"2",color,mask,1);
  sprintf(txt,"/%d=%.1g",degfree_dat,ptr->chi_sqr/degfree_dat);
  PlotText(plot,NULL,txtfontname,fontsize,xoff+txbox[0]*2,yoff+lhgt,
            strlen(txt),txt,color,mask,1);
}
  
