/* oldgtablewrite.h
   ================
   Author: R.J.Barnes
*/


/*
   See license.txt
*/




#ifndef _OLDGTABLEWRITE_H
#define _OLDGTABLEWRITE_H

int OldGridTableWrite(int fp,struct GridTable *ptr,char *log,int xtd);
int OldGridTableFwrite(FILE *fp,struct GridTable *ptr,char *log,int xtd);

#endif
