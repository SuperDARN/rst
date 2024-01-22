/* evalvelocity.h
   ============== 
   Author R.J.Barnes
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

#ifndef _EVALVELOCITY_H
#define _EVALVELOCITY_H

double *CnvMapEvalThetaCoef(int Lmax,double *coef,double *theta,int n,
                            double latmin);

double *CnvMapEvalPhiCoef(int Lmax,double *coef,double *theta,int n);

void CnvMapEvalComponent(int Lmax,double *ecoef,double *plm,
                         double *phi,int n,double *ecomp);

void CnvMapEvalVelocity(int Lmax,double *coef,double *plm,
                        struct CnvGrid *vptr,double latmin,
                        struct CnvMapData *ptr,float decyear,
                        int magflg);

#endif
