/* fitacf.h
   ========
   Author: R.J.Barnes
*/

#ifndef _FITACF_H
#define _FITACF_H

void FitACFFree(struct FitBlock *fit_blk); 
struct FitBlock *FitACFMake(struct RadarSite *radar_site,int year);
void FitACF(struct RadarParm *radar_prms,struct RawData *raw_data,struct FitBlock *fit_blk,
	    struct FitData *fit_data);

#endif
