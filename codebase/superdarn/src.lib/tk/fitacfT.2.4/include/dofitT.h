/* dofit.h
   ========
   Author: R.J.Barnes
*/

#ifndef _DOFITT_H
#define _DOFITT_H

int do_fit(struct FitBlock *iptr, int lag_lim, int goose,
           struct FitRange *ptr, struct FitRange *xptr,
           struct FitElv *elv, struct FitNoise *nptr, int print);

#endif
