/* oldfitscan.h
   ============
   Author: R.J.Barnes
*/

/*
   See license.txt
*/



#ifndef _OLDFITSCAN_H
#define _OLDFITSCAN_H

int OldFitReadRadarScan(struct OldFitFp *fp,int *state,
                 struct RadarScan *ptr,struct RadarParm *prm,
                 struct FitData *fit,int tlen,
                 int lock,int chn);



#endif


