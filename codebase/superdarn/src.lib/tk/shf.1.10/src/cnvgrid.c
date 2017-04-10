/* cnvgrid.c
   =========
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
#include "cnvgrid.h"




struct CnvGrid *CnvGridMake() {
  struct CnvGrid *ptr=NULL;
  ptr=malloc(sizeof(struct CnvGrid));
  if (ptr==NULL) return 0;

  memset(ptr,0,sizeof(struct CnvGrid));
  ptr->vertex=NULL;
  ptr->lat=NULL;
  ptr->lon=NULL;
  ptr->mag=NULL;
  ptr->azm=NULL;
  ptr->ex=NULL;
  ptr->ey=NULL;
  return ptr;
}

void CnvGridFree(struct CnvGrid *ptr) {
  if (ptr==NULL);
  if (ptr->vertex !=NULL) free(ptr->vertex);
  if (ptr->lat !=NULL) free(ptr->lat);
  if (ptr->lon !=NULL) free(ptr->lon);
  if (ptr->mag !=NULL) free(ptr->mag);
  if (ptr->azm !=NULL) free(ptr->azm);
  if (ptr->ex !=NULL) free(ptr->ex);
  if (ptr->ey !=NULL) free(ptr->ey);
  free (ptr);
}

