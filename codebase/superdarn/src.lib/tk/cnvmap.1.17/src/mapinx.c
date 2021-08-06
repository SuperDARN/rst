/* mapinx.c
   ========
   Author: R.J.Barnes
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/ 


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "zlib.h"
#include "rtypes.h"
#include "rconvert.h"
#include "cnvmapindex.h"

#define FILE_STEP 256

void CnvMapIndexFree(struct CnvMapIndex *ptr) {
  if (ptr==NULL) return;
  if (ptr->tme !=NULL) free(ptr->tme);
  if (ptr->inx !=NULL) free(ptr->inx);
  free(ptr);
}

struct CnvMapIndex *CnvMapIndexLoad(int fid) {
  void *tmp;
  double tme;
  int32 inx;
  int st;
  struct CnvMapIndex *ptr;
  ptr=malloc(sizeof(struct CnvMapIndex));
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

struct CnvMapIndex *CnvMapIndexFload(FILE *fp) {
  return CnvMapIndexLoad(fileno(fp));
}

