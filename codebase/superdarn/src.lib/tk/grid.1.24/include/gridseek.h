/* gridread.h
   ==========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/




#ifndef _GRIDSEEK_H
#define _GRIDSEEK_H

int GridSeek(int fid,
	     int yr,int mo,int dy,int hr,int mt,int sc,double *atme,
	     struct GridIndex *inx);
int GridFseek(FILE *fp,
	      int yr,int mo,int dy,int hr,int mt,int sc,double *atme,
	      struct GridIndex *inx);

#endif



