/* cnvmap.c
   ========
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

