/* grdidl.c
   ======== 
   Author R.J.Barnes
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
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
#include "rtime.h"
#include "dmap.h"
#include "griddata.h"
#include "grdidl.h"

void IDLCopyGridPrmFromIDL(struct GridIDLPrm *iprm,
                           struct GridData *grd) {

  grd->st_time=TimeYMDHMSToEpoch(iprm->start.yr,iprm->start.mo,iprm->start.dy,
                                iprm->start.hr,iprm->start.mt,iprm->start.sc);
  grd->ed_time=TimeYMDHMSToEpoch(iprm->end.yr,iprm->end.mo,iprm->end.dy,
                                iprm->end.hr,iprm->end.mt,iprm->end.sc);

  grd->stnum=iprm->stnum;
  grd->vcnum=iprm->vcnum;
  grd->xtd=iprm->xtd;



}

void IDLCopyGridStVecFromIDL(struct GridIDLStVec *istvec,
                             int nvec,int size,struct GridData *grd) {

  struct GridIDLStVec *sptr;
  void *tmp;
  int n;

  if (grd->sdata==NULL) tmp=malloc(sizeof(struct GridSVec)*nvec);
  else tmp=realloc(grd->sdata,sizeof(struct GridSVec)*nvec);
 
  grd->sdata=tmp;


  for (n=0;n<nvec;n++) {
  
    sptr=(struct GridIDLStVec *) (((char *) istvec)+size*n);

    grd->sdata[n].st_id=sptr->stid;
    grd->sdata[n].chn=sptr->chn;
    grd->sdata[n].npnt=sptr->npnt;
    grd->sdata[n].freq0=sptr->freq;
    grd->sdata[n].major_revision=sptr->major_revision;
    grd->sdata[n].minor_revision=sptr->minor_revision;
    grd->sdata[n].prog_id=sptr->prog_id;
    grd->sdata[n].gsct=sptr->gsct;
    grd->sdata[n].noise.mean=sptr->noise.mean;
    grd->sdata[n].noise.sd=sptr->noise.sd;
    grd->sdata[n].vel.min=sptr->vel.min;
    grd->sdata[n].vel.max=sptr->vel.max;
    grd->sdata[n].pwr.min=sptr->pwr.min;
    grd->sdata[n].pwr.max=sptr->pwr.max;
    grd->sdata[n].wdt.min=sptr->wdt.min;
    grd->sdata[n].wdt.max=sptr->wdt.max;
    grd->sdata[n].verr.min=sptr->verr.min;
    grd->sdata[n].verr.max=sptr->verr.max;
  }
}

void IDLCopyGridGVecFromIDL(struct GridIDLGVec *igvec,
                             int nvec,int size,struct GridData *grd) {


  struct GridIDLGVec *gptr;
  void *tmp;
  int n;

  if (grd->data==NULL) tmp=malloc(sizeof(struct GridGVec)*nvec);
  else tmp=realloc(grd->data,sizeof(struct GridGVec)*nvec);
 
  grd->data=tmp;


  for (n=0;n<nvec;n++) {

    gptr=(struct GridIDLGVec *) (((char *) igvec)+size*n);
   
    grd->data[n].st_id=gptr->stid;
    grd->data[n].chn=gptr->chn;
    grd->data[n].index=gptr->index;
    grd->data[n].mlat=gptr->mlat;
    grd->data[n].mlon=gptr->mlon;
    grd->data[n].azm=gptr->azm;
    grd->data[n].srng=gptr->srng;
    grd->data[n].vel.median=gptr->vel.median;
    grd->data[n].vel.sd=gptr->vel.sd;    
    grd->data[n].pwr.median=gptr->pwr.median;
    grd->data[n].pwr.sd=gptr->pwr.sd;
    grd->data[n].wdt.median=gptr->wdt.median;
    grd->data[n].wdt.sd=gptr->wdt.sd;
  }


}



void IDLCopyGridPrmToIDL(struct GridData *grd,
                         struct GridIDLPrm *iprm) {

  int yr,mo,dy,hr,mt;
  double sc;

  iprm->stnum=grd->stnum;
  iprm->vcnum=grd->vcnum;
  iprm->xtd=grd->xtd;
  TimeEpochToYMDHMS(grd->st_time,&yr,&mo,
                    &dy,&hr,&mt,
                    &sc);
  iprm->start.yr=yr;
  iprm->start.mo=mo;
  iprm->start.dy=dy;
  iprm->start.hr=hr;
  iprm->start.mt=mt;
  iprm->start.sc=sc;

  TimeEpochToYMDHMS(grd->ed_time,&yr,&mo,
                    &dy,&hr,&mt,
                    &sc);
  iprm->end.yr=yr;
  iprm->end.mo=mo;
  iprm->end.dy=dy;
  iprm->end.hr=hr;
  iprm->end.mt=mt;
  iprm->end.sc=sc;



}



void IDLCopyGridStVecToIDL(struct GridData *grd,int nvec,int size,
                         struct GridIDLStVec *istvec) {

  struct GridIDLStVec *sptr;
  int n;
 
  for (n=0;n<nvec;n++) {
  
    sptr=(struct GridIDLStVec *) (((char *) istvec)+size*n);
    sptr->stid=grd->sdata[n].st_id;
    sptr->chn=grd->sdata[n].chn;
    sptr->npnt=grd->sdata[n].npnt;
    sptr->freq=grd->sdata[n].freq0;
    sptr->major_revision=grd->sdata[n].major_revision;
    sptr->minor_revision=grd->sdata[n].minor_revision;
    sptr->prog_id=grd->sdata[n].prog_id;
    sptr->gsct=grd->sdata[n].gsct;
    sptr->noise.mean=grd->sdata[n].noise.mean;
    sptr->noise.sd=grd->sdata[n].noise.sd;
    sptr->vel.min=grd->sdata[n].vel.min;
    sptr->vel.max=grd->sdata[n].vel.max;
    sptr->pwr.min=grd->sdata[n].pwr.min;
    sptr->pwr.max=grd->sdata[n].pwr.max;
    sptr->wdt.min=grd->sdata[n].wdt.min;
    sptr->wdt.max=grd->sdata[n].wdt.max;
    sptr->verr.min=grd->sdata[n].verr.min;
    sptr->verr.max=grd->sdata[n].verr.max;
  }
}

void IDLCopyGridGVecToIDL(struct GridData *grd,int nvec,int size,
                         struct GridIDLGVec *igvec) {

  struct GridIDLGVec *gptr;
  int n;
 
  for (n=0;n<nvec;n++) {
     gptr=(struct GridIDLGVec *) (((char *) igvec)+size*n);
     gptr->stid=grd->data[n].st_id;
     gptr->chn=grd->data[n].chn;
     gptr->index=grd->data[n].index;
     gptr->mlat=grd->data[n].mlat;
     gptr->mlon=grd->data[n].mlon;
     gptr->azm=grd->data[n].azm;
     gptr->srng=grd->data[n].srng;
     gptr->vel.median=grd->data[n].vel.median;
     gptr->vel.sd=grd->data[n].vel.sd;
     gptr->pwr.median=grd->data[n].pwr.median;
     gptr->pwr.sd=grd->data[n].pwr.sd;
     gptr->wdt.median=grd->data[n].wdt.median;
     gptr->wdt.sd=grd->data[n].wdt.sd;
  }
}

struct GridIDLPrm *IDLMakeGridPrm(IDL_VPTR *vptr) {
  
  void *s=NULL;
   
  static IDL_STRUCT_TAG_DEF ttime[]={
    {"YR",0,(void *) IDL_TYP_INT},
    {"MO",0,(void *) IDL_TYP_INT},
    {"DY",0,(void *) IDL_TYP_INT},
    {"HR",0,(void *) IDL_TYP_INT},
    {"MT",0,(void *) IDL_TYP_INT},
    {"SC",0,(void *) IDL_TYP_DOUBLE},
    {0}};

  
  static IDL_STRUCT_TAG_DEF gridprm[]={    
    {"STME",0,NULL},   /* 0 */
    {"ETME",0,NULL},   /* 1 */ 
    {"STNUM",0,(void *) IDL_TYP_LONG}, /* 2 */
    {"VCNUM",0,(void *) IDL_TYP_LONG}, /* 3 */
    {"XTD",0,(void *) IDL_TYP_INT}, /* 4 */ 
    {0}};

  static IDL_MEMINT ilDims[IDL_MAX_ARRAY_DIM];
 
    
  gridprm[0].type=IDL_MakeStruct("GRIDTIME",ttime);
  gridprm[1].type=IDL_MakeStruct("GRIDTIME",ttime);

  s=IDL_MakeStruct("GRIDPRM",gridprm);
           
  ilDims[0]=1;
  
  return (struct GridIDLPrm *) IDL_MakeTempStruct(s,1,ilDims,vptr,TRUE);
  
}

struct GridIDLStVec *IDLMakeGridStVec(int nvec,IDL_VPTR *vptr) {

  void *s=NULL;

  static IDL_STRUCT_TAG_DEF noise[]={
    {"MEAN",0,(void *) IDL_TYP_FLOAT},
    {"SD",0,(void *) IDL_TYP_FLOAT},
    {0}};

  static IDL_STRUCT_TAG_DEF limit[]={
    {"MIN",0,(void *) IDL_TYP_FLOAT},
    {"MAX",0,(void *) IDL_TYP_FLOAT},
    {0}};

  static IDL_STRUCT_TAG_DEF gridstvec[]={    
    {"ST_ID",0, (void *) IDL_TYP_INT},   /* 0 */
    {"CHN",0, (void *) IDL_TYP_INT},   /* 1 */
    {"NPNT",0, (void *) IDL_TYP_INT},   /* 2 */
    {"FREQ",0, (void *) IDL_TYP_FLOAT},   /* 3 */
    {"MAJOR_REVISION",0, (void *) IDL_TYP_INT},   /* 4 */
    {"MINOR_REVISION",0, (void *) IDL_TYP_INT},   /* 5 */
    {"PROG_ID",0, (void *) IDL_TYP_INT},   /* 6 */
    {"GSCT",0, (void *) IDL_TYP_INT},   /* 7 */
    {"NOISE",0,NULL}, /* 8 */
    {"VEL",0,NULL}, /* 9 */
    {"PWR",0,NULL}, /* 10 */
    {"WDT",0,NULL}, /* 11 */
    {"VERR",0,NULL}, /* 12 */
    {0}};

  static IDL_MEMINT ilDims[IDL_MAX_ARRAY_DIM];
 
  gridstvec[8].type=IDL_MakeStruct("GRIDNOISE",noise);
  gridstvec[9].type=IDL_MakeStruct("GRIDLIMIT",limit);
  gridstvec[10].type=IDL_MakeStruct("GRIDLIMIT",limit);
  gridstvec[11].type=IDL_MakeStruct("GRIDLIMIT",limit);
  gridstvec[12].type=IDL_MakeStruct("GRIDLIMIT",limit);

  s=IDL_MakeStruct("GRIDSVEC",gridstvec);

  ilDims[0]=nvec;
  return (struct GridIDLStVec *) IDL_MakeTempStruct(s,1,ilDims,vptr,TRUE);

}



struct GridIDLGVec *IDLMakeGridGVec(int nvec,IDL_VPTR *vptr) {

  void *s=NULL;

  static IDL_STRUCT_TAG_DEF value[]={
    {"MEDIAN",0,(void *) IDL_TYP_FLOAT},
    {"SD",0,(void *) IDL_TYP_FLOAT},
    {0}};

  static IDL_STRUCT_TAG_DEF gridgvec[]={    
    {"MLAT",0,(void *) IDL_TYP_FLOAT},   /* 0 */
    {"MLON",0,(void *) IDL_TYP_FLOAT},   /* 1 */ 
    {"AZM",0,(void *) IDL_TYP_FLOAT}, /* 2 */
    {"SRNG",0,(void *) IDL_TYP_FLOAT}, /* 3 */
    {"VEL",0,NULL}, /* 4 */
    {"PWR",0,NULL}, /* 5 */
    {"WDT",0,NULL}, /* 6 */
    {"ST_ID",0,(void *) IDL_TYP_INT}, /* 7 */
    {"CHN",0,(void *) IDL_TYP_INT}, /* 8 */ 
    {"INDEX",0,(void *) IDL_TYP_LONG}, /* 9 */ 
    {0}};

  static IDL_MEMINT ilDims[IDL_MAX_ARRAY_DIM];

  gridgvec[4].type=IDL_MakeStruct("GRIDVALUE",value);
  gridgvec[5].type=IDL_MakeStruct("GRIDVALUE",value);
  gridgvec[6].type=IDL_MakeStruct("GRIDVALUE",value);
 
  s=IDL_MakeStruct("GRIDGVEC",gridgvec);

  ilDims[0]=nvec;
  return (struct GridIDLGVec *) IDL_MakeTempStruct(s,1,ilDims,vptr,TRUE);

}




struct GridIDLInx *IDLMakeGridInx(int num,IDL_VPTR *vptr) {
  
  void *s=NULL;
  
  static IDL_MEMINT idim[1]={0};
 
  static IDL_STRUCT_TAG_DEF grdinx[]={
    {"TIME",0,(void *) IDL_TYP_DOUBLE},
    {"OFFSET",0,(void *) IDL_TYP_LONG},
    {0}};

   s=IDL_MakeStruct("GRDINX",grdinx);  
   idim[0]=num;

   return (struct GridIDLInx *) IDL_MakeTempStruct(s,1,idim,vptr,TRUE);
}
