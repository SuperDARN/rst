/* sort_grid.c
   ===========
   Author: R.J.Barnes
*/

/*
   See license.txt
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
