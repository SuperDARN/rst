/* sndidl.c
   ======== 
   Author E.G.Thomas

 
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
#include "snddata.h"
#include "sndidl.h"


void IDLCopySndDataFromIDL(int nrang, struct SndIDLData *isnd,
                           struct SndData *snd) {

  int n;

  SndSetRng(snd,nrang);

  snd->radar_revision.major = isnd->radar_revision.major;
  snd->radar_revision.minor = isnd->radar_revision.minor;
  snd->origin.code = isnd->origin.code;

  if (strlen(IDL_STRING_STR(&isnd->origin.time)) !=0)
    SndSetOriginTime(snd,IDL_STRING_STR(&isnd->origin.time));

  if (strlen(IDL_STRING_STR(&isnd->origin.command)) !=0)
    SndSetOriginCommand(snd,IDL_STRING_STR(&isnd->origin.command));

  snd->cp = isnd->cp;
  snd->stid = isnd->stid;
  snd->time.yr = isnd->time.yr;
  snd->time.mo = isnd->time.mo;
  snd->time.dy = isnd->time.dy;
  snd->time.hr = isnd->time.hr;
  snd->time.mt = isnd->time.mt;
  snd->time.sc = isnd->time.sc;
  snd->time.us = isnd->time.us;
  snd->nave = isnd->nave;
  snd->lagfr = isnd->lagfr;
  snd->smsep = isnd->smsep;
  snd->noise.search = isnd->noise.search;
  snd->noise.mean = isnd->noise.mean;
  snd->channel = isnd->channel;
  snd->bmnum = isnd->bmnum;
  snd->bmazm = isnd->bmazm;
  snd->scan = isnd->scan;
  snd->rxrise = isnd->rxrise;
  snd->intt.sc = isnd->intt.sc;
  snd->intt.us = isnd->intt.us;
  snd->nrang = isnd->nrang;
  snd->frang = isnd->frang;
  snd->rsep = isnd->rsep;
  snd->xcf = isnd->xcf;
  snd->tfreq = isnd->tfreq;
  snd->sky_noise = isnd->sky_noise;

  if (strlen(IDL_STRING_STR(&isnd->combf)) !=0)
    SndSetCombf(snd,IDL_STRING_STR(&isnd->combf));

  snd->fit_revision.major = isnd->fit_revision.major;
  snd->fit_revision.minor = isnd->fit_revision.minor;

  snd->snd_revision.major = isnd->snd_revision.major;
  snd->snd_revision.minor = isnd->snd_revision.minor;

  for (n=0;n<nrang;n++) {
    snd->rng[n].qflg = isnd->qflg[n];
    snd->rng[n].gsct = isnd->gflg[n];
    snd->rng[n].v = isnd->v[n];
    snd->rng[n].p_l = isnd->p_l[n];
    snd->rng[n].v_err = isnd->v_e[n];
    snd->rng[n].w_l = isnd->w_l[n];
    snd->rng[n].x_qflg = isnd->x_qflg[n];
    snd->rng[n].phi0 = isnd->phi0[n];
    snd->rng[n].phi0_err = isnd->phi0_e[n];
  }
}


void IDLCopySndDataToIDL(int nrang, struct SndData *snd,
                         struct SndIDLData *isnd) {

  int n;

  char combftmp[COMBF_SIZE+1];
  char origintimetmp[ORIGIN_TIME_SIZE+1];
  char origincommandtmp[ORIGIN_COMMAND_SIZE+1];

  memset(&combftmp,0,COMBF_SIZE+1);
  memset(&origintimetmp,0,ORIGIN_TIME_SIZE+1);
  memset(&origincommandtmp,0,ORIGIN_COMMAND_SIZE+1);

  memset(isnd,0,sizeof(struct SndIDLData));

  isnd->radar_revision.major = snd->radar_revision.major;
  isnd->radar_revision.minor = snd->radar_revision.minor;
  isnd->origin.code = snd->origin.code;

  if (snd->origin.time !=NULL) {
    strncpy(origintimetmp,snd->origin.time,ORIGIN_TIME_SIZE);
    IDL_StrStore(&isnd->origin.time,origintimetmp);
  }

  if (snd->origin.command !=NULL) {
    strncpy(origincommandtmp,snd->origin.command,ORIGIN_COMMAND_SIZE);
    IDL_StrStore(&isnd->origin.command,origincommandtmp);
  }

  isnd->cp = snd->cp;
  isnd->stid = snd->stid;
  isnd->time.yr = snd->time.yr;
  isnd->time.mo = snd->time.mo;
  isnd->time.dy = snd->time.dy;
  isnd->time.hr = snd->time.hr;
  isnd->time.mt = snd->time.mt;
  isnd->time.sc = snd->time.sc;
  isnd->time.us = snd->time.us;
  isnd->nave = snd->nave;
  isnd->lagfr = snd->lagfr;
  isnd->smsep = snd->smsep;
  isnd->noise.search = snd->noise.search;
  isnd->noise.mean = snd->noise.mean;
  isnd->channel = snd->channel;
  isnd->bmnum = snd->bmnum;
  isnd->bmazm = snd->bmazm;
  isnd->scan = snd->scan;
  isnd->rxrise = snd->rxrise;
  isnd->intt.sc = snd->intt.sc;
  isnd->intt.us = snd->intt.us;
  isnd->nrang = snd->nrang;
  isnd->frang = snd->frang;
  isnd->rsep = snd->rsep;
  isnd->xcf = snd->xcf;
  isnd->tfreq = snd->tfreq;
  isnd->sky_noise = snd->sky_noise;

  if (snd->combf !=NULL) {
    strncpy(combftmp,snd->combf,COMBF_SIZE);
    IDL_StrStore(&isnd->combf,combftmp);
  }

  isnd->fit_revision.major = snd->fit_revision.major;
  isnd->fit_revision.minor = snd->fit_revision.minor;

  isnd->snd_revision.major = snd->snd_revision.major;
  isnd->snd_revision.minor = snd->snd_revision.minor;

  for (n=0;n<nrang;n++) {
    isnd->qflg[n] = snd->rng[n].qflg;
    isnd->gflg[n] = snd->rng[n].gsct;
    isnd->v[n] = snd->rng[n].v;
    isnd->v_e[n] = snd->rng[n].v_err;
    isnd->p_l[n] = snd->rng[n].p_l;
    isnd->w_l[n] = snd->rng[n].w_l;
    isnd->x_qflg[n] = snd->rng[n].x_qflg;
    isnd->phi0[n] = snd->rng[n].phi0;
    isnd->phi0_e[n] = snd->rng[n].phi0_err;
  }
}


struct SndIDLData *IDLMakeSndData(IDL_VPTR *vptr) {

  void *s=NULL;

  static IDL_MEMINT rdim[]={1,MAX_RANGE};

  static IDL_STRUCT_TAG_DEF radar_revision[]={
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

  static IDL_STRUCT_TAG_DEF noise[]={
    {"SEARCH",0,(void *) IDL_TYP_FLOAT},
    {"MEAN",0,(void *) IDL_TYP_FLOAT},
    {0}};

  static IDL_STRUCT_TAG_DEF intt[]={
    {"SC",0,(void *) IDL_TYP_INT},
    {"US",0,(void *) IDL_TYP_LONG},
    {0}};

  static IDL_STRUCT_TAG_DEF fit_revision[]={
    {"MAJOR",0,(void *) IDL_TYP_LONG},
    {"MINOR",0,(void *) IDL_TYP_LONG},
    {0}};

  static IDL_STRUCT_TAG_DEF snd_revision[]={
    {"MAJOR",0,(void *) IDL_TYP_INT},
    {"MINOR",0,(void *) IDL_TYP_INT},
    {0}};

  static IDL_STRUCT_TAG_DEF snddata[]={
    {"RADAR_REVISION",0,NULL},           /* 0 */
    {"ORIGIN",0,NULL},                   /* 1 */
    {"CP",0,(void *) IDL_TYP_INT},       /* 2 */
    {"STID",0,(void *) IDL_TYP_INT},     /* 3 */
    {"TIME",0,NULL},                     /* 4 */
    {"NAVE",0,(void *) IDL_TYP_INT},     /* 5 */
    {"LAGFR",0,(void *) IDL_TYP_INT},    /* 6 */
    {"SMSEP",0,(void *) IDL_TYP_INT},    /* 7 */
    {"NOISE",0,NULL},                    /* 8 */
    {"CHANNEL",0,(void *) IDL_TYP_INT},  /* 9 */
    {"BMNUM",0,(void *) IDL_TYP_INT},   /* 10 */
    {"BMAZM",0,(void *) IDL_TYP_FLOAT}, /* 11 */
    {"SCAN",0,(void *) IDL_TYP_INT},    /* 12 */
    {"RXRISE",0,(void *) IDL_TYP_INT},  /* 13 */
    {"INTT",0,NULL},                    /* 14 */
    {"NRANG",0,(void *) IDL_TYP_INT},   /* 15 */
    {"FRANG",0,(void *) IDL_TYP_INT},   /* 16 */
    {"RSEP",0,(void *) IDL_TYP_INT},    /* 17 */
    {"XCF",0,(void *) IDL_TYP_INT},     /* 18 */
    {"TFREQ",0,(void *) IDL_TYP_INT},   /* 19 */
    {"SKY_NOISE",0,(void *) IDL_TYP_FLOAT}, /* 20 */
    {"COMBF",0,(void *) IDL_TYP_STRING}, /* 21 */
    {"FIT_REVISION",0,NULL},             /* 22 */
    {"SND_REVISION",0,NULL},             /* 23 */
    {"QFLG",rdim,(void *) IDL_TYP_BYTE}, /* 24 */
    {"GFLG",rdim,(void *) IDL_TYP_BYTE}, /* 25 */
    {"V",rdim,(void *) IDL_TYP_FLOAT},   /* 26 */
    {"V_E",rdim,(void *) IDL_TYP_FLOAT}, /* 27 */
    {"P_L",rdim,(void *) IDL_TYP_FLOAT}, /* 28 */
    {"W_L",rdim,(void *) IDL_TYP_FLOAT}, /* 29 */
    {"X_QFLG",rdim,(void *) IDL_TYP_BYTE}, /* 30 */
    {"PHI0",rdim,(void *) IDL_TYP_FLOAT}, /* 31 */
    {"PHI0_E",rdim,(void *) IDL_TYP_FLOAT}, /* 32 */
    {0}};

  static IDL_MEMINT ilDims[IDL_MAX_ARRAY_DIM];

  snddata[0].type=IDL_MakeStruct("RDSTR",radar_revision);
  snddata[1].type=IDL_MakeStruct("OGSTR",origin);
  snddata[4].type=IDL_MakeStruct("TMSTR",time);
  snddata[8].type=IDL_MakeStruct("NSSTR",noise);
  snddata[14].type=IDL_MakeStruct("ITSTR",intt);
  snddata[22].type=IDL_MakeStruct("RLSTR",fit_revision);
  snddata[23].type=IDL_MakeStruct("SDSTR",snd_revision);

  s=IDL_MakeStruct("SNDDATA",snddata);

  ilDims[0]=1;

  return (struct SndIDLData *) IDL_MakeTempStruct(s,1,ilDims,vptr,TRUE);
  
}
