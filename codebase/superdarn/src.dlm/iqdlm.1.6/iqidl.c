/* iqidl.c
   ======== 
   Author R.J.Barnes
*/
/*
Copyright (C) <year>  <name of author>

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
#include <unistd.h>
#include <zlib.h>

#include "idl_export.h"

#include "rtypes.h"
#include "dmap.h"
#include "rprm.h"
#include "iq.h"
#include "rprmidl.h"
#include "iqidl.h"

void IDLCopyIQFromIDL(struct IQIDL *iiq,
                           struct IQ *iq) {
 
  int n;

  struct timespec tval[MAXNAVE];
  int atten[MAXNAVE];
  int offset[MAXNAVE];
  int size[MAXNAVE];
  int badtr[MAXNAVE];


  iq->revision.major=iiq->revision.major;
  iq->revision.minor=iiq->revision.minor;
  iq->chnnum=iiq->chnnum;
  iq->smpnum=iiq->smpnum;
  iq->skpnum=iiq->skpnum;
  iq->seqnum=iiq->seqnum;
  iq->tbadtr=iiq->tbadtr;

   for (n=0;n<iq->seqnum;n++) {
    tval[n].tv_sec=iiq->tval[n].sec;
    tval[n].tv_nsec=iiq->tval[n].nsec;
    atten[n]=iiq->atten[n];
    offset[n]=iiq->offset[n];
    size[n]=iiq->size[n];
    badtr[n]=iiq->badtr[n];
  }
  IQSetTime(iq,iq->seqnum,tval);
  IQSetAtten(iq,iq->seqnum,atten);
  IQSetNoise(iq,iq->seqnum,iiq->noise);
  IQSetOffset(iq,iq->seqnum,offset);
  IQSetSize(iq,iq->seqnum,size);
  IQSetBadTR(iq,iq->seqnum,badtr);

}

void IDLCopyIQToIDL(struct IQ *iq,struct IQIDL *iiq) {

  int n;
  iiq->revision.major=iq->revision.major;
  iiq->revision.minor=iq->revision.minor;
  iiq->chnnum=iq->chnnum;
  iiq->smpnum=iq->smpnum;
  iiq->skpnum=iq->skpnum;
  iiq->seqnum=iq->seqnum;
  iiq->tbadtr=iq->tbadtr;

  for (n=0;n<iq->seqnum;n++) {
    iiq->tval[n].sec=iq->tval[n].tv_sec;
    iiq->tval[n].nsec=iq->tval[n].tv_nsec;
    iiq->atten[n]=iq->atten[n];
    iiq->noise[n]=iq->noise[n];
    iiq->offset[n]=iq->offset[n];
    iiq->size[n]=iq->size[n];
    if (iq->badtr !=NULL) iiq->badtr[n]=iq->badtr[n];
  }
}

struct IQIDL *IDLMakeIQ(IDL_VPTR *vptr) {
  
  void *s=NULL;
  
  static IDL_MEMINT ndim[]={1,MAXNAVE};
 
  static IDL_STRUCT_TAG_DEF revision[]={
    {"MAJOR",0,(void *) IDL_TYP_LONG},
    {"MINOR",0,(void *) IDL_TYP_LONG},
    {0}};

  static IDL_STRUCT_TAG_DEF ttime[]={
    {"SEC",0,(void *) IDL_TYP_LONG},
    {"NSEC",0,(void *) IDL_TYP_LONG},
    {0}};
  
  static IDL_STRUCT_TAG_DEF iqdata[]={    
    {"REVISION",0,NULL},   /* 0 */
    {"CHNNUM",0,(void *) IDL_TYP_LONG},   /* 1 */ 
    {"SMPNUM",0,(void *) IDL_TYP_LONG}, /* 2 */
    {"SKPNUM",0,(void *) IDL_TYP_LONG}, /* 3 */
    {"SEQNUM",0,(void *) IDL_TYP_LONG}, /* 4 */
    {"TBADTR",0,(void *) IDL_TYP_LONG}, /* 5 */
    {"TVAL",ndim,0}, /* 6 */
    {"ATTEN",ndim,(void *) IDL_TYP_INT}, /* 7 */
    {"NOISE",ndim,(void *) IDL_TYP_FLOAT}, /* 8 */
    {"OFFSET",ndim,(void *) IDL_TYP_LONG}, /* 9 */
    {"SIZE",ndim,(void *) IDL_TYP_LONG}, /* 10 */
    {"BADTR",ndim,(void *) IDL_TYP_LONG}, /* 11 */
    {0}};

  static IDL_MEMINT ilDims[IDL_MAX_ARRAY_DIM];
 
  iqdata[0].type=IDL_MakeStruct("RLSTR",revision);
  iqdata[6].type=IDL_MakeStruct("UNXTMSTR",ttime);

  s=IDL_MakeStruct("IQDATA",iqdata);
           
  ilDims[0]=1;
  
  return (struct IQIDL *) IDL_MakeTempStruct(s,1,ilDims,vptr,TRUE);
  
}


struct IQIDLInx *IDLMakeIQInx(int num,IDL_VPTR *vptr) {
  
  void *s=NULL;

  
  static IDL_MEMINT idim[1]={0};
 
  static IDL_STRUCT_TAG_DEF iqinx[]={
    {"TIME",0,(void *) IDL_TYP_DOUBLE},
    {"OFFSET",0,(void *) IDL_TYP_LONG},
    {0}};

   s=IDL_MakeStruct("IQINX",iqinx);  
   idim[0]=num;

   return (struct IQIDLInx *) IDL_MakeTempStruct(s,1,idim,vptr,TRUE);
}
  
