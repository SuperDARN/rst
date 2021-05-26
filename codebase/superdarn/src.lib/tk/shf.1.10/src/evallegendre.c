/* evallegendre.c
   ============== 
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


#include <math.h>
#include <stdio.h>
#include "evallegendre.h"





double CnvMapEvalPLM(int l,int m,double x) {
  double pmm=1.0,pnmp1=0,pll=0;
  double somx2=0;
  double fact=0;
  int i=0,ll=0;
  if (m>0) { 
    somx2=sqrt((1.0-x)*(1.0+x));
    fact=1.0;
    for (i=1;i<=m;i++) {
      pmm=-pmm*fact*somx2;
      fact+=2.0;
    }
  }
  if (l==m) return pmm;
  else {
    pnmp1=x*(2*m+1)*pmm;
    if (l==(m+1)) return pnmp1;
    else {
      for (ll=m+2;ll<=l;ll++) {
        pll=(x*(2*ll-1)*pnmp1-(ll+m-1)*pmm)/(ll-m);
        pmm=pnmp1;
        pnmp1=pll;
      }
    }
  }
  return pll;
}


void CnvMapEvalLegendre(int Lmax, double *x,int n, double *plm) {
 
  int L,m,i;
  
  for (i=0; i<n; ++i) {
    for (L=0;L<=Lmax;L++) {
      for (m=0;m<=Lmax;m++) {
         PLM(L,m,i)=CnvMapEvalPLM(L,m,x[i]);
      }
    }
  }
}
    
