/* plot_extra.c
   ============ 
   Author: R.J.Barnes
*/


/*
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rfbuffer.h"
#include "iplot.h"
#include "rtime.h"
#include "text_box.h"
#include "rfile.h"
#include "griddata.h"
#include "cnvmap.h"



void plot_extra(struct Plot *plot,
                float xoff,float yoff,
	        struct CnvMapData *ptr,
                unsigned int color,unsigned char mask,
                char *fontname,float fontsize,
                void *txtdata) {

  char txt[256];
  float txbox[3];
  float lhgt;

  sprintf(txt,"Xy");
  txtbox(fontname,fontsize,strlen(txt),txt,txbox,txtdata);
  lhgt=4+txbox[2]-txbox[1];

  sprintf(txt,"order=%d",ptr->fit_order);
  PlotText(plot,NULL,fontname,fontsize,xoff,yoff,
            strlen(txt),txt,color,mask,1);
  sprintf(txt,"lat. bnd.=%g",ptr->latmin);
  PlotText(plot,NULL,fontname,fontsize,xoff,yoff+lhgt,
            strlen(txt),txt,color,mask,1);

  if (ptr->error_wt !=0) strcpy(txt,"err wt/");
  else strcpy(txt,"");
  if (ptr->model_wt !=0) strcat(txt,"norm mod wt");
  else strcat(txt,"fix mod wt");
  PlotText(plot,NULL,fontname,fontsize,xoff,yoff+2*lhgt,
            strlen(txt),txt,color,mask,1);
  sprintf(txt,"%s %d.%.2d",ptr->source,ptr->major_rev,ptr->minor_rev);
  PlotText(plot,NULL,fontname,fontsize,xoff,yoff+3*lhgt,
            strlen(txt),txt,color,mask,1);



  
}
  
