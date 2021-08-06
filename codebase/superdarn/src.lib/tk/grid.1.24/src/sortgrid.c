/* sort_grid.c
   ===========
   Author: R.J.Barnes
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
#include <sys/types.h>
#include "rtypes.h"
#include "rfile.h"
#include "griddata.h"

int GridSortVec(const void *a,const void *b) {
  struct GridGVec *ga,*gb;
  ga=(struct GridGVec *) a;
  gb=(struct GridGVec *) b;
  if (ga->st_id < gb->st_id) return -1;
  if (ga->st_id > gb->st_id) return 1;
  if (ga->index < gb->index) return -1;
  if (ga->index > gb->index) return 1;
  return 0;
}

void GridSort(struct GridData *ptr) {
            
  /* sort the stations into order */

   qsort(ptr->data,ptr->vcnum,sizeof(struct GridGVec),GridSortVec);
}
