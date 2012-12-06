/* fitacf.h
   ========
   Author: R.J.Barnes
*/

#ifndef _FITACFT_H
#define _FITACFT_H

void FitACFFree(struct FitBlock *fptr); 
struct FitBlock *FitACFMake(struct RadarSite *hd,int year);
void FitACFT(struct RadarParm *prm,struct RawData *ptr,struct FitBlock *input,
	    struct FitData *fit,int print);

#endif
