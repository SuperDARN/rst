/* badlags.h
   =========
   Author: R.J.Barnes
*/



#ifndef _BADLAGS_H
#define _BADLAGS_H

#define BAD 1
#define GOOD 0
   
void FitACFBadlags(struct FitPrm *fitted_prms,struct FitACFBadSample *samples);
void FitACFBadlagsStereo(struct FitPrm *ptr,struct FitACFBadSample *bptr);
void FitACFCkRng(int range,int *lag,struct FitACFBadSample *samples,
	         struct FitPrm *fitted_prms);

#endif
