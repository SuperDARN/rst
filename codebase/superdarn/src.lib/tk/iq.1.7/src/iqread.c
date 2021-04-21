/* iqread.c
   ======== 
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
#include <time.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "rprm.h"
#include "iq.h"

int IQDecode(struct DataMap *ptr,struct IQ *iq,
             unsigned int **badtr,int16 **samples) {

  int c,x;

  struct DataMapScalar *s;
  struct DataMapArray *a;


  if (iq->tval !=NULL) free(iq->tval);
  if (iq->atten !=NULL) free(iq->atten);
  if (iq->noise !=NULL) free(iq->noise);
  if (iq->offset !=NULL) free(iq->offset);
  if (iq->size !=NULL) free(iq->size);
  if (iq->badtr !=NULL) free(iq->badtr);

  memset(iq,0,sizeof(struct IQ));
  iq->tval=NULL;
  iq->atten=NULL;
  iq->noise=NULL;
  iq->offset=NULL;
  iq->size=NULL;
  iq->badtr=NULL;

  for (c=0;c<ptr->snum;c++) {
    s=ptr->scl[c];
   
    if ((strcmp(s->name,"iqdata.revision.major")==0) && (s->type==DATAINT))
      iq->revision.major=*(s->data.iptr);
    if ((strcmp(s->name,"iqdata.revision.minor")==0) && (s->type==DATAINT))
      iq->revision.minor=*(s->data.sptr);
   
    if ((strcmp(s->name,"seqnum")==0) && (s->type==DATAINT))
      iq->seqnum=*(s->data.iptr);
    if ((strcmp(s->name,"chnnum")==0) && (s->type==DATAINT))
      iq->chnnum=*(s->data.iptr);
    if ((strcmp(s->name,"smpnum")==0) && (s->type==DATAINT))
      iq->smpnum=*(s->data.iptr);
    if ((strcmp(s->name,"skpnum")==0) && (s->type==DATAINT))
      iq->skpnum=*(s->data.iptr);
    if ((strcmp(s->name,"btnum")==0) && (s->type==DATAINT))
      iq->tbadtr=*(s->data.iptr);
  }
 
  for (c=0;c<ptr->anum;c++) {
    a=ptr->arr[c];
 
    if ((strcmp(a->name,"tsc")==0) && (a->type==DATAINT) &&
        (a->dim==1) && (a->rng[0]>0)) {
      struct timespec *buf=NULL;
      if (iq->tval==NULL) buf=malloc(a->rng[0]*sizeof(struct timespec));
      else buf=realloc(iq->tval,a->rng[0]*sizeof(struct timespec));     
      if (buf==NULL) break;
      iq->tval=buf;
      for (x=0;x<a->rng[0];x++) iq->tval[x].tv_sec=a->data.iptr[x];
    }

    if ((strcmp(a->name,"tus")==0) && (a->type==DATAINT) &&
         (a->dim==1) && (a->rng[0]>0)) {
      struct timespec *buf=NULL;
      if (iq->tval==NULL) buf=malloc(a->rng[0]*sizeof(struct timespec));
      else buf=realloc(iq->tval,a->rng[0]*sizeof(struct timespec));     
      if (buf==NULL) break;
      iq->tval=buf;
      for (x=0;x<a->rng[0];x++) iq->tval[x].tv_nsec=a->data.iptr[x]*1000;
    }

    if ((strcmp(a->name,"tatten")==0) && (a->type==DATASHORT) &&
        (a->dim==1) && (a->rng[0]>0)) {
      int *buf=NULL;
      if (iq->atten==NULL) buf=malloc(a->rng[0]*sizeof(int));
      else buf=realloc(iq->atten,a->rng[0]*sizeof(int));     
      if (buf==NULL) break;
      iq->atten=buf;
      for (x=0;x<a->rng[0];x++) iq->atten[x]=a->data.sptr[x];
    }

    if ((strcmp(a->name,"tnoise")==0) && (a->type==DATAFLOAT) &&
         (a->dim==1) && (a->rng[0]>0)) {
      float *buf=NULL;
      if (iq->noise==NULL) buf=malloc(a->rng[0]*sizeof(float));
      else buf=realloc(iq->noise,a->rng[0]*sizeof(float));     
      if (buf==NULL) break;
      iq->noise=buf;
      for (x=0;x<a->rng[0];x++) iq->noise[x]=a->data.fptr[x];
    }

    if ((strcmp(a->name,"toff")==0) && (a->type==DATAINT) &&
        (a->dim==1) && (a->rng[0]>0)) {
      int *buf=NULL;
      if (iq->offset==NULL) buf=malloc(a->rng[0]*sizeof(int));
      else buf=realloc(iq->offset,a->rng[0]*sizeof(float));     
      if (buf==NULL) break;
      iq->offset=buf;
      for (x=0;x<a->rng[0];x++) iq->offset[x]=a->data.iptr[x];
    }

    if ((strcmp(a->name,"tsze")==0) && (a->type==DATAINT) &&
         (a->dim==1) && (a->rng[0]>0)) {
      int *buf=NULL;
      if (iq->size==NULL) buf=malloc(a->rng[0]*sizeof(int));
      else buf=realloc(iq->size,a->rng[0]*sizeof(float));     
      if (buf==NULL) break;
      iq->size=buf;
      for (x=0;x<a->rng[0];x++) iq->size[x]=a->data.iptr[x];
    }

   if ((strcmp(a->name,"tbadtr")==0) && (a->type==DATAINT) &&
        (a->dim==1) && (a->rng[0]>0)) {
     int *buf=NULL;
     if (iq->badtr==NULL) buf=malloc(a->rng[0]*sizeof(int));
     else buf=realloc(iq->badtr,a->rng[0]*sizeof(float));     
     if (buf==NULL) break;
     iq->badtr=buf;
     for (x=0;x<a->rng[0];x++) iq->badtr[x]=a->data.iptr[x];
   }

   if ((strcmp(a->name,"badtr")==0) && (a->type==DATAINT) &&
        (a->dim==1) && (a->rng[0]>0)) {
      unsigned int *buf=NULL;
      if (*badtr==NULL) buf=malloc(a->rng[0]*sizeof(unsigned int));
      else buf=realloc(*badtr,a->rng[0]*sizeof(unsigned int));
      if (buf==NULL) break;
      *badtr=buf;
      for (x=0;x<a->rng[0];x++) buf[x]=a->data.iptr[x];
    }

    if ((strcmp(a->name,"data")==0) && (a->type==DATASHORT) &&
        (a->dim==1) && (a->rng[0]>0)) {
      int16 *buf=NULL;
      if (*samples==NULL) buf=malloc(a->rng[0]*sizeof(int16));
      else buf=realloc(*samples,a->rng[0]*sizeof(int16));
      if (buf==NULL) break;
      *samples=buf;
      for (x=0;x<a->rng[0];x++) buf[x]=a->data.sptr[x];
    }
  }
      
  return 0;

}

int IQRead(int fid,struct RadarParm *prm,
	   struct IQ *iq,unsigned int **badtr,int16 **samples) {
  int s;
  struct DataMap *ptr;

  ptr=DataMapRead(fid);
  if (ptr==NULL) return -1;
  s=RadarParmDecode(ptr,prm);
  if (s !=0) {
    DataMapFree(ptr);
    return s;
  }
  s=IQDecode(ptr,iq,badtr,samples);
  DataMapFree(ptr);
  return s;
}

int IQFread(FILE *fp,struct RadarParm *prm,
	    struct IQ *iq,unsigned int **badtr,int16 **samples) {
  return IQRead(fileno(fp),prm,iq,badtr,samples);
}

