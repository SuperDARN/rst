/* solvevelocity.c
   ===============
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
#include "rmath.h"
#include "rfile.h"
#include "cnvgrid.h"
#include "griddata.h"
#include "cnvmap.h"
#include "evallegendre.h"
#include "evalvelocity.h"
#include "crdshft.h"



int CnvMapSolveVelocity(struct CnvMapData *ptr,struct CnvGrid *vptr) {
  
  int i;
 
  double tlimit=PI;
  double alpha;
  double th;
  double lon,lat,tmp=0;
  
  double *x;
  double *plm;
   
  alpha=tlimit/((90.0-fabs(ptr->latmin))/180.0*PI);  
 
  x=malloc(sizeof(double)*vptr->num);
 
  if (x==NULL) return -1;

  for (i=0;i<vptr->num;i++) {
    lat=fabs(vptr->lat[i]);
    lon=vptr->lon[i];
    if ((ptr->lat_shft !=0) || (ptr->lon_shft !=0)) 
      CnvMapCrdShft(&lat,&lon,&tmp,ptr->lat_shft,ptr->lon_shft);
    th=alpha*(90.0-lat)/180.0*PI;
    x[i]=cos(th);
  }
 
  plm=malloc(sizeof(double)*(ptr->fit_order+1)*(ptr->fit_order+1)*vptr->num);

  CnvMapEvalLegendre(ptr->fit_order,x,vptr->num,plm);
  CnvMapEvalVelocity(ptr->fit_order,ptr->coef,plm,vptr,ptr->latmin,ptr);
  free(plm);
  free(x);
  return 0;
}











