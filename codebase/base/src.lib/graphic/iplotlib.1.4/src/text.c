/* text.c
   ====== 
   Author: R.J.Barnes
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
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

