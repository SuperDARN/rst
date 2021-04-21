/* svdbksb.c
   ========= 
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
