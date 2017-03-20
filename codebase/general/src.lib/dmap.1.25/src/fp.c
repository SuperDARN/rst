/* dmap.c
   ====== 
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <zlib.h>
#include "rtypes.h"
#include "rconvert.h"
#include "dmap.h"

struct DataMapFp *DataMapOpen(char *fname,int zflg,char *mode) {
  struct DataMapFp *ptr=NULL;
  FILE *fp;
  ptr=malloc(sizeof(struct DataMapFp));
  ptr->zflg=zflg;
  ptr->size=0;
  fp=stdout;  

  if (fname !=NULL) {
    ptr->sflg=0;
    if (zflg) {
      ptr->fp.z=gzopen(fname,mode);
      if (ptr->fp.z==0) {
        free(ptr);
        return NULL;
      }
    } else {
      ptr->fp.f=fopen(fname,mode);
      if (ptr->fp.f==NULL) {
        free(ptr);
        return NULL;
      }
    }
  } else {    
    ptr->sflg=1;
    if ((mode !=NULL) && (mode[0]=='r')) fp=stdin;
    if (zflg) ptr->fp.z=gzdopen(fileno(fp),mode);
    else ptr->fp.f=fp;
  }
  return ptr;
}

void DataMapClose(struct DataMapFp *fp) {
  if (fp->zflg) gzclose(fp->fp.z);
  else  if (!fp->sflg) fclose(fp->fp.f);
  free(fp);
}

struct DataMap *DataMapGet(struct DataMapFp *fp) {
  if (fp->zflg) return DataMapReadBlockZ(fp->fp.z,&fp->size);
  return DataMapFreadBlock(fp->fp.f,&fp->size);
}

int DataMapPut(struct DataMapFp *fp,struct DataMap *ptr) {
  if (fp==NULL) return -1;
  if (ptr==NULL) return -1;
  if (fp->zflg) return DataMapWriteZ(fp->fp.z,ptr);
  return DataMapFwrite(fp->fp.f,ptr);
}
