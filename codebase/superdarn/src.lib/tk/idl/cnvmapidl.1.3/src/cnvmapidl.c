/* cnvmapidl.c
   =========== 
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
 
 Functions

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
#include "cnvmap.h"
#include "grdidl.h"
#include "cnvmapidl.h"


void IDLCopyCnvMapPrmFromIDL(struct CnvMapIDLPrm *iprm,
			     struct CnvMapData *map,struct GridData *grd) {

  int n;
  grd->st_time=TimeYMDHMSToEpoch(iprm->start.yr,iprm->start.mo,iprm->start.dy,
                                iprm->start.hr,iprm->start.mt,iprm->start.sc);
  grd->ed_time=TimeYMDHMSToEpoch(iprm->end.yr,iprm->end.mo,iprm->end.dy,
                                iprm->end.hr,iprm->end.mt,iprm->end.sc);

  grd->stnum=iprm->stnum;
  grd->vcnum=iprm->vcnum;
  grd->xtd=iprm->xtd;

  map->st_time=grd->st_time;
  map->ed_time=grd->ed_time;

  map->major_rev=iprm->major_rev;
  map->minor_rev=iprm->minor_rev;

  if (IDL_STRING_STR(&iprm->source) !=NULL) {
    n=strlen(IDL_STRING_STR(&iprm->source));
    if (n>256) n=256;
    strncpy(map->source,IDL_STRING_STR(&iprm->source),n);
  }

  map->num_model=iprm->modnum;
  map->doping_level=iprm->doping_level;
  map->model_wt=iprm->model_wt;
  map->error_wt=iprm->error_wt;
  map->imf_flag=iprm->imf_flag;
  map->imf_delay=iprm->imf_delay;
  map->Bx=iprm->Bx;
  map->By=iprm->By;
  map->Bz=iprm->Bz;


  if (IDL_STRING_STR(&iprm->model[0]) !=NULL) {
    n=strlen(IDL_STRING_STR(&iprm->model[0]));
    if (n>64) n=64;
    strncpy(map->imf_model[0],IDL_STRING_STR(&iprm->model[0]),n);
  }


  if (IDL_STRING_STR(&iprm->model[1]) !=NULL) {
    n=strlen(IDL_STRING_STR(&iprm->model[1]));
    if (n>64) n=64;
    strncpy(map->imf_model[1],IDL_STRING_STR(&iprm->model[1]),n);
  }

  if (IDL_STRING_STR(&iprm->model[2]) !=NULL) {
    n=strlen(IDL_STRING_STR(&iprm->model[2]));
    if (n>64) n=64;
    strncpy(map->imf_model[2],IDL_STRING_STR(&iprm->model[2]),n);
  }

  if (IDL_STRING_STR(&iprm->model[3]) !=NULL) {
    n=strlen(IDL_STRING_STR(&iprm->model[3]));
    if (n>64) n=64;
    strncpy(map->imf_model[3],IDL_STRING_STR(&iprm->model[3]),n);
  }

  

  map->hemisphere=iprm->hemisphere;
  map->fit_order=iprm->fit_order;
  map->latmin=iprm->latmin;
  map->num_coef=iprm->coefnum;
  map->chi_sqr=iprm->chi_sqr;
  map->chi_sqr_dat=iprm->chi_sqr_dat;
  map->rms_err=iprm->rms_err;
  map->lon_shft=iprm->lon_shft;
  map->lat_shft=iprm->lat_shft;
  map->mlt.start=iprm->mlt.st;
  map->mlt.end=iprm->mlt.ed;
  map->mlt.av=iprm->mlt.av;
  map->pot_drop=iprm->pot_drop;
  map->pot_drop_err=iprm->pot_drop_err;
  map->pot_max=iprm->pot_max;
  map->pot_max_err=iprm->pot_max_err;
  map->pot_min=iprm->pot_min;
  map->pot_min_err=iprm->pot_min_err;
  map->num_bnd=iprm->bndnum;


}


void IDLCopyCnvMapGVecFromIDL(struct GridIDLGVec *igvec,
                             int nvec,int size,struct CnvMapData *map) {


  struct GridIDLGVec *gptr;
  void *tmp;
  int n;

  if (map->model==NULL) tmp=malloc(sizeof(struct GridGVec)*nvec);
  else tmp=realloc(map->model,sizeof(struct GridGVec)*nvec);
 
  map->model=tmp;


  for (n=0;n<nvec;n++) {

    gptr=(struct GridIDLGVec *) (((char *) igvec)+size*n);
   
    map->model[n].st_id=gptr->stid;
    map->model[n].chn=gptr->chn;
    map->model[n].index=gptr->index;
    map->model[n].mlat=gptr->mlat;
    map->model[n].mlon=gptr->mlon;
    map->model[n].azm=gptr->azm;
    map->model[n].srng=gptr->srng;
    map->model[n].vel.median=gptr->vel.median;
    map->model[n].vel.sd=gptr->vel.sd;    
    map->model[n].pwr.median=gptr->pwr.median;
    map->model[n].pwr.sd=gptr->pwr.sd;
    map->model[n].wdt.median=gptr->wdt.median;
    map->model[n].wdt.sd=gptr->wdt.sd;
  }


}


void IDLCopyCnvMapBndFromIDL(struct CnvMapIDLBnd *ibnd,
                             int nvec,int size,struct CnvMapData *map) {


  struct CnvMapIDLBnd *bptr;
  void *tmp;
  int n;

  if (map->bnd_lat==NULL) tmp=malloc(sizeof(double)*nvec);
  else tmp=realloc(map->bnd_lat,sizeof(double)*nvec);
 
  map->bnd_lat=tmp;

 if (map->bnd_lon==NULL) tmp=malloc(sizeof(double)*nvec);
  else tmp=realloc(map->bnd_lon,sizeof(double)*nvec);
 
  map->bnd_lon=tmp;



  for (n=0;n<nvec;n++) {

    bptr=(struct CnvMapIDLBnd *) (((char *) ibnd)+size*n);
   
    map->bnd_lat[n]=bptr->lat;
    map->bnd_lon[n]=bptr->lon;
  }


}




void IDLCopyCnvMapPrmToIDL(struct CnvMapData *map,struct GridData *grd,
                         struct CnvMapIDLPrm *iprm) {

  int yr,mo,dy,hr,mt;
  double sc;

  char tmp[257];
  
  memset(tmp,0,257);

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



  iprm->major_rev=map->major_rev;
  iprm->minor_rev=map->minor_rev;

  if (map->source !=NULL) {
    strncpy(tmp,map->source,256);
    IDL_StrStore(&iprm->source,tmp);
  }

  iprm->modnum=map->num_model;
  iprm->doping_level=map->doping_level;
  iprm->model_wt=map->model_wt;
  iprm->error_wt=map->error_wt;
  iprm->imf_flag=map->imf_flag;
  iprm->imf_delay=map->imf_delay;
  iprm->Bx=map->Bx;
  iprm->By=map->By;
  iprm->Bz=map->Bz;

  iprm->Vx=map->Vx;
  iprm->tilt=map->tilt;
  iprm->Kp=map->Kp;

  if (map->imf_model[0] !=NULL) {
     strncpy(tmp,map->imf_model[0],64);
     IDL_StrStore(&iprm->model[0],tmp);
  }

  if (map->imf_model[1] !=NULL) {
    strncpy(tmp,map->imf_model[1],64);
    IDL_StrStore(&iprm->model[1],tmp);
  }

  if (map->imf_model[2] !=NULL) {
    strncpy(tmp,map->imf_model[2],64);
    IDL_StrStore(&iprm->model[2],tmp);
  }

  if (map->imf_model[3] !=NULL) {
    strncpy(tmp,map->imf_model[3],64);
    IDL_StrStore(&iprm->model[3],tmp);
  }

  iprm->hemisphere=map->hemisphere;
  iprm->noigrf=map->noigrf;
  iprm->fit_order=map->fit_order;
  iprm->latmin=map->latmin;
  iprm->coefnum=map->num_coef;
  iprm->chi_sqr=map->chi_sqr;
  iprm->chi_sqr_dat=map->chi_sqr_dat;
  iprm->rms_err=map->rms_err;
  iprm->lon_shft=map->lon_shft;
  iprm->lat_shft=map->lat_shft;
  iprm->mlt.st=map->mlt.start;
  iprm->mlt.ed=map->mlt.end;
  iprm->mlt.av=map->mlt.av;
  iprm->pot_drop=map->pot_drop;
  iprm->pot_drop_err=map->pot_drop_err;
  iprm->pot_max=map->pot_max;
  iprm->pot_max_err=map->pot_max_err;
  iprm->pot_min=map->pot_min;
  iprm->pot_min_err=map->pot_min_err;
  iprm->bndnum=map->num_bnd;
}




void IDLCopyCnvMapGVecToIDL(struct CnvMapData *map,int nvec,int size,
                         struct GridIDLGVec *igvec) {

  struct GridIDLGVec *gptr;
  int n;
 
  for (n=0;n<nvec;n++) {
     gptr=(struct GridIDLGVec *) (((char *) igvec)+size*n);
     gptr->stid=map->model[n].st_id;
     gptr->chn=map->model[n].chn;
     gptr->index=map->model[n].index;
     gptr->mlat=map->model[n].mlat;
     gptr->mlon=map->model[n].mlon;
     gptr->azm=map->model[n].azm;
     gptr->srng=map->model[n].srng;
     gptr->vel.median=map->model[n].vel.median;
     gptr->vel.sd=map->model[n].vel.sd;
     gptr->pwr.median=map->model[n].pwr.median;
     gptr->pwr.sd=map->model[n].pwr.sd;
     gptr->wdt.median=map->model[n].wdt.median;
     gptr->wdt.sd=map->model[n].wdt.sd;
  }
}


void IDLCopyCnvMapBndToIDL(struct CnvMapData *map,int nvec,int size,
                         struct CnvMapIDLBnd *ibnd) {

  struct CnvMapIDLBnd *bptr;
  int n;
 
  for (n=0;n<nvec;n++) {
     bptr=(struct CnvMapIDLBnd *) (((char *) ibnd)+size*n);
     bptr->lat=map->bnd_lat[n];
     bptr->lon=map->bnd_lon[n];
  }
}



struct CnvMapIDLPrm *IDLMakeCnvMapPrm(IDL_VPTR *vptr) {
  
  void *s=NULL;


  static IDL_MEMINT mdim[]={1,4};

  static IDL_STRUCT_TAG_DEF ttime[]={
    {"YR",0,(void *) IDL_TYP_INT},
    {"MO",0,(void *) IDL_TYP_INT},
    {"DY",0,(void *) IDL_TYP_INT},
    {"HR",0,(void *) IDL_TYP_INT},
    {"MT",0,(void *) IDL_TYP_INT},
    {"SC",0,(void *) IDL_TYP_DOUBLE},
    {0}};

  
  static IDL_STRUCT_TAG_DEF tmlt[]={
    {"ST",0,(void *) IDL_TYP_DOUBLE},
    {"ED",0,(void *) IDL_TYP_DOUBLE},
    {"AV",0,(void *) IDL_TYP_DOUBLE},
    {0}};

  static IDL_STRUCT_TAG_DEF cnvmapprm[]={    
    {"STME",0,NULL},   /* 0 */
    {"ETME",0,NULL},   /* 1 */
    {"STNUM",0,(void *) IDL_TYP_LONG}, /* 2 */
    {"VCNUM",0,(void *) IDL_TYP_LONG}, /* 3 */
    {"XTD",0,(void *) IDL_TYP_INT}, /* 4 */
    {"MAJOR_REV",0,(void *) IDL_TYP_INT}, /* 5 */
    {"MINOR_REV",0,(void *) IDL_TYP_INT}, /* 6 */
    {"SOURCE",0,(void *) IDL_TYP_STRING}, /* 7 */ 
    {"MODNUM",0,(void *) IDL_TYP_LONG}, /* 8 */
    {"DOPING_LEVEL",0,(void *) IDL_TYP_INT}, /* 9 */
    {"MODEL_WT",0,(void *) IDL_TYP_INT}, /* 10 */ 
    {"ERROR_WT",0,(void *) IDL_TYP_INT}, /* 11 */ 
    {"IMF_FLAG",0,(void *) IDL_TYP_INT}, /* 12 */
    {"IMF_DELAY",0,(void *) IDL_TYP_INT}, /* 13 */
    {"BX",0,(void *) IDL_TYP_DOUBLE}, /* 14 */
    {"BY",0,(void *) IDL_TYP_DOUBLE}, /* 15 */
    {"BZ",0,(void *) IDL_TYP_DOUBLE}, /* 16 */
    {"VX",0,(void *) IDL_TYP_DOUBLE}, /* 17 */
    {"TILT",0,(void *) IDL_TYP_DOUBLE}, /* 18 */
    {"KP",0,(void *) IDL_TYP_DOUBLE}, /* 19 */
    {"IMF_MODEL",mdim,(void *) IDL_TYP_STRING}, /* 20 */
    {"HEMISPHERE",0,(void *) IDL_TYP_INT}, /* 21 */
    {"NOIGRF",0,(void *) IDL_TYP_INT}, /* 22 */
    {"FIT_ORDER",0,(void *) IDL_TYP_INT}, /* 23 */
    {"LATMIN",0,(void *) IDL_TYP_FLOAT}, /* 24 */
    {"COEFNUM",0,(void *) IDL_TYP_LONG}, /* 25 */
    {"CHI_SQR",0,(void *) IDL_TYP_DOUBLE}, /* 26 */
    {"CHI_SQR_DAT",0,(void *) IDL_TYP_DOUBLE}, /* 27 */
    {"RMS_ERR",0,(void *) IDL_TYP_DOUBLE}, /* 28 */
    {"LON_SHFT",0,(void *) IDL_TYP_FLOAT}, /* 29 */
    {"LAT_SHFT",0,(void *) IDL_TYP_FLOAT}, /* 30 */
    {"MLT",0, NULL}, /* 31 */
    {"POT_DROP",0,(void *) IDL_TYP_DOUBLE}, /* 32 */
    {"POT_DROP_ERR",0,(void *) IDL_TYP_DOUBLE}, /* 33 */
    {"POT_MAX",0,(void *) IDL_TYP_DOUBLE}, /* 34 */
    {"POT_MAX_ERR",0,(void *) IDL_TYP_DOUBLE}, /* 35 */
    {"POT_MIN",0,(void *) IDL_TYP_DOUBLE}, /* 36 */
    {"POT_MIN_ERR",0,(void *) IDL_TYP_DOUBLE}, /* 37 */
    {"BNDNUM",0,(void *) IDL_TYP_LONG}, /* 38 */
    
    {0}};

  static IDL_MEMINT ilDims[IDL_MAX_ARRAY_DIM];
 
  cnvmapprm[0].type=IDL_MakeStruct("CNVMAPTIME",ttime);
  cnvmapprm[1].type=IDL_MakeStruct("CNVMAPTIME",ttime);
  cnvmapprm[31].type=IDL_MakeStruct("CNVMAPMLT",tmlt);

  s=IDL_MakeStruct("CNVMAPPRM",cnvmapprm);
           
  ilDims[0]=1;
  
  return (struct CnvMapIDLPrm *) IDL_MakeTempStruct(s,1,ilDims,vptr,TRUE);
  
}


struct CnvMapIDLBnd *IDLMakeCnvMapBnd(int num,IDL_VPTR *vptr) {
  
  void *s=NULL;
  
  static IDL_MEMINT idim[1]={0};
 
  static IDL_STRUCT_TAG_DEF cnvmapbnd[]={
    {"LAT",0,(void *) IDL_TYP_FLOAT},
    {"LON",0,(void *) IDL_TYP_FLOAT},
    {0}};

   s=IDL_MakeStruct("CNVMAPBND",cnvmapbnd);  
   idim[0]=num;

   return (struct CnvMapIDLBnd *) IDL_MakeTempStruct(s,1,idim,vptr,TRUE);
}
  



struct CnvMapIDLInx *IDLMakeCnvMapInx(int num,IDL_VPTR *vptr) {
  
  void *s=NULL;
  
  static IDL_MEMINT idim[1]={0};
 
  static IDL_STRUCT_TAG_DEF grdinx[]={
    {"TIME",0,(void *) IDL_TYP_DOUBLE},
    {"OFFSET",0,(void *) IDL_TYP_LONG},
    {0}};

   s=IDL_MakeStruct("CNVMAPINX",grdinx);  
   idim[0]=num;

   return (struct CnvMapIDLInx *) IDL_MakeTempStruct(s,1,idim,vptr,TRUE);
}
  

