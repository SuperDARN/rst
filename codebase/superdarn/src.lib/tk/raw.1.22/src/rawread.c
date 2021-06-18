/* rawread.c
   ========= 
   Author: R.J.Barnes
*/

/*
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
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "rprm.h"
#include "rawdata.h"


int RawDecode(struct DataMap *ptr,struct RawData *raw) {

  int c,n;
 
  int *slist=NULL;
  struct DataMapScalar *s;
  struct DataMapArray *a;
  int snum=0,nrang=0;
  

  if (raw->pwr0 !=NULL) free(raw->pwr0);
  for (n=0;n<2;n++) { 
    if (raw->acfd[n] !=NULL) free(raw->acfd[n]);
    if (raw->xcfd[n] !=NULL) free(raw->xcfd[n]);
  }
  memset(raw,0,sizeof(struct RawData));
  raw->pwr0=NULL;
  for (n=0;n<2;n++) {
    raw->acfd[n]=NULL;
    raw->xcfd[n]=NULL;
  }

  for (c=0;c<ptr->snum;c++) {
    s=ptr->scl[c];
   
    if ((strcmp(s->name,"rawacf.revision.major")==0) && (s->type==DATAINT))
      raw->revision.major=*(s->data.iptr);
    if ((strcmp(s->name,"rawacf.revision.minor")==0) && (s->type==DATAINT))
      raw->revision.minor=*(s->data.iptr);

    if ((strcmp(s->name,"thr")==0) && (s->type==DATAFLOAT))
      raw->thr=*(s->data.fptr);
  }
 
  for (c=0;c<ptr->anum;c++) {
    a=ptr->arr[c];
  
    if ((strcmp(a->name,"slist")==0) && (a->type==DATASHORT) &&
        (a->dim==1)) {
      snum=a->rng[0];
      slist=malloc(sizeof(int)*snum);
      if (slist==NULL) break;
      for (n=0;n<snum;n++) slist[n]=a->data.sptr[n];
    }


    if ((strcmp(a->name,"pwr0")==0) && (a->type==DATAFLOAT) &&
        (a->dim==1)) {
      nrang=a->rng[0];
      RawSetPwr(raw,nrang,a->data.fptr,0,NULL);
    }
  }

  if ((snum==0) || (slist==NULL)) return 0;

  for (c=0;c<ptr->anum;c++) {
    a=ptr->arr[c];
    if ((strcmp(a->name,"acfd")==0) && (a->type==DATAFLOAT) &&
	(a->dim==3)) 
      RawSetACF(raw,nrang,a->rng[1],a->data.fptr,snum,slist);
 
    if ((strcmp(a->name,"xcfd")==0) && (a->type==DATAFLOAT) &&
	(a->dim==3)) 
      RawSetXCF(raw,nrang,a->rng[1],a->data.fptr,snum,slist);
  }
  free(slist);
  return 0;

}


int RawRead(int fid,struct RadarParm *prm,struct RawData *raw) {

  int s;
  struct DataMap *ptr;

  ptr=DataMapRead(fid);
  if (ptr==NULL) return -1;
  s=RadarParmDecode(ptr,prm);
  if (s !=0) {
    DataMapFree(ptr);
    return s;
  }
  s=RawDecode(ptr,raw);
  DataMapFree(ptr);
  return s;
}

int RawFread(FILE *fp,struct RadarParm *prm,
              struct RawData *raw) {
  return RawRead(fileno(fp),prm,raw);
}
