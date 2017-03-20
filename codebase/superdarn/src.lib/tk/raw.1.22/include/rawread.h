/* rawread.h
   ========= 
   Author: R.J.Barnes
*/


/* 
   See license.txt
*/




#ifndef _RAWREAD_H
#define _RAWREAD_H


int RawDecode(struct DataMap *ptr,struct RawData *raw);
int RawFread(FILE *fp,struct RadarParm *,struct RawData *);
int RawRead(int fid,struct RadarParm *,struct RawData *);

#endif
