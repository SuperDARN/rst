/* info.c
   ======= 
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




int PlotInfoStart(struct Plot *ptr) {
  if (ptr==NULL) return -1;
  if (ptr->info.start.func==NULL) return 0;
  return (ptr->info.start.func)(ptr->info.start.data);
} 

int PlotInfo(struct Plot *ptr,char *buf,int sze) {
  if (ptr==NULL) return -1;
  if (ptr->info.text.func==NULL) return 0;
  return (ptr->info.text.func)(ptr->info.text.data,buf,sze);
}

int PlotInfoEnd(struct Plot *ptr) {
  if (ptr==NULL) return -1;
  if (ptr->info.end.func==NULL) return 0;
  return (ptr->info.end.func)(ptr->info.end.data);
}


