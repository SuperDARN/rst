/* fitvector.c
   =========== 
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

#include "rmath.h"
#include "shfvector.h"
#include "fitvector.h"

#include "svdcmp.h"
#include "svdbksb.h"
#include "shfconst.h"



void CnvMapEvalLegendre(int Lmax,double *x,int n,double *plm);
int CnvMapIndexLegendre(int,int); 
void CnvMapLegendreIndex(int,int *,int *);

 
#define PLM(L,m,i) *(plm+(m)*num+(L)*(order+1)*num+(i))

double *CnvMapVlosMatrix(int num,struct CnvMapSHFVec *data,
			 int order,double latmin) {
  int kmax;
  int i,m,L,k;

  double bpolar,bmag=-0.5e-4;
  double thetamax;
  double alpha;
  double etheta,ephi;
  double vtheta,vphi;  
  double vlos;

  double *x=NULL;
  double *y=NULL;
  double *plm=NULL;
  double *a=NULL;
  double *phi=NULL;
  double *theta=NULL;

  x=malloc(sizeof(double)*num);
  if (x==NULL) return NULL;
  y=malloc(sizeof(double)*num);
  if (y==NULL) return NULL;
  phi=malloc(sizeof(double)*num);
  if (phi==NULL) return NULL;
  theta=malloc(sizeof(double)*num);
  if (theta==NULL) return NULL;

  plm=malloc(sizeof(double)*(order+1)*(order+1)*num);
  if (plm==NULL) {
    free(x);
    return NULL;
  }

  kmax=CnvMapIndexLegendre(order,order)+1;

  if (latmin>0) {

    bpolar = BNorth;

    thetamax=(90-latmin)*PI/180;
    alpha=PI/thetamax;
    for (i=0;i<num;i++) {
      x[i]=cos(alpha*(90.0-data[i].lat)*PI/180);
      y[i]=sin(alpha*(90.0-data[i].lat)*PI/180);
      phi[i]=data[i].lon*PI/180.0;
      theta[i]=(90.0-data[i].lat)*PI/180.0;
      phi[i]=(phi[i]>PI) ? (phi[i]-2.0*PI) : phi[i];
    }
  } else {

    bpolar = BSouth;

    thetamax=(90+latmin)*PI/180;
    alpha=PI/thetamax;
    for (i=0;i<num;i++) {
      x[i]=cos(alpha*(90.0+data[i].lat)*PI/180);
      y[i]=sin(alpha*(90.0+data[i].lat)*PI/180);
      phi[i]=data[i].lon*PI/180.0;
      theta[i]=(90.0+data[i].lat)*PI/180.0;
      phi[i]=(phi[i]>PI) ? (phi[i]-2.0*PI) : phi[i];
    }
  }


  CnvMapEvalLegendre(order,x,num,plm);
  
  a=malloc(sizeof(double)*num*(kmax+1)); 
  memset(a,0,sizeof(double)*num*(kmax+1));
  if (a==NULL) {
    free(plm);
    free(x);
    free(y);
    free(phi);
    free(theta);
    return NULL;
  } 
  m=0;
  for (L=1;L<=order;L++) {
    k=CnvMapIndexLegendre(L,m);
    for (i=0;i<num;i++) {

      bmag = bpolar*(1.0 - 3.0 * Altitude/Re)*
             sqrt(3.0*(cos(theta[i])*cos(theta[i]))
	          + 1.0)/2.0;

      etheta=alpha/Radial_Dist*L*(PLM(L-1,0,i)-x[i]*PLM(L,0,i))/y[i];
      vphi=-etheta/(bmag);  
      vlos=vphi*data[i].sin;
      a[k*num+i]=vlos;
    }
  }
  for (m=1;m<=order;m++) {
    for (L=m;L<=order;L++) {
      k=CnvMapIndexLegendre(L,m);
      for (i=0;i<num;i++) {

        bmag = bpolar*(1.0 - 3.0 * Altitude/Re)*
                       sqrt(3.0*(cos(theta[i])*cos(theta[i]))+ 1.0)/2.0;

        etheta=-(cos(m*phi[i])/y[i]*(-((L+m)*PLM(L-1,m,i))+
			             L*x[i]*PLM(L,m,i)));
        etheta=etheta*alpha/Radial_Dist;
        ephi=m*PLM(L,m,i)*sin(m*phi[i])/sin(theta[i]);
        ephi=ephi/Radial_Dist;
        vtheta=ephi/bmag;
        vphi=-etheta/bmag;
        
        vlos=vtheta*data[i].cos+vphi*data[i].sin;
        a[k*num+i]=vlos;                    
        etheta=-(sin(m*phi[i])/y[i]*(-((L+m)*PLM(L-1,m,i))+
			             L*x[i]*PLM(L,m,i)));
        etheta=etheta*alpha/Radial_Dist;
        ephi=-m*(cos(m*phi[i])/sin(theta[i])*PLM(L,m,i));
        ephi=ephi/Radial_Dist;
        vtheta=ephi/bmag;
        vphi=-etheta/bmag;
        
        vlos=vtheta*data[i].cos+vphi*data[i].sin;
     
        a[num*(k+1)+i]=vlos;  
       
     
      }
    }
  }
  free(plm);
  free(x);
  free(y);
  free(theta);
  free(phi);
  return a;
}

double CnvMapFitVector(int num,struct CnvMapSHFVec *data,
		       double *coef,double *fitvel,
		        int order,double latmin) {

  int kmax;
  int i,k,n,L,m;
  double t,e;
  double wmax=0;
  double *amat=NULL;
  double *a;
  double *v,*w;
  double *result=NULL;
  double *soltn=NULL;
  double *plm=NULL;
  double *var=NULL;
  double x,chi_sqr=0;

  kmax=CnvMapIndexLegendre(order,order)+1;

  result=malloc(sizeof(double)*num);
  if (result==NULL) return -1;
  
  soltn=malloc(sizeof(double)*(kmax+1));
  if (soltn==NULL) {
    free(result);
    return -1;
  }

  plm=malloc(sizeof(double)*(order+1)*(order+1));
  if (plm==NULL) {
    free(result);
    free(soltn);
    return -1;
  }

  w=malloc(sizeof(double)*(kmax+1));
  v=malloc(sizeof(double)*(kmax+1)*(kmax+1));
  var=malloc(sizeof(double)*(kmax+1)*(kmax+1));
  a=malloc(sizeof(double)*num*(kmax+1)); 

  amat=CnvMapVlosMatrix(num,data,order,latmin);
  if (amat==NULL) { 
    free(result);
    free(soltn);
    free(plm);
    return -1;
  }
  
  for (i=0;i<num;i++) {
    result[i]=data[i].vlos/data[i].verr;
    for (k=0;k<=kmax;k++) {
      amat[k*num+i]=amat[k*num+i]/data[i].verr; 
    }
  }
  memcpy(a,amat,sizeof(double)*num*(kmax+1));
 
  /* perform SVD on the matrix of equations */

  CnvMapSVDCMP(a,num,kmax+1,w,v); 
     
  n=(num<(kmax+1)) ? (kmax+1) : num;
  for (i=0;i<=kmax;i++) if (fabs(w[i])>wmax) wmax=fabs(w[i]);
  for (i=0;i<=kmax;i++) if (fabs(w[i])<(wmax*n*2e-16)) w[i]=0;
 
  CnvMapSVDBKSB(a,w,v,num,kmax+1,result,soltn);
  
  for (i=0;i<num;i++) { /* calculate chi-squared error estimate */
    e=0;
    for (k=0;k<=kmax;k++) e+=amat[k*num+i]*soltn[k];
    fitvel[i]=e*data[i].verr;    
    chi_sqr+=(e-result[i])*(e-result[i]);
  }

  for (i=0;i<=kmax;i++) { /* calculate the variance of the solution */
    t=0;
    for (k=0;k<=kmax;k++) 
      if (w[k] !=0) t+=(v[k+i*(kmax+1)]/w[k])*(v[k+i*(kmax+1)]/w[k]);
    var[i]=t;
  }
  x=-1;
  CnvMapEvalLegendre(order,&x,1,plm); 
   
  for (k=0;k<=kmax;k++) {
    CnvMapLegendreIndex(k,&L,&m);
    coef[4*k]=L;
    coef[4*k+1]=m;
    coef[4*k+2]=soltn[k];
    coef[4*k+3]=sqrt(var[k]);
    if (m==0) coef[2]-=coef[4*k+2]*plm[(order+1)*L];
  }
  free(a);
  free(var);
  free(w);
  free(v);  
  free(soltn);
  free(result);
  free(amat);
  free(plm);
  return chi_sqr;
}  






















