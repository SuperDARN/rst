/* indexlegendre.c
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




int CnvMapIndexLegendre(int L,int m) {
  if (L==0) return 0;
  if (m==0) return L*L;
  return (L*L)+(2*m)-1;
}

void CnvMapLegendreIndex(int k,int *L,int *m) {
 
  *L=sqrt(k);
  if (*L**L==k) *m = 0; 
  else *m = (k + 1 - *L**L)/2;
  if (CnvMapIndexLegendre(*L,*m) !=k) *m=-*m;
}
