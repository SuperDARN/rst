/* cfitinx.c
   =========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <zlib.h>
#include "rtypes.h"
#include "rconvert.h"
#include "rtime.h"
#include "cfitindex.h"


#define FILE_STEP 256

void CFitIndexFree(struct CFitIndex *ptr) {
  if (ptr==NULL) return;
  if (ptr->tme !=NULL) free(ptr->tme);
  if (ptr->inx !=NULL) free(ptr->inx);
  free(ptr);
}


struct CFitIndex *CFitIndexLoad(int fid) {
  void *tmp;
  double tme;
  int32 inx;
  int st;
  struct CFitIndex *ptr;
  ptr=malloc(sizeof(struct CFitIndex));
  if (ptr==NULL) return NULL;

  ptr->tme=malloc(sizeof(double)*FILE_STEP);
  ptr->inx=malloc(sizeof(int)*FILE_STEP);
  ptr->num=0;
 
  if ((ptr->tme==NULL) || (ptr->inx==NULL)) {
    if (ptr->tme !=NULL) free(ptr->tme);
    if (ptr->inx !=NULL) free(ptr->inx);
    free(ptr);
    return NULL;
  }

  do {
    st=ConvertReadDouble(fid,&tme);
    if (st !=0) break;
    st=ConvertReadInt(fid,&inx);
    if (st !=0) break;


    ptr->tme[ptr->num]=tme;
    ptr->inx[ptr->num]=inx;
    ptr->num++;
    st=1;
    if ((ptr->num % FILE_STEP)==0) {
      int inc;
      inc=FILE_STEP*(ptr->num/FILE_STEP+1);
      tmp=realloc(ptr->tme,sizeof(double)*inc);
      if (tmp==NULL) break;
      ptr->tme=(double *) tmp;
      tmp=realloc(ptr->inx,sizeof(int)*inc);
      if (tmp==NULL) break;
      ptr->inx=(int *) tmp;
    }
  } while (1);
  if (st==1) {
    free(ptr->tme);
    free(ptr->inx);
    free(ptr);
    return NULL;
  }


  tmp=realloc(ptr->tme,sizeof(double)*ptr->num);
  if (tmp==NULL) {
    free(ptr->tme);
    free(ptr->inx);
    free(ptr);
    return NULL;
  }
  ptr->tme=tmp;
  tmp=realloc(ptr->inx,sizeof(int)*ptr->num);
  if (tmp==NULL) {
    free(ptr->tme);
    free(ptr->inx);
    free(ptr);
    return NULL;
  }
  ptr->inx=tmp;
  return ptr;

}

struct CFitIndex *CFitIndexFload(FILE *fp) {
  return CFitIndexLoad(fileno(fp));
}









