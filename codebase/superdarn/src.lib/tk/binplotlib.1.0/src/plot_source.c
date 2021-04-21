/* plot_source.c
   ============= 
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
  
