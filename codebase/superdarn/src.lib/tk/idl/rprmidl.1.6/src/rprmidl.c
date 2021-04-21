/* rprm.c
   ====== 
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
#include "dmap.h"
#include "rprm.h"
#include "rprmidl.h"

void IDLCopyRadarParmFromIDL(struct RadarIDLParm *iprm,struct RadarParm *prm) {

  int16 *lag=NULL;
  int n;

  prm->revision.major=iprm->revision.major;
  prm->revision.minor=iprm->revision.minor;
  prm->origin.code=iprm->origin.code;

  if (strlen(IDL_STRING_STR(&iprm->origin.time)) !=0)
    RadarParmSetOriginTime(prm,IDL_STRING_STR(&iprm->origin.time));

  if (strlen(IDL_STRING_STR(&iprm->origin.command)) !=0)
    RadarParmSetOriginCommand(prm,IDL_STRING_STR(&iprm->origin.command));

  prm->cp=iprm->cp;
  prm->stid=iprm->stid;
  prm->time.yr=iprm->time.yr;
  prm->time.mo=iprm->time.mo;
  prm->time.dy=iprm->time.dy;
  prm->time.hr=iprm->time.hr;
  prm->time.mt=iprm->time.mt;
  prm->time.sc=iprm->time.sc;
  prm->time.us=iprm->time.us;
  prm->txpow=iprm->txpow;
  prm->nave=iprm->nave;
  prm->atten=iprm->atten;
  prm->lagfr=iprm->lagfr;
  prm->smsep=iprm->smsep;
  prm->ercod=iprm->ercod;
  prm->stat.agc=iprm->stat.agc;
  prm->stat.lopwr=iprm->stat.lopwr;
  prm->noise.search=iprm->noise.search;
  prm->noise.mean=iprm->noise.mean;
  prm->channel=iprm->channel;
  prm->bmnum=iprm->bmnum;
  prm->bmazm=iprm->bmazm;
  prm->scan=iprm->scan;
  prm->rxrise=iprm->rxrise;
  prm->intt.sc=iprm->intt.sc;
  prm->intt.us=iprm->intt.us;
  prm->txpl=iprm->txpl;
  prm->mpinc=iprm->mpinc;
  prm->mppul=iprm->mppul;
  prm->mplgs=iprm->mplgs;
  prm->mplgexs=iprm->mplgexs;
  prm->nrang=iprm->nrang;
  prm->frang=iprm->frang;
  prm->rsep=iprm->rsep;
  prm->xcf=iprm->xcf;
  prm->tfreq=iprm->tfreq;
  prm->offset=iprm->offset;
  prm->ifmode=iprm->ifmode;
  prm->mxpwr=iprm->mxpwr;
  prm->lvmax=iprm->lvmax;

  RadarParmSetPulse(prm,iprm->mppul,iprm->pulse);

  lag=malloc(sizeof(int16)*2*prm->mplgs+1);
  for (n=0;n<=prm->mplgs;n++) {
    lag[2*n]=iprm->lag[n];
    lag[2*n+1]=iprm->lag[LAG_SIZE+n];
  }
  RadarParmSetLag(prm,iprm->mplgs,lag);
    free(lag);

  if (strlen(IDL_STRING_STR(&iprm->combf)) !=0)
    RadarParmSetCombf(prm,IDL_STRING_STR(&iprm->combf));

}

void IDLCopyRadarParmToIDL(struct RadarParm *prm,struct RadarIDLParm *iprm) {

  int n;

  char combftmp[COMBF_SIZE+1];
  char origintimetmp[ORIGIN_TIME_SIZE+1];
  char origincommandtmp[ORIGIN_COMMAND_SIZE+1];

  memset(&combftmp,0,COMBF_SIZE+1);
  memset(&origintimetmp,0,ORIGIN_TIME_SIZE+1);
  memset(&origincommandtmp,0,ORIGIN_COMMAND_SIZE+1);



  iprm->revision.major=prm->revision.major;
  iprm->revision.minor=prm->revision.minor;
  iprm->origin.code=prm->origin.code;

  if (prm->origin.time !=NULL) {  
    strncpy(origintimetmp,prm->origin.time,ORIGIN_TIME_SIZE);
    IDL_StrStore(&iprm->origin.time,origintimetmp);
  }

  if (prm->origin.command !=NULL) {
    strncpy(origincommandtmp,prm->origin.command,ORIGIN_COMMAND_SIZE);
    IDL_StrStore(&iprm->origin.command,origincommandtmp);
  }

  iprm->cp=prm->cp;
  iprm->stid=prm->stid;
  iprm->time.yr=prm->time.yr;
  iprm->time.mo=prm->time.mo;
  iprm->time.dy=prm->time.dy;
  iprm->time.hr=prm->time.hr;
  iprm->time.mt=prm->time.mt;
  iprm->time.sc=prm->time.sc;
  iprm->time.us=prm->time.us;
  iprm->txpow=prm->txpow;
  iprm->nave=prm->nave;
  iprm->atten=prm->atten;
  iprm->lagfr=prm->lagfr;
  iprm->smsep=prm->smsep;
  iprm->ercod=prm->ercod;
  iprm->stat.agc=prm->stat.agc;
  iprm->stat.lopwr=prm->stat.lopwr;
  iprm->noise.search=prm->noise.search;
  iprm->noise.mean=prm->noise.mean;
  iprm->channel=prm->channel;
  iprm->bmnum=prm->bmnum;
  iprm->bmazm=prm->bmazm;
  iprm->scan=prm->scan;
  iprm->rxrise=prm->rxrise;
  iprm->intt.sc=prm->intt.sc;
  iprm->intt.us=prm->intt.us;
  iprm->txpl=prm->txpl;
  iprm->mpinc=prm->mpinc;
  iprm->mppul=prm->mppul;
  iprm->mplgs=prm->mplgs;
  iprm->mplgexs=prm->mplgexs;
  iprm->nrang=prm->nrang;
  iprm->frang=prm->frang;
  iprm->rsep=prm->rsep;
  iprm->xcf=prm->xcf;
  iprm->tfreq=prm->tfreq;
  iprm->offset=prm->offset;
  iprm->ifmode=prm->ifmode;
  iprm->mxpwr=prm->mxpwr;
  iprm->lvmax=prm->lvmax;

  for (n=0;n<prm->mppul;n++) iprm->pulse[n]=prm->pulse[n];
  
  for (n=0;n<=prm->mplgs;n++) {
    iprm->lag[n]=prm->lag[0][n];
    iprm->lag[LAG_SIZE+n]=prm->lag[1][n];
  }
 
  if (prm->combf !=NULL) {
    strncpy(combftmp,prm->combf,COMBF_SIZE);
    IDL_StrStore(&iprm->combf,combftmp);
  }

}

struct RadarIDLParm *IDLMakeRadarParm(IDL_VPTR *vptr) {
  
  void *s=NULL;
  
  static IDL_MEMINT pdim[]={1,PULSE_SIZE};
  static IDL_MEMINT ldim[]={2,LAG_SIZE,2};

  static IDL_STRUCT_TAG_DEF revision[]={
    {"MAJOR",0,(void *) IDL_TYP_BYTE},
    {"MINOR",0,(void *) IDL_TYP_BYTE},
    {0}};

  static IDL_STRUCT_TAG_DEF origin[]={
    {"CODE",0,(void *) IDL_TYP_BYTE},
    {"TIME",0,(void *) IDL_TYP_STRING},
    {"COMMAND",0,(void *) IDL_TYP_STRING},
    {0}};

  static IDL_STRUCT_TAG_DEF time[]={
    {"YR",0,(void *) IDL_TYP_INT},
    {"MO",0,(void *) IDL_TYP_INT},
    {"DY",0,(void *) IDL_TYP_INT},
    {"HR",0,(void *) IDL_TYP_INT},
    {"MT",0,(void *) IDL_TYP_INT},
    {"SC",0,(void *) IDL_TYP_INT},
    {"US",0,(void *) IDL_TYP_LONG},
    {0}};  

 
  static IDL_STRUCT_TAG_DEF stat[]={
    {"AGC",0,(void *) IDL_TYP_INT},
    {"LOPWR",0,(void *) IDL_TYP_INT},{0}};
 
  static IDL_STRUCT_TAG_DEF noise[]={
      {"SEARCH",0,(void *) IDL_TYP_FLOAT},
      {"MEAN",0,(void *) IDL_TYP_FLOAT},
      {0}};

  static IDL_STRUCT_TAG_DEF intt[]={
    {"SC",0,(void *) IDL_TYP_INT},
    {"US",0,(void *) IDL_TYP_LONG},
    {0}};
  
  static IDL_STRUCT_TAG_DEF rprm[]={    
    {"REVISION",0,NULL},   /* 0 */
    {"ORIGIN",0,NULL},   /* 1 */ 
    {"CP",0,(void *) IDL_TYP_INT}, /* 2 */
    {"STID",0,(void *) IDL_TYP_INT}, /* 3 */
    {"TIME",0,NULL}, /* 4 */
    {"TXPOW",0,(void *) IDL_TYP_INT}, /* 5 */
    {"NAVE",0,(void *) IDL_TYP_INT}, /* 6 */
    {"ATTEN",0,(void *) IDL_TYP_INT}, /* 7 */
    {"LAGFR",0,(void *) IDL_TYP_INT}, /* 8 */
    {"SMSEP",0,(void *) IDL_TYP_INT}, /* 9 */
    {"ERCOD",0,(void *) IDL_TYP_INT}, /* 10 */
    {"STAT",0,NULL}, /* 11 */
    {"NOISE",0,NULL}, /* 12 */
    {"CHANNEL",0,(void *) IDL_TYP_INT}, /* 13 */
    {"BMNUM",0,(void *) IDL_TYP_INT}, /* 14 */
    {"BMAZM",0,(void *) IDL_TYP_FLOAT}, /* 15 */
    {"SCAN",0,(void *) IDL_TYP_INT}, /* 16 */
    {"RXRISE",0,(void *) IDL_TYP_INT}, /* 17 */
    {"INTT",0,NULL}, /* 18 */
    {"TXPL",0,(void *) IDL_TYP_INT}, /* 19 */
    {"MPINC",0,(void *) IDL_TYP_INT}, /* 20 */
    {"MPPUL",0,(void *) IDL_TYP_INT}, /* 21 */
    {"MPLGS",0,(void *) IDL_TYP_INT}, /* 22 */
    {"MPLGEXS",0,(void *) IDL_TYP_INT}, /* 23 */
    {"NRANG",0,(void *) IDL_TYP_INT}, /* 24 */
    {"FRANG",0,(void *) IDL_TYP_INT}, /* 25 */
    {"RSEP",0,(void *) IDL_TYP_INT}, /* 26 */
    {"XCF",0,(void *) IDL_TYP_INT}, /* 27 */
    {"TFREQ",0,(void *) IDL_TYP_INT}, /* 28 */

    {"OFFSET",0,(void *) IDL_TYP_INT}, /* 29 */
    {"IFMODE",0,(void *) IDL_TYP_INT}, /* 30 */
    {"MXPWR",0,(void *) IDL_TYP_LONG}, /* 31 */
    {"LVMAX",0,(void *) IDL_TYP_LONG}, /* 32 */
    {"PULSE",pdim,(void *) IDL_TYP_INT}, /* 33 */
    {"LAG",ldim,(void *) IDL_TYP_INT}, /* 34 */
    {"COMBF",0,(void *) IDL_TYP_STRING}, /* 35 */   
    {0}};

  static IDL_MEMINT ilDims[IDL_MAX_ARRAY_DIM];
 
    
  rprm[0].type=IDL_MakeStruct("RBSTR",revision);
  rprm[1].type=IDL_MakeStruct("OGSTR",origin);
  rprm[4].type=IDL_MakeStruct("TMSTR",time);
  rprm[11].type=IDL_MakeStruct("STSTR",stat);
  rprm[12].type=IDL_MakeStruct("NSSTR",noise);
  rprm[18].type=IDL_MakeStruct("ITSTR",intt);
  

  s=IDL_MakeStruct("RADARPRM",rprm);
           
  ilDims[0]=1;
  
  return (struct RadarIDLParm *) IDL_MakeTempStruct(s,1,ilDims,vptr,TRUE);
  
}
