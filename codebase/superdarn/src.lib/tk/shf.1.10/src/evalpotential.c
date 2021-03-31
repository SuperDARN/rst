/* evalpotential.c
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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "evallegendre.h"




void CnvMapEvalPotential(int Lmax,int n,double *coef,double *plm,
		    double *phi,double *v) {

  int i,m,k,L;

  for (i=0;i<n;i++) v[i]=0.0;
  for (m=0;m<=Lmax;m++) {
    for (L=m;L<=Lmax;L++) {
      k=CnvMapIndexLegendre(L,m);
      for (i=0;i<n;i++) {
        if (m==0) v[i]=v[i]+a(k)*PLM(L,0,i);
        else v[i]=v[i]+a(k)*cos(m*phi[i])*PLM(L,m,i)+
                  a(k+1)*sin(m*phi[i])*PLM(L,m,i);
      }
    }
  }
}












