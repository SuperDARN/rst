/* gtablewrite.h
   ================
   Author: R.J.Barnes
*/


/*
   See license.txt
*/




#ifndef _GTABLEWRITE_H
#define _GTABLEWRITE_H

int GridTableWrite(int fp,struct GridTable *ptr,char *log,int xtd);
int GridTableFwrite(FILE *fp,struct GridTable *ptr,char *log,int xtd);

#endif
