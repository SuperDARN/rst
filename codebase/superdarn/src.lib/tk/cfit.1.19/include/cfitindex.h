/* cfitindex.h
   =========== 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/

#ifndef _CFITINDEX_H
#define _CFITINDEX_H



struct CFitIndex {
  int num;
  double *tme;
  int *inx;
};

void CFitIndexFree(struct CFitIndex *inx);
struct CFitIndex *CFitIndexLoad(int fid);
struct CFitIndex *CFitIndexFload(FILE *fp);

#endif
