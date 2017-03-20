/* svdbksb.c
   ========= 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/


#include <stdlib.h>
#include <stdio.h>
#include <math.h>



void CnvMapSVDBKSB(double *u, double *w, double *v, int m, int n, 
	    double *b, double *x) { 
  int jj,j,i; 
  double s,*tmp; 
  tmp=malloc(sizeof(double)*n); 
  for (j=0;j<n;j++) { 
    s=0.0; 
    if (w[j]) {
      for (i=0;i<m;i++) s += u[i+m*j]*b[i]; 
      s /= w[j];
    } tmp[j]=s; 
  } 
  for (j=0;j<n;j++) { 
    s=0.0; 
    for (jj=0;jj<n;jj++) s += v[j+n*jj]*tmp[jj]; 
    x[j]=s; 
  } 
  free(tmp); 
}
