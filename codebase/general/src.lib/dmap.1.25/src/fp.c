/* dmap.c
   ====== 
   Author: R.J.Barnes
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
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
