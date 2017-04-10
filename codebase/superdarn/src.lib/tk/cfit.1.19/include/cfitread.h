/* cfitread.h
   =========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/




#ifndef _CFITREAD_H
#define _CFITREAD_H

struct CFitfp {
  gzFile fp;
  unsigned char *fbuf;
  int fptr;
  int fsze;
  int blen;
  double ctime;
};

int CFitRead(struct CFitfp *fptr,struct CFitdata *ptr);
struct CFitfp *CFitOpen(char *name);
void CFitClose(struct CFitfp *fptr);

#endif






