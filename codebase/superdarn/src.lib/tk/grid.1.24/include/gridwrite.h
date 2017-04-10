/* gridwrite.h
   ===========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/



#ifndef _GRIDWRITE_H
#define _GRIDWRITE_H

int GridWrite(int fd,struct GridData *ptr);
int GridFwrite(FILE *fp,struct GridData *ptr);

#endif



