/* nrfit.h
   =====
   Author: R.J.Barnes
*/


/*
   See license.txt
*/




#ifndef _NRFIT_H
#define _NRFIT_H

void nrfit(float *x,float *y,int ndata,float *sig,int mwt,
         float *a,float *b,float *siga,float *sigb,float *chi2,
	 float *q);

#endif













