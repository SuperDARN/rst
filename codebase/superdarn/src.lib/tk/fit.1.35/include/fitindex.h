/* fitindex.h
   ========== 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/



#ifndef _FITINDEX_H
#define _FITINDEX_H


struct FitIndex {
  int num;
  double *tme;
  int *inx;
};

void FitIndexFree(struct FitIndex *inx);
struct FitIndex *FitIndexLoad(int fid);
struct FitIndex *FitIndexFload(FILE *fp);
 


#endif
