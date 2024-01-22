/* evalvelocity.c
   =============== 
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
#include "rtime.h"
#include "griddata.h"
#include "cnvmap.h"
#include "cnvgrid.h"
#include "evallegendre.h"
#include "crdshft.h"
#include "shfconst.h"
#include "calc_bmag.h"


double *CnvMapEvalThetaCoef(int Lmax,double *coef,double *theta,int n,
                            double latmin) {

  int k1,k2,k3,k4,kmax;
  int i,m,L;
  double alpha=1;
  double tmax;
  double *ecoef;

  tmax=(90.0-latmin)*PI/180.0;
  
  for (i=0;i<n;i++) {
     theta[i]=theta[i]*PI/tmax;
  }

  alpha=PI/tmax;
  kmax=CnvMapIndexLegendre(Lmax,Lmax);
  ecoef=malloc(sizeof(double)*(kmax+2)*n);
  for (i=0;i<(kmax+2)*n;i++) ecoef[i]=0.0;

  for (m=0;m<=Lmax;m++) {
    for (L=m;L<=Lmax;L++) { 
      k3=CnvMapIndexLegendre(L,m);
      k4=CnvMapIndexLegendre(L,m);
      if (k3>=0) {
        for (i=0;i<n;i++) if (theta[i] !=0.0) ecoef[k4*n+i]=ecoef[k4*n+i]-
	   a(k3)*alpha*L*cos(theta[i])/sin(theta[i])/Radial_Dist;
      }
      if (L < Lmax) k1=CnvMapIndexLegendre(L+1,m);
      else k1=-1;
      k2=CnvMapIndexLegendre(L,m);
      if (k1 >=0) {
        for (i=0;i<n;i++) if (theta[i] !=0.0) ecoef[k2*n+i]=ecoef[k2*n+i]+
	   a(k1)*alpha*(L+1+m)/sin(theta[i])/Radial_Dist;
      }
      if (m > 0) {
        if (k3 >=0) k3=k3+1;
        k4=k4+1;
        if (k1 >=0) k1=k1+1;
        k2=k2+1;
	if (k3 >=0) {
          for (i=0;i<n;i++) if (theta[i] !=0.0) ecoef[k4*n+i]=ecoef[k4*n+i]-
	     a(k3)*alpha*L*cos(theta[i])/sin(theta[i])/Radial_Dist;
        }
        if (k1 >=0) {
          for (i=0;i<n;i++) if (theta[i] !=0.0) ecoef[k2*n+i]=ecoef[k2*n+i]+
             a(k1)*alpha*(L+1+m)/sin(theta[i])/Radial_Dist;
	}
      }
    }
  }
  return ecoef;
}

     
double *CnvMapEvalPhiCoef(int Lmax,double *coef,double *theta,int n) { 

  int kmax,i,m,L;
  int k3,k4;
  double *ecoef;

  kmax=CnvMapIndexLegendre(Lmax,Lmax);
  ecoef=malloc(sizeof(double)*(kmax+2)*n);
  for (i=0;i<(kmax+2)*n;i++) ecoef[i]=0.0;

  for (m=1;m<=Lmax;m++) {
    for (L=m;L<=Lmax;L++) {
      k3=CnvMapIndexLegendre(L,m);
      k4=CnvMapIndexLegendre(L,m);
      if (k3 >=0) {
	    for (i=0;i<n;i++) if (theta[i] !=0.0) {
          ecoef[k4*n+i]=ecoef[k4*n+i]-a(k3+1)*m/sin(theta[i])/Radial_Dist;
          ecoef[(k4+1)*n+i]=ecoef[(k4+1)*n+i]+a(k3)*m/sin(theta[i])/
                            Radial_Dist;
        }
      }
    }
  }
  return ecoef;
}      


void CnvMapEvalComponent(int Lmax,double *ecoef,double *plm,
                         double *phi,int n,double *ecomp) {

  int i,m,L,k; 
  for (i=0;i<n;i++) ecomp[i]=0.0;
  for (m=0;m<=Lmax;m++) {
    for (L=m;L<=Lmax;L++) {
       k=CnvMapIndexLegendre(L,m);
       for (i=0;i<n;i++) {
         if (m==0) ecomp[i]=ecomp[i]+ecoef[k*n+i]*PLM(L,m,i);
         else ecomp[i]=ecomp[i]+ecoef[k*n+i]*PLM(L,m,i)*cos(m*phi[i])+
              ecoef[(k+1)*n+i]*PLM(L,m,i)*sin(m*phi[i]); 
       }
    }
  }
}


void CnvMapEvalVelocity(int Lmax,double *coef,double *plm,
                        struct CnvGrid *vptr,double latmin,
                        struct CnvMapData *ptr,float decyear,
                        int magflg) {

  int i;
  double *theta,*phi;
  double *etc,*epc;
  double bpolar;
  double bmag=-0.5e-4;
  double *ex; 
  double *ey;
  double lon,lat,tmp=0;

  double vx,vy;  

  theta=malloc(vptr->num*sizeof(double));
  if (theta==NULL) return;
  phi=malloc(vptr->num*sizeof(double));
  if (phi==NULL) {
    free(theta);
    return;
  }
  ex=malloc(vptr->num*sizeof(double));
  if (ex==NULL) {
    free(theta);
    free(phi);
    return;
  }
  ey=malloc(vptr->num*sizeof(double));
  if (ey==NULL) {
    free(theta);
    free(phi);
    free(ex);
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
  CnvMapEvalComponent(Lmax,etc,plm,phi,vptr->num,ex);
  CnvMapEvalComponent(Lmax,epc,plm,phi,vptr->num,ey);
  
  for (i=0;i<vptr->num;i++) {

    if (ptr->hemisphere == -1) bpolar = BNorth;
    else bpolar = BSouth;

    if (ptr->noigrf) { /* use dipole value for B */
      bmag = bpolar*(1.0 - 3.0 * Altitude/Re)*
             sqrt(3.0*(cos(theta[i])*cos(theta[i])) + 1.0)/2.0;
    } else {
      bmag = -calc_bmag(vptr->lat[i],vptr->lon[i],decyear,magflg);
    }

    vx=ey[i]/bmag;
    vy=-ex[i]/bmag;

    vptr->mag[i]=sqrt(vx*vx+vy*vy);
    vptr->azm[i]=atan2(vy,-vx)*180.0/PI;
    
    if (ptr->hemisphere==-1) vptr->azm[i]=atan2(vy,vx)*180.0/PI;
    
  }
  free(ex);
  free(ey);
  free(phi);
  free(theta); 
  free(epc);
  free(etc);

}

