/* dofit.h
   ========
   Author: R.J.Barnes
*/




#ifndef _DOFIT_H
#define _DOFIT_H

int do_fit(struct FitBlock *iptr,int lag_lim,int goose,
	   struct FitRange *ptr,struct FitRange *xptr,
           struct FitElv *elv,
	   struct FitNoise *nptr);

#endif
