/* rawwrite.h
   ========== 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/




#ifndef _RAWWRITE_H
#define _RAWWRITE_H

int RawEncode(struct DataMap *ptr,struct RadarParm *prm,struct RawData *raw);
int RawFwrite(FILE *fp,struct RadarParm *,struct RawData *);
int RawWrite(int,struct RadarParm *,struct RawData *);

#endif
