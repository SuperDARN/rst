/* make_hmb.c
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
#include "cnvmap.h"
#include "polygon.h"




struct PolygonData *make_hmb(struct CnvMapData *map) {
  int i;
  float pnt[2];
  struct PolygonData *ptr;

  ptr=PolygonMake(sizeof(float)*2,NULL);
  PolygonAddPolygon(ptr,1);
           
  for (i=0;i<map->num_bnd;i++) {
    pnt[0]=map->bnd_lat[i];
    pnt[1]=map->bnd_lon[i];
    PolygonAdd(ptr,pnt);
  }
  return ptr;
}




