/* evalefield.c
   ============= 
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


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rmath.h"
#include "rfile.h"
#include "griddata.h"
#include "cnvmap.h"
#include "cnvgrid.h"
#include "evallegendre.h"
#include "evalvelocity.h"
#include "crdshft.h"




void CnvMapEvalEfield(int Lmax,double *coef,double *plm,
		      struct CnvGrid *vptr,
		      double latmin,struct CnvMapData *ptr) {

  int i;
  double *theta,*phi;
  double *etc,*epc;
  double lon,lat,tmp=0;

  theta=malloc(vptr->num*sizeof(double));
  if (theta==NULL) return;
  phi=malloc(vptr->num*sizeof(double));
  if (phi==NULL) {
    free(theta);
    return;
  }

  for (i=0;i<vptr->num;i++) {
     lat=fabs(vptr->lat[i]);
     lon=vptr->lon[i];
     if ((ptr->lat_shft !=0) || (ptr->lon_shft !=0)) 
       CnvMapCrdShft(&lat,&lon,&tmp,ptr->lat_shft,ptr->lon_shft);
     theta[i]=(90.0-lat)*PI/180.0;
     phi[i]=lon*PI/180.0;
  }
  epc=CnvMapEvalPhiCoef(Lmax,coef,theta,vptr->num);
  etc=CnvMapEvalThetaCoef(Lmax,coef,theta,vptr->num,fabs(latmin));
  CnvMapEvalComponent(Lmax,etc,plm,phi,vptr->num,vptr->ex);
  CnvMapEvalComponent(Lmax,epc,plm,phi,vptr->num,vptr->ey);
   
  free(phi);
  free(theta); 
  free(epc);
  free(etc);

}














