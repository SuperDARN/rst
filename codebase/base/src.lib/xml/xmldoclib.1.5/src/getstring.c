/* getstring.c
   =========== 
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rxml.h"
#include "rxmldb.h"



char *getDBstring(struct XMLDBbuffer *ptr) {
  int s=0;
  char *str=NULL;
  char zero[1];
  struct XMLDBbuffer *decode=NULL;  

  zero[0]=0;
  decode=XMLDBCopyBuffer(ptr);
  if (decode==NULL) return NULL;
  s=XMLDBAddBuffer(decode,zero,1);
  if (s !=0) {
    XMLDBFreeBuffer(decode);
    return NULL;
  }
  str=decode->buf;
  free(decode);
  return str;
}

