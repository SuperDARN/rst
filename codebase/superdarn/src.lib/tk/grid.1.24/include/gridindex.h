/* gridindex.h
   =========== 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/



#ifndef _GRIDINDEX_H
#define _GRIDINDEX_H


struct GridIndex {
  int num;
  double *tme;
  int *inx;
};

void GridIndexFree(struct GridIndex *inx);
struct GridIndex *GridIndexLoad(int fid);
struct GridIndex *GridIndexFload(FILE *fp);
 


#endif
