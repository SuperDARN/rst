/* solvepotential.c
   ================= 
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
#include "evalpotential.h"
#include "crdshft.h"

void CnvMapEvaPotential(int,int,double *,double *,double *,double *);

int CnvMapSolvePotential(struct CnvMapData *ptr,struct CnvGrid *pptr) {
  
  int i;
  double lon,lat,tmp=0; 
  double tlimit=PI;
  double alpha;
  double th;
  
  double *ph;
  double *x;
  double *plm;
   
 
  alpha=tlimit/((90.0-fabs(ptr->latmin))/180.0*PI);  

  x=malloc(sizeof(double)*pptr->num);
  ph=malloc(sizeof(double)*pptr->num);
 
  if ((x==NULL) || (ph==NULL)) return -1;

  for (i=0;i<pptr->num;i++) {
    lat=fabs(pptr->lat[i]);
    lon=pptr->lon[i];
    if ((ptr->lat_shft !=0) || (ptr->lon_shft !=0)) 
      CnvMapCrdShft(&lat,&lon,&tmp,ptr->lat_shft,ptr->lon_shft);
    th=alpha*(90.0-lat)/180.0*PI;
    ph[i]=lon/180.0*PI;
    x[i]=cos(th);
  }

  plm=malloc(sizeof(double)*(ptr->fit_order+1)*(ptr->fit_order+1)*pptr->num);

  CnvMapEvalLegendre(ptr->fit_order,x,pptr->num,plm);
  CnvMapEvalPotential(ptr->fit_order,pptr->num,ptr->coef,plm,ph,pptr->mag);
  free(plm);
  free(x);
  free(ph);
  return 0;
}











