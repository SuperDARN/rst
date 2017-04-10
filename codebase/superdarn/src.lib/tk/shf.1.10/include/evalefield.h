/* evalefield.h
   ============  
   Author: R.J.Barnes
*/

/*
   See license.txt
*/




#ifndef _EVALEFIELD_H
#define _EVALEFIELD_H

void CnvMapEvalEfield(int Lmax,double *coef,double *plm,
		     struct CnvGrid *vptr,double latmin,
		     struct CnvMapData *ptr);

#endif
