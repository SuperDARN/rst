/* fitacf.h
   ========
   Author: R.J.Barnes
*/

#ifndef _FITACF_H
#define _FITACF_H

#include "fitdata.h"
#include "radar.h"
#include "rawdata.h"
#include "rprm.h"

void FitACFFree(struct FitBlock *fptr); 
struct FitBlock *FitACFMake(struct RadarSite *hd,int year);
void FitACF(struct RadarParm *prm,struct RawData *ptr,struct FitBlock *input,
	    struct FitData *fit, struct fitacf_functions *fit_func);

#endif
