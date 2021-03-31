/* evallegendre.h
   ==============
   Author R.J.Barnes
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




#ifndef _EVALLEGENDRE_H
#define _EVALLEGENDRE_H



#define PLM(L,m,i) *(plm+(m)*n+(L)*(Lmax+1)*n+(i))
#define a(k) *(coef+4*(k)+2)


void CnvMapEvalLegendre(int Lmax,double *x,int n,double *plm);
int CnvMapIndexLegendre(int,int); 

#endif


