/* make_vgrid.c
   ============ 
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















