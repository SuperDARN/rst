/* cnvmapseek.h
   ============
   Author: R.J.Barnes
*/

/*
   See license.txt
*/




#ifndef _CNVMAPSEEK_H
#define _CNVMAPSEEK_H

int CnvMapSeek(int fid,
	     int yr,int mo,int dy,int hr,int mt,int sc,double *atme,
	     struct CnvMapIndex *inx);
int CnvMapFseek(FILE *fp,
	      int yr,int mo,int dy,int hr,int mt,int sc,double *atme,
	      struct CnvMapIndex *inx);

#endif



