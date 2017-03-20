/* indexlegendre.c
   =============== 
   Author: R.J.Barnes
*/


/*
   See license.txt
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
