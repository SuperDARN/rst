/* plot_model_name.c
   ================= 
   Author: R.J.Barnes
*/


/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "rfbuffer.h"
#include "iplot.h"
#include "rtime.h"
#include "text_box.h"



void plot_model_name(struct Plot *plot,
                    float xoff,float yoff,
	            char *dir,char *mag,
                    unsigned int color,unsigned char mask,
                    char *fontname,float fontsize,
                    void *txtdata) {

  char txt[256];
  float txbox[3];
  float lhgt;

  sprintf(txt,"Xy");
  txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);
  lhgt=4+txbox[2]-txbox[1];

  sprintf(txt,"APL MODEL");
  txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);
 
  PlotText(plot,NULL,fontname,fontsize,xoff-txbox[0]/2,yoff,
            strlen(txt),txt,color,mask,1);

  txtbox(fontname,fontsize,strlen(mag),mag,txbox,txtdata);
  PlotText(plot,NULL,fontname,fontsize,xoff-txbox[0]/2,yoff+lhgt,
            strlen(mag),mag,color,mask,1);

  txtbox(fontname,fontsize,strlen(dir),dir,txbox,txtdata);
  PlotText(plot,NULL,fontname,fontsize,xoff-txbox[0]/2,yoff+2*lhgt,
            strlen(dir),dir,color,mask,1);
}
  
