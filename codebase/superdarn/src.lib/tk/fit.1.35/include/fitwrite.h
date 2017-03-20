/* fitwrite.h
   ========== 
   Author: R.J.Barnes
*/

/*
   See license.txt
*/



#ifndef _FITWRITE_H
#define _FITWRITE_H

int FitEncode(struct DataMap *ptr,struct RadarParm *prm, struct FitData *fit);
int FitFwrite(FILE *fp,struct RadarParm *,struct FitData *);
int FitWrite(int fid,struct RadarParm *,struct FitData *);

#endif
