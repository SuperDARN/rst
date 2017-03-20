/* grid.c
   ======
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <zlib.h>
#include "rtypes.h"
#include "rtime.h"
#include "dmap.h"
#include "griddata.h"




struct GridData *GridMake() {
  struct GridData *ptr=NULL;
  ptr=malloc(sizeof(struct GridData));
  if (ptr==NULL) return 0;

  memset(ptr,0,sizeof(struct GridData));
  ptr->sdata=NULL;
  ptr->data=NULL;
  return ptr;
}

void GridFree(struct GridData *ptr) {
  if (ptr==NULL);
  if (ptr->sdata !=NULL) free(ptr->sdata);
  if (ptr->data !=NULL) free(ptr->data);
  free (ptr);
}

