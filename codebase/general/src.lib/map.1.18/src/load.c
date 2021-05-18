/* load.c
   ====== 
   Author: R.J.Barnes
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> 
#include <zlib.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rconvert.h"
#include "rfbuffer.h"
#include "iplot.h"
#include "polygon.h"
#include "rmath.h"
#include "rmap.h"




struct PolygonData *MapFread(FILE *fp) {
  float pnt[2];
  struct PolygonData *ptr=NULL;
  int32 n;
  int s,i;
  

  ptr=PolygonMake(2*sizeof(float),NULL);
  if (ptr==NULL) return NULL;

  while(ConvertFreadInt(fp,&n)==0) {
    s=fgetc(fp);
    PolygonAddPolygon(ptr,s);
    for (i=0;i<n;i++) {
      ConvertFreadFloat(fp,&pnt[0]);
      ConvertFreadFloat(fp,&pnt[1]);
      PolygonAdd(ptr,pnt);
    }
  } 
  return ptr;
}

struct PolygonData *MapBndFread(FILE *fp) {
  float pnt[2];
  struct PolygonData *ptr=NULL;
  int32 n;
  int32 id;
  int i;
  

  ptr=PolygonMake(2*sizeof(float),NULL);
  if (ptr==NULL) return NULL;

  while(ConvertFreadInt(fp,&n)==0) {
    ConvertFreadInt(fp,&id);
    PolygonAddPolygon(ptr,id);
    for (i=0;i<n;i++) {
      ConvertFreadFloat(fp,&pnt[0]);
      ConvertFreadFloat(fp,&pnt[1]);
      PolygonAdd(ptr,pnt);
    }
  } 
  return ptr;
}
