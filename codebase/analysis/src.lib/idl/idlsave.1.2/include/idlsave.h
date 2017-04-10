/* idlsave.h
   =========
   Author: R.J.Barnes
*/


/*
   See license.txt
*/




#ifndef _IDLSAVE_H
#define _IDLSAVE_H

struct IDLFile {
  unsigned char zflg;
  union {
    FILE *p;
    gzFile z;
  } fp;
};

struct IDLFile *IDLOpen(char *fname,char zflg);
struct IDLFile *IDLFdopen(int fd,char zflg);
void IDLClose(struct IDLFile *fp);

struct DataMap *IDLReadSave(struct IDLFile *fp);

#endif

