/* cnvmapindex.h
   ============= 
   Author: R.J.Barnes
*/


/*
   See license.txt
*/



#ifndef _CNVMAPINDEX_H
#define _CNVMAPINDEX_H


struct CnvMapIndex {
  int num;
  double *tme;
  int *inx;
};

void CnvMapIndexFree(struct CnvMapIndex *);
struct CnvMapIndex *CnvMapIndexLoad(int fid);
struct CnvMapIndex *CnvMapIndexFload(FILE *fp);
 


#endif
