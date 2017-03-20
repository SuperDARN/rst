/* fitread.h
   ========= 
   Author: R.J.Barnes
*/

/*
   See license.txt
*/




#ifndef _FITREAD_H
#define _FITREAD_H

int FitDecode(struct DataMap *ptr,struct FitData *);
int FitFread(FILE *fp,struct RadarParm *,struct FitData *);
int FitRead(int fid,struct RadarParm *,struct FitData *);

#endif

