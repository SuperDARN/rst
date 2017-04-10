/* rawindex.h
   ========== 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/



#ifndef _RAWINDEX_H
#define _RAWINDEX_H


struct RawIndex {
  int num;
  double *tme;
  int *inx;
};

void RawIndexFree(struct RawIndex *inx);
struct RawIndex *RawIndexLoad(int fid);
struct RawIndex *RawIndexFload(FILE *fp);
 


#endif
