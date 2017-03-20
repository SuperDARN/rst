/* smrscan.h
   =========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/




#ifndef _SMRSCAN_H
#define _SMRSCAN_H

int SmrRadarScan(FILE *fp,int *state,
                 struct RadarScan *ptr,struct RadarParm *prm,
                 struct FitData *fit,int fbeam,
                 int tlen,
                 int lock,int chn);


#endif






