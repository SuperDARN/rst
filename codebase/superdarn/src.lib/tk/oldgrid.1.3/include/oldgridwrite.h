/* oldgridwrite.h
   ==============
   Author: R.J.Barnes
*/

/*
   See license.txt
*/



#ifndef _OLDGRIDWRITE_H
#define _OLDGRIDWRITE_H

int OldGridEncodeOne(FILE *fp,struct GridData *ptr);
int OldGridEncodeTwo(FILE *fp,struct GridData *ptr);
int OldGridEncodeThree(FILE *fp,struct GridData *ptr);
int OldGridFwrite(FILE *fp,struct GridData *ptr);

#endif



