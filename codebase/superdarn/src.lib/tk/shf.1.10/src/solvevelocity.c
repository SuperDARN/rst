/* solvevelocity.c
   ===============
   Author: R.J.Barnes
*/


/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
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











