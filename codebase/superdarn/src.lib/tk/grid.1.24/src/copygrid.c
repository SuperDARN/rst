/* copygrid.c
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
#include "rfile.h"
#include "griddata.h"



void GridCopy(struct GridData *a,struct GridData *b) {

  a->st_time=b->st_time;
  a->ed_time=b->ed_time;

  a->stnum=b->stnum;
  a->vcnum=b->vcnum;
  a->xtd=b->xtd;

  if (b->stnum>0) {
    if (a->sdata==NULL) a->sdata=malloc(sizeof(struct GridSVec)*b->stnum);
    else a->sdata=realloc(a->sdata,sizeof(struct GridSVec)*b->stnum);
  } else if (a->sdata !=NULL) {
    free(a->sdata);
    a->sdata=NULL;
  }

  if (b->vcnum>0) {
  if (a->data==NULL) a->data=malloc(sizeof(struct GridGVec)*b->vcnum);
  else a->data=realloc(a->data,sizeof(struct GridGVec)*b->vcnum);
  } else if (a->data !=NULL) {
    free(a->data);
    a->data=NULL;
  }

  
  if (b->stnum !=0) memcpy(a->sdata,b->sdata,
                             sizeof(struct GridSVec)*b->stnum);
  if (b->vcnum !=0) memcpy(a->data,b->data,
                             sizeof(struct GridGVec)*b->vcnum);
}
