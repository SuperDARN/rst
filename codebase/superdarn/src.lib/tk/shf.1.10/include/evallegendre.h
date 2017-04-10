/* evallegendre.h
   ==============
   Author R.J.Barnes
*/


/*
   See license.txt
*/




#ifndef _EVALLEGENDRE_H
#define _EVALLEGENDRE_H



#define PLM(L,m,i) *(plm+(m)*n+(L)*(Lmax+1)*n+(i))
#define a(k) *(coef+4*(k)+2)


void CnvMapEvalLegendre(int Lmax,double *x,int n,double *plm);
int CnvMapIndexLegendre(int,int); 

#endif


