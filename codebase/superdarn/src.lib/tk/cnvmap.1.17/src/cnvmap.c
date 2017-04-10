/* cnvmap.c
   ========
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
#include "cnvmap.h"




struct CnvMapData *CnvMapMake() {
  struct CnvMapData *ptr=NULL;
  ptr=malloc(sizeof(struct CnvMapData));
  if (ptr==NULL) return 0;

  memset(ptr,0,sizeof(struct CnvMapData));
  ptr->coef=NULL;
  ptr->model=NULL;
  ptr->bnd_lat=NULL;
  ptr->bnd_lon=NULL;
  return ptr;
}

void CnvMapFree(struct CnvMapData *ptr) {
  if (ptr==NULL);
  if (ptr->coef !=NULL) free(ptr->coef);
  if (ptr->model !=NULL) free(ptr->model);
  if (ptr->bnd_lat !=NULL) free(ptr->bnd_lat);
  if (ptr->bnd_lon !=NULL) free(ptr->bnd_lon);
  free (ptr);
}

