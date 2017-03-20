/* gridread.h
   ==========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/




#ifndef _GRIDREAD_H
#define _GRIDREAD_H

int GridRead(int fid,struct GridData *gp);
int GridFread(FILE *fp,struct GridData *gp);

#endif



