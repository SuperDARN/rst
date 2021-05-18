/* make_vgrid.c
   ============ 
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
#include <sys/types.h>

#include "rtypes.h"
#include "rfile.h"
#include "cnvgrid.h"
#include "griddata.h"
#include "rmath.h"



void  make_vgrid(struct GridData *ptr,struct CnvGrid *vptr) {
 
  int i;
  int num=0;
  int sze;
  sze=sizeof(double)*(ptr->vcnum);
 
  if (vptr->vertex !=NULL) free(vptr->vertex);
  if (vptr->lat !=NULL) free(vptr->lat);
  if (vptr->lon !=NULL) free(vptr->lon);
  if (vptr->mag !=NULL) free(vptr->mag);
  if (vptr->azm !=NULL) free(vptr->azm);

  vptr->type=1;
  vptr->vertex=NULL;
  vptr->lon=malloc(sze);
  vptr->lat=malloc(sze); 
  vptr->mag=malloc(sze); 
  vptr->azm=malloc(sze); 

  for (i=0;i<ptr->vcnum;i++) {
    vptr->lon[num]=ptr->data[i].mlon;
    vptr->lat[num]=ptr->data[i].mlat;
    vptr->mag[i]=0;
    vptr->azm[i]=0;
    num++;
    
  }
  vptr->num=num;
  vptr->poly=0;
}















