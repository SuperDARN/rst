/* load.c
   ====== 
   Author: R.J.Barnes
*/

/*
   See license.txt
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
