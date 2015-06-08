/* badlags.h
   =========
   Author: R.J.Barnes
*/



#ifndef _BADLAGS_H
#define _BADLAGS_H

#include "fitblk.h"
#include "badsmp.h"

#define BAD 1
#define GOOD 0
 
void FitACFMarkBadSamples(struct FitPrm *fitted_prms,struct FitACFBadSample *samples);
void FitACFBadlagsStereo(struct FitPrm *ptr,struct FitACFBadSample *bptr);
void FitACFMarkBadLags(int range,int *lag,struct FitACFBadSample *samples,
	         struct FitPrm *fitted_prms);

#endif
