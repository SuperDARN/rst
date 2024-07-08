/* rposdlm.c
   ========= 
   Author R.J.Barnes

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
  E.G.Thomas 2021-08: added support for new hdw file fields
  E.G.Thomas 2022-04: added support for tdiff calibration files
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
#include "radar.h"
#include "rpos.h"

#define RPOS_ERROR 0
#define RPOS_BADSTRUCT -1
#define RPOS_BADRNGARR -2
#define RPOS_BADFRANGARR -3
#define RPOS_BADRSEPARR -4
#define RPOS_BADRXRISEARR -5
#define RPOS_BADHEIGHTARR -6

static IDL_MSG_DEF msg_arr[] =
  {
    {"RPOS_ERROR", "%NError: %s."},
    {"RPOS_BADSTRUCT", "%NFailed to decode data structure %s"},
    {"RPOS_BADRNGARR", "%NBeam and range arrays must be of equal length %s"},
    {"RPOS_BADFRANGARR", "%NBeam and frang arrays must be of equal length %s"},
    {"RPOS_BADRSEPARR", "%NBeam and rsep arrays must be of equal length %s"},
    {"RPOS_BADRXRISEARR", "%NBeam and rxrise arrays must be of equal length %s"},
    {"RPOS_BADHEIGHTARR", "%NBeam and height arrays must be of equal length %s"},
  };

static IDL_MSG_BLOCK msg_block;

union RadarIDLPtr {
  unsigned char *bptr;
  short *iptr;
  int *lptr;
  float *fptr;
  double *dptr;
  void *vptr;
};

struct RadarIDLSite {
  IDL_LONG status;
  double tval;
  double geolat;
  double geolon;
  double alt;
  double boresite;
  double bmoff;
  double bmsep;
  double vdir;
  double phidiff;
  double tdiff[2];
  double interfer[3];
  double recrise;
  double atten;
  IDL_LONG maxatten;
  IDL_LONG maxrange;
  IDL_LONG maxbeam;
};

struct RadarIDLTdiff {
  IDL_LONG method;
  IDL_LONG channel;
  double freq[2];
  double tval[2];
  double tdiff;
  double tdiff_err;
};

struct RadarIDLRadar {
  IDL_LONG id;
  IDL_LONG status;
  IDL_LONG cnum;
  IDL_STRING code[8];
  IDL_STRING name;
  IDL_STRING operator;
  IDL_STRING hdwfname;
  double st_time;
  double ed_time;
  IDL_LONG snum;
  struct RadarIDLSite site[32];
  IDL_LONG tnum;
  struct RadarIDLTdiff tdiff[32];
};


int IDLRadarCopyFromIDL(int rnum,int sze,char  *iptr,struct Radar *radar) {
  int r,c,s,t;
  char *cptr;
  struct RadarIDLRadar *iradar;

  memset(radar,0,sizeof(struct Radar)*rnum);

  for (r=0;r<rnum;r++) {
    iradar=(struct RadarIDLRadar *) (iptr+r*sze);
    radar[r].id=iradar->id;
    radar[r].status=iradar->status;
    radar[r].cnum=iradar->cnum;
    radar[r].code=malloc(sizeof(char *)*radar[r].cnum);
    if (radar[r].code==NULL) break;
    for (c=0;c<radar[r].cnum;c++) {
      cptr=IDL_STRING_STR(&iradar->code[c]);
      if (cptr !=NULL) {
        radar[r].code[c]=malloc(strlen(cptr)+1);
        if (radar[r].code[c]==NULL) break;
        strcpy(radar[r].code[c],cptr);
      }
    }
    if (c !=radar[r].cnum) break;
    cptr=IDL_STRING_STR(&iradar->name);
    radar[r].name=malloc(strlen(cptr)+1);
    if (radar[r].name==NULL) break;
    strcpy(radar[r].name,cptr);
    cptr=IDL_STRING_STR(&iradar->operator);
    radar[r].operator=malloc(strlen(cptr)+1);
    if (radar[r].operator==NULL) break;
    strcpy(radar[r].operator,cptr);
    cptr=IDL_STRING_STR(&iradar->hdwfname);
    radar[r].hdwfname=malloc(strlen(cptr)+1);
    if (radar[r].hdwfname==NULL) break;
    strcpy(radar[r].hdwfname,cptr);
    radar[r].st_time=iradar->st_time;
    radar[r].ed_time=iradar->ed_time;
    radar[r].snum=iradar->snum;
    if (radar[r].snum !=0) {
      radar[r].site=malloc(sizeof(struct RadarSite)*
                                    radar[r].snum);

      if (radar[r].site==NULL) break;
      for (s=0;s<radar[r].snum;s++) {
        radar[r].site[s].status=iradar->site[s].status;
        radar[r].site[s].tval=iradar->site[s].tval;
        radar[r].site[s].geolat=iradar->site[s].geolat;
        radar[r].site[s].geolon=iradar->site[s].geolon;
        radar[r].site[s].alt=iradar->site[s].alt;
        radar[r].site[s].boresite=iradar->site[s].boresite;
        radar[r].site[s].bmoff=iradar->site[s].bmoff;
        radar[r].site[s].bmsep=iradar->site[s].bmsep;
        radar[r].site[s].vdir=iradar->site[s].vdir;
        radar[r].site[s].phidiff=iradar->site[s].phidiff;
        radar[r].site[s].tdiff[0]=iradar->site[s].tdiff[0];
        radar[r].site[s].tdiff[1]=iradar->site[s].tdiff[1];
        radar[r].site[s].interfer[0]=iradar->site[s].interfer[0];
        radar[r].site[s].interfer[1]=iradar->site[s].interfer[1];
        radar[r].site[s].interfer[2]=iradar->site[s].interfer[2];
        radar[r].site[s].recrise=iradar->site[s].recrise;
        radar[r].site[s].atten=iradar->site[s].atten;
        radar[r].site[s].maxatten=iradar->site[s].maxatten;
        radar[r].site[s].maxrange=iradar->site[s].maxrange;
        radar[r].site[s].maxbeam=iradar->site[s].maxbeam;
      }
    }
    radar[r].tnum=iradar->tnum;
    if (radar[r].tnum !=0) {
      radar[r].tdiff=malloc(sizeof(struct RadarTdiff)*
                                    radar[r].tnum);

      if (radar[r].tdiff==NULL) break;
      for (t=0;t<radar[r].tnum;t++) {
        radar[r].tdiff[t].method=iradar->tdiff[t].method;
        radar[r].tdiff[t].channel=iradar->tdiff[t].channel;
        radar[r].tdiff[t].freq[0]=iradar->tdiff[t].freq[0];
        radar[r].tdiff[t].freq[1]=iradar->tdiff[t].freq[1];
        radar[r].tdiff[t].tval[0]=iradar->tdiff[t].tval[0];
        radar[r].tdiff[t].tval[1]=iradar->tdiff[t].tval[1];
        radar[r].tdiff[t].tdiff=iradar->tdiff[t].tdiff;
        radar[r].tdiff[t].tdiff_err=iradar->tdiff[t].tdiff_err;
      }
    }
  }
  return r;
}


void IDLRadarCopyToIDL(int rnum,struct Radar *radar,int sze,
                       char *iptr) {

  int r,c,s,t;
  struct RadarIDLRadar *iradar=NULL;

  for (r=0;r<rnum;r++) {
    iradar=(struct RadarIDLRadar *) (iptr+r*sze);

    iradar->id=radar[r].id;
    iradar->status=radar[r].status;
    iradar->cnum=radar[r].cnum;
    for (c=0;c<radar[r].cnum;c++) 
      IDL_StrStore(&iradar->code[c],radar[r].code[c]);
    IDL_StrStore(&iradar->name,radar[r].name);
    IDL_StrStore(&iradar->operator,radar[r].operator);
    IDL_StrStore(&iradar->hdwfname,radar[r].hdwfname);
    iradar->st_time=radar[r].st_time;
    iradar->ed_time=radar[r].ed_time;
    iradar->snum=radar[r].snum;
    for (s=0;s<radar[r].snum;s++) {
      iradar->site[s].status=radar[r].site[s].status;
      iradar->site[s].tval=radar[r].site[s].tval;
      iradar->site[s].geolat=radar[r].site[s].geolat;
      iradar->site[s].geolon=radar[r].site[s].geolon;
      iradar->site[s].alt=radar[r].site[s].alt;
      iradar->site[s].boresite=radar[r].site[s].boresite;
      iradar->site[s].bmoff=radar[r].site[s].bmoff;
      iradar->site[s].bmsep=radar[r].site[s].bmsep;
      iradar->site[s].vdir=radar[r].site[s].vdir;
      iradar->site[s].phidiff=radar[r].site[s].phidiff;
      iradar->site[s].tdiff[0]=radar[r].site[s].tdiff[0];
      iradar->site[s].tdiff[1]=radar[r].site[s].tdiff[1];
      iradar->site[s].interfer[0]=radar[r].site[s].interfer[0];
      iradar->site[s].interfer[1]=radar[r].site[s].interfer[1];
      iradar->site[s].interfer[2]=radar[r].site[s].interfer[2];
      iradar->site[s].recrise=radar[r].site[s].recrise;
      iradar->site[s].atten=radar[r].site[s].atten;
      iradar->site[s].maxatten=radar[r].site[s].maxatten;
      iradar->site[s].maxrange=radar[r].site[s].maxrange;
      iradar->site[s].maxbeam=radar[r].site[s].maxbeam;
    }
    iradar->tnum=radar[r].tnum;
    for (t=0;t<radar[r].tnum;t++) {
      iradar->tdiff[t].method=radar[r].tdiff[t].method;
      iradar->tdiff[t].channel=radar[r].tdiff[t].channel;
      iradar->tdiff[t].freq[0]=radar[r].tdiff[t].freq[0];
      iradar->tdiff[t].freq[1]=radar[r].tdiff[t].freq[1];
      iradar->tdiff[t].tval[0]=radar[r].tdiff[t].tval[0];
      iradar->tdiff[t].tval[1]=radar[r].tdiff[t].tval[1];
      iradar->tdiff[t].tdiff=radar[r].tdiff[t].tdiff;
      iradar->tdiff[t].tdiff_err=radar[r].tdiff[t].tdiff_err;
    }
  }
  return;
}


struct RadarIDLSite *IDLRadarMakeSite(IDL_VPTR *vptr) {

  void *s;

  static IDL_MEMINT tdim[]={1,2};
  static IDL_MEMINT idim[]={1,3};

  static IDL_STRUCT_TAG_DEF site[]={
    {"STATUS",0,(void *) IDL_TYP_LONG},
    {"TVAL",0,(void *) IDL_TYP_DOUBLE},
    {"GEOLAT",0,(void *) IDL_TYP_DOUBLE},
    {"GEOLON",0,(void *) IDL_TYP_DOUBLE},
    {"ALT",0,(void *) IDL_TYP_DOUBLE},
    {"BORESITE",0,(void *) IDL_TYP_DOUBLE},
    {"BMOFF",0,(void *) IDL_TYP_DOUBLE},
    {"BMSEP",0,(void *) IDL_TYP_DOUBLE},
    {"VDIR",0,(void *) IDL_TYP_DOUBLE},
    {"PHIDIFF",0,(void *) IDL_TYP_DOUBLE},
    {"TDIFF",tdim,(void *) IDL_TYP_DOUBLE},
    {"INTERFER",idim,(void *) IDL_TYP_DOUBLE},
    {"RECRISE",0,(void *) IDL_TYP_DOUBLE},
    {"ATTEN",0,(void *) IDL_TYP_DOUBLE},
    {"MAXATTEN",0,(void *) IDL_TYP_LONG},
    {"MAXRANGE",0,(void *) IDL_TYP_LONG},
    {"MAXBEAM",0,(void *) IDL_TYP_LONG},
    {0}
  };

  static IDL_MEMINT ilDims[IDL_MAX_ARRAY_DIM];

  s=IDL_MakeStruct("SITE",site);

  ilDims[0]=1;
  return (struct RadarIDLSite *) IDL_MakeTempStruct(s,1,ilDims,vptr,TRUE);
}


struct RadarIDLTdiff *IDLRadarMakeTdiff(IDL_VPTR *vptr) {

  void *s;

  static IDL_MEMINT dim[]={1,2};

  static IDL_STRUCT_TAG_DEF tdiff[]={
    {"METHOD",0,(void *) IDL_TYP_LONG},
    {"CHANNEL",0,(void *) IDL_TYP_LONG},
    {"FREQ",dim,(void *) IDL_TYP_DOUBLE},
    {"TVAL",dim,(void *) IDL_TYP_DOUBLE},
    {"TDIFF",0,(void *) IDL_TYP_DOUBLE},
    {"TDIFF_ERR",0,(void *) IDL_TYP_DOUBLE},
    {0}
  };

  static IDL_MEMINT ilDims[IDL_MAX_ARRAY_DIM];

  s=IDL_MakeStruct("TDIFF",tdiff);

  ilDims[0]=1;
  return (struct RadarIDLTdiff *) IDL_MakeTempStruct(s,1,ilDims,vptr,TRUE);
}


struct RadarIDLRadar *IDLRadarMakeRadar(int num,IDL_VPTR *vptr) {

  void *s;

  static IDL_MEMINT tdim[]={1,2};
  static IDL_MEMINT idim[]={1,3};
  static IDL_MEMINT cdim[]={1,8};
  static IDL_MEMINT sdim[]={1,32};

  static IDL_STRUCT_TAG_DEF site[]={
    {"STATUS",0,(void *) IDL_TYP_LONG},
    {"TVAL",0,(void *) IDL_TYP_DOUBLE},
    {"GEOLAT",0,(void *) IDL_TYP_DOUBLE},
    {"GEOLON",0,(void *) IDL_TYP_DOUBLE},
    {"ALT",0,(void *) IDL_TYP_DOUBLE},
    {"BORESITE",0,(void *) IDL_TYP_DOUBLE},
    {"BMOFF",0,(void *) IDL_TYP_DOUBLE},
    {"BMSEP",0,(void *) IDL_TYP_DOUBLE},
    {"VDIR",0,(void *) IDL_TYP_DOUBLE},
    {"PHIDIFF",0,(void *) IDL_TYP_DOUBLE},
    {"TDIFF",tdim,(void *) IDL_TYP_DOUBLE},
    {"INTERFER",idim,(void *) IDL_TYP_DOUBLE},
    {"RECRISE",0,(void *) IDL_TYP_DOUBLE},
    {"ATTEN",0,(void *) IDL_TYP_DOUBLE},
    {"MAXATTEN",0,(void *) IDL_TYP_LONG},
    {"MAXRANGE",0,(void *) IDL_TYP_LONG},
    {"MAXBEAM",0,(void *) IDL_TYP_LONG},
    {0}
  };

  static IDL_STRUCT_TAG_DEF tdiff[]={
    {"METHOD",0,(void *) IDL_TYP_LONG},
    {"CHANNEL",0,(void *) IDL_TYP_LONG},
    {"FREQ",tdim,(void *) IDL_TYP_DOUBLE},
    {"TVAL",tdim,(void *) IDL_TYP_DOUBLE},
    {"TDIFF",0,(void *) IDL_TYP_DOUBLE},
    {"TDIFF_ERR",0,(void *) IDL_TYP_DOUBLE},
    {0}
  };

  static IDL_STRUCT_TAG_DEF radar[]={
    {"ID",0,(void *) IDL_TYP_LONG}, /* 0 */
    {"STATUS",0,(void *) IDL_TYP_LONG}, /* 1 */
    {"CNUM",0,(void *) IDL_TYP_LONG}, /* 2 */
    {"CODE",cdim,(void *) IDL_TYP_STRING}, /* 3 */
    {"NAME",0,(void *) IDL_TYP_STRING}, /* 4 */
    {"OPERATOR",0,(void *) IDL_TYP_STRING}, /* 5 */
    {"HDWFNAME",0,(void *) IDL_TYP_STRING}, /* 6 */
    {"ST_TIME",0,(void *) IDL_TYP_DOUBLE}, /* 7 */
    {"ED_TIME",0,(void *) IDL_TYP_DOUBLE}, /* 8 */
    {"SNUM",0,(void *) IDL_TYP_LONG}, /* 9 */
    {"SITE",sdim,NULL}, /* 10 */
    {"TNUM",0,(void *) IDL_TYP_LONG}, /* 11 */
    {"TDIFF",sdim,NULL}, /* 12 */
    {0}};

    static IDL_MEMINT ilDims[IDL_MAX_ARRAY_DIM];

    radar[10].type=IDL_MakeStruct("SITE",site);
    radar[12].type=IDL_MakeStruct("TDIFF",tdiff);

    s=IDL_MakeStruct("RADAR",radar);

    ilDims[0]=num;
    return (struct RadarIDLRadar *) IDL_MakeTempStruct(s,1,ilDims,vptr,TRUE);
}


static IDL_VPTR IDLRadarLoad(int argc,IDL_VPTR *argv) {

  int s=0;
  
  IDL_LONG unit=0;
  IDL_FILE_STAT stat;

  FILE *fp=NULL;

  IDL_VPTR vradar;
  char *iradar=NULL;
  struct RadarNetwork *network=NULL;

  IDL_ENSURE_SCALAR(argv[0]);

  unit=IDL_LongScalar(argv[0]);

  s=IDL_FileEnsureStatus(IDL_MSG_RET,unit,IDL_EFS_USER);

  if (s==FALSE) {
    s=-1;
    return (IDL_GettmpLong(s));
  }

  /* Get information about the file */

  IDL_FileFlushUnit(unit);
  IDL_FileStat(unit,&stat);

  /* Find the file pointer */

  fp=stat.fptr;

  if (fp==NULL) {
    s=-1;
    return (IDL_GettmpLong(s));
  }

  network=RadarLoad(fp);

  iradar=(char *) IDLRadarMakeRadar(network->rnum,&vradar);

  IDLRadarCopyToIDL(network->rnum,
                    network->radar,sizeof(struct RadarIDLRadar),iradar);

  RadarFree(network);

  return (vradar);
}


static IDL_VPTR IDLRadarLoadHardware(int argc,IDL_VPTR *argv,char *argk) {

  int s=0;

  static IDL_STRING spath;
  static int pflg=0;

  char *iradar=NULL;
  struct RadarNetwork *network=NULL;

  int num=0,sze;
  IDL_VPTR outargv[8];
  static IDL_KW_PAR kw_pars[]={IDL_KW_FAST_SCAN,
                               {"PATH",IDL_TYP_STRING,1,
                                0,&pflg,
                                IDL_CHARA(spath)},
                               {NULL}};

  char *path=NULL;

  IDL_KWCleanup(IDL_KW_MARK);
  IDL_KWGetParams(argc,argv,argk,kw_pars,outargv,1);

  IDL_ENSURE_ARRAY(outargv[0]);

  if (pflg) path=IDL_STRING_STR(&spath);

  num=outargv[0]->value.s.arr->n_elts;
  sze=outargv[0]->value.s.arr->elt_len;
  iradar=(char *) outargv[0]->value.s.arr->data;

  network=malloc(sizeof(struct RadarNetwork));
  if (network==NULL) {
    IDL_MessageFromBlock(msg_block,RPOS_BADSTRUCT,IDL_MSG_LONGJMP,
                         "in RadarLoadHardware()");
  }
  network->rnum=num;
  network->radar=malloc(sizeof(struct Radar)*num);

  if (network->radar==NULL) {
    free(network);
    IDL_MessageFromBlock(msg_block,RPOS_BADSTRUCT,IDL_MSG_LONGJMP,
                         "in RadarLoadHardware()");
  }

  s=IDLRadarCopyFromIDL(num,sze,iradar,network->radar);

  if (s !=num) {
    RadarFree(network);
    IDL_MessageFromBlock(msg_block,RPOS_BADSTRUCT,IDL_MSG_LONGJMP,
                         "in RadarLoadHardware()");
  }

  RadarLoadHardware(path,network);

  IDLRadarCopyToIDL(num,network->radar,sze,iradar);

  RadarFree(network);

  IDL_KWCleanup(IDL_KW_CLEAN);

  return (IDL_GettmpLong(0));
}


static IDL_VPTR IDLRadarLoadTdiff(int argc,IDL_VPTR *argv,char *argk) {

  int s=0;

  static IDL_STRING tpath;
  static int pflg=0;

  char *iradar=NULL;
  struct RadarNetwork *network=NULL;

  int num=0,sze;
  IDL_VPTR outargv[8];
  static IDL_KW_PAR kw_pars[]={IDL_KW_FAST_SCAN,
                               {"PATH",IDL_TYP_STRING,1,
                                0,&pflg,
                                IDL_CHARA(tpath)},
                               {NULL}};

  char *path=NULL;

  IDL_KWCleanup(IDL_KW_MARK);
  IDL_KWGetParams(argc,argv,argk,kw_pars,outargv,1);

  IDL_ENSURE_ARRAY(outargv[0]);

  if (pflg) path=IDL_STRING_STR(&tpath);

  num=outargv[0]->value.s.arr->n_elts;
  sze=outargv[0]->value.s.arr->elt_len;
  iradar=(char *) outargv[0]->value.s.arr->data;

  network=malloc(sizeof(struct RadarNetwork));
  if (network==NULL) {
    IDL_MessageFromBlock(msg_block,RPOS_BADSTRUCT,IDL_MSG_LONGJMP,
                         "in RadarLoadTdiff()");
  }
  network->rnum=num;
  network->radar=malloc(sizeof(struct Radar)*num);

  if (network->radar==NULL) {
    free(network);
    IDL_MessageFromBlock(msg_block,RPOS_BADSTRUCT,IDL_MSG_LONGJMP,
                         "in RadarLoadTdiff()");
  }

  s=IDLRadarCopyFromIDL(num,sze,iradar,network->radar);

  if (s !=num) {
    RadarFree(network);
    IDL_MessageFromBlock(msg_block,RPOS_BADSTRUCT,IDL_MSG_LONGJMP,
                         "in RadarLoadTdiff()");
  }

  RadarLoadTdiff(path,network);

  IDLRadarCopyToIDL(num,network->radar,sze,iradar);

  RadarFree(network);

  IDL_KWCleanup(IDL_KW_CLEAN);

  return (IDL_GettmpLong(0));
}


static IDL_VPTR IDLRadarEpochGetSite(int argc,IDL_VPTR *argv) {
  int s;

  IDL_VPTR vsite;
  struct RadarIDLRadar *iradar=NULL;
  struct RadarIDLSite *isite=NULL;

  double tval;
  
  IDL_ENSURE_STRUCTURE(argv[0]);
  IDL_ENSURE_SCALAR(argv[1]);

  iradar=(struct RadarIDLRadar *) argv[0]->value.s.arr->data;
  tval=IDL_DoubleScalar(argv[1]);

  if ((iradar->st_time !=-1) && (tval<iradar->st_time)) 
    return (IDL_GettmpLong(0));
  if ((iradar->ed_time !=-1) && (tval>iradar->ed_time)) 
    return (IDL_GettmpLong(0));
  for (s=0;(s<iradar->snum) && (iradar->site[s].tval<=tval);s++);
  s=s-1;

  isite=IDLRadarMakeSite(&vsite);

  isite->status=iradar->site[s].status;
  isite->tval=iradar->site[s].tval;
  isite->geolat=iradar->site[s].geolat;
  isite->geolon=iradar->site[s].geolon;
  isite->alt=iradar->site[s].alt;
  isite->boresite=iradar->site[s].boresite;
  isite->bmoff=iradar->site[s].bmoff;
  isite->bmsep=iradar->site[s].bmsep;
  isite->vdir=iradar->site[s].vdir;
  isite->phidiff=iradar->site[s].phidiff;
  isite->tdiff[0]=iradar->site[s].tdiff[0];
  isite->tdiff[1]=iradar->site[s].tdiff[1];
  isite->interfer[0]=iradar->site[s].interfer[0];
  isite->interfer[1]=iradar->site[s].interfer[1];
  isite->interfer[2]=iradar->site[s].interfer[2];
  isite->recrise=iradar->site[s].recrise;
  isite->atten=iradar->site[s].atten;
  isite->maxatten=iradar->site[s].maxatten;
  isite->maxrange=iradar->site[s].maxrange;
  isite->maxbeam=iradar->site[s].maxbeam;

  return (vsite);
}


static IDL_VPTR IDLRadarYMDHMSGetSite(int argc,IDL_VPTR *argv) {

  IDL_VPTR vsite;
  struct RadarIDLRadar *iradar=NULL;
  struct RadarIDLSite *isite=NULL;

  int s;
  double tval;
  int yr,mo,dy,hr,mt,sc;

  IDL_ENSURE_STRUCTURE(argv[0]);
  IDL_ENSURE_SCALAR(argv[1]);
  IDL_ENSURE_SCALAR(argv[2]);
  IDL_ENSURE_SCALAR(argv[3]);
  IDL_ENSURE_SCALAR(argv[4]);
  IDL_ENSURE_SCALAR(argv[5]);
  IDL_ENSURE_SCALAR(argv[6]);

  yr=IDL_LongScalar(argv[1]);
  mo=IDL_LongScalar(argv[2]);
  dy=IDL_LongScalar(argv[3]);
  hr=IDL_LongScalar(argv[4]);
  mt=IDL_LongScalar(argv[5]);
  sc=IDL_LongScalar(argv[6]);

  tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);

  iradar=(struct RadarIDLRadar *) argv[0]->value.s.arr->data;

  if ((iradar->st_time !=-1) && (tval<iradar->st_time)) 
    return (IDL_GettmpLong(0));
  if ((iradar->ed_time !=-1) && (tval>iradar->ed_time)) 
    return (IDL_GettmpLong(0));
  for (s=0;(s<iradar->snum) && (iradar->site[s].tval<=tval);s++);
  s=s-1;

  isite=IDLRadarMakeSite(&vsite);

  isite->status=iradar->site[s].status;
  isite->tval=iradar->site[s].tval;
  isite->geolat=iradar->site[s].geolat;
  isite->geolon=iradar->site[s].geolon;
  isite->alt=iradar->site[s].alt;
  isite->boresite=iradar->site[s].boresite;
  isite->bmoff=iradar->site[s].bmoff;
  isite->bmsep=iradar->site[s].bmsep;
  isite->vdir=iradar->site[s].vdir;
  isite->phidiff=iradar->site[s].phidiff;
  isite->tdiff[0]=iradar->site[s].tdiff[0];
  isite->tdiff[1]=iradar->site[s].tdiff[1];
  isite->interfer[0]=iradar->site[s].interfer[0];
  isite->interfer[1]=iradar->site[s].interfer[1];
  isite->interfer[2]=iradar->site[s].interfer[2];
  isite->recrise=iradar->site[s].recrise;
  isite->atten=iradar->site[s].atten;
  isite->maxatten=iradar->site[s].maxatten;
  isite->maxrange=iradar->site[s].maxrange;
  isite->maxbeam=iradar->site[s].maxbeam;

  return (vsite);
}


static IDL_VPTR IDLRadarEpochGetTdiff(int argc,IDL_VPTR *argv) {
  int t;

  IDL_VPTR vtdiff;
  struct RadarIDLRadar *iradar=NULL;
  struct RadarIDLTdiff *itdiff=NULL;

  double tval;
  int method,channel,tfreq;

  IDL_ENSURE_STRUCTURE(argv[0]);
  IDL_ENSURE_SCALAR(argv[1]);
  IDL_ENSURE_SCALAR(argv[2]);
  IDL_ENSURE_SCALAR(argv[3]);
  IDL_ENSURE_SCALAR(argv[4]);

  iradar=(struct RadarIDLRadar *) argv[0]->value.s.arr->data;
  tval=IDL_DoubleScalar(argv[1]);
  method=IDL_LongScalar(argv[2]);
  channel=IDL_LongScalar(argv[3]);
  tfreq=IDL_LongScalar(argv[4]);

  for (t=0;(t<iradar->tnum);t++) {
    if (iradar->tdiff[t].method !=method) continue;
    if (iradar->tdiff[t].channel !=channel) continue;
    if (iradar->tdiff[t].freq[0] > tfreq) continue;
    if (iradar->tdiff[t].freq[1] < tfreq) continue;
    if (iradar->tdiff[t].tval[0] > tval) continue;
    if (iradar->tdiff[t].tval[1] < tval) continue;
    break;
  }
  if (t==iradar->tnum) return (IDL_GettmpLong(0));

  itdiff=IDLRadarMakeTdiff(&vtdiff);

  itdiff->method=iradar->tdiff[t].method;
  itdiff->channel=iradar->tdiff[t].channel;
  itdiff->freq[0]=iradar->tdiff[t].freq[0];
  itdiff->freq[1]=iradar->tdiff[t].freq[1];
  itdiff->tval[0]=iradar->tdiff[t].tval[0];
  itdiff->tval[1]=iradar->tdiff[t].tval[1];
  itdiff->tdiff=iradar->tdiff[t].tdiff;
  itdiff->tdiff_err=iradar->tdiff[t].tdiff_err;

  return (vtdiff);
}


static IDL_VPTR IDLRadarYMDHMSGetTdiff(int argc,IDL_VPTR *argv) {

  IDL_VPTR vtdiff;
  struct RadarIDLRadar *iradar=NULL;
  struct RadarIDLTdiff *itdiff=NULL;

  int t;
  double tval;
  int method,channel,tfreq;
  int yr,mo,dy,hr,mt,sc;

  IDL_ENSURE_STRUCTURE(argv[0]);
  IDL_ENSURE_SCALAR(argv[1]);
  IDL_ENSURE_SCALAR(argv[2]);
  IDL_ENSURE_SCALAR(argv[3]);
  IDL_ENSURE_SCALAR(argv[4]);
  IDL_ENSURE_SCALAR(argv[5]);
  IDL_ENSURE_SCALAR(argv[6]);
  IDL_ENSURE_SCALAR(argv[7]);
  IDL_ENSURE_SCALAR(argv[8]);
  IDL_ENSURE_SCALAR(argv[9]);

  yr=IDL_LongScalar(argv[1]);
  mo=IDL_LongScalar(argv[2]);
  dy=IDL_LongScalar(argv[3]);
  hr=IDL_LongScalar(argv[4]);
  mt=IDL_LongScalar(argv[5]);
  sc=IDL_LongScalar(argv[6]);
  method=IDL_LongScalar(argv[7]);
  channel=IDL_LongScalar(argv[8]);
  tfreq=IDL_LongScalar(argv[9]);

  tval=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);

  iradar=(struct RadarIDLRadar *) argv[0]->value.s.arr->data;

  for (t=0;(t<iradar->tnum);t++) {
    if (iradar->tdiff[t].method !=method) continue;
    if (iradar->tdiff[t].channel !=channel) continue;
    if (iradar->tdiff[t].freq[0] > tfreq) continue;
    if (iradar->tdiff[t].freq[1] < tfreq) continue;
    if (iradar->tdiff[t].tval[0] > tval) continue;
    if (iradar->tdiff[t].tval[1] < tval) continue;
    break;
  }
  if (t==iradar->tnum) return (IDL_GettmpLong(0));

  itdiff=IDLRadarMakeTdiff(&vtdiff);

  itdiff->method=iradar->tdiff[t].method;
  itdiff->channel=iradar->tdiff[t].channel;
  itdiff->freq[0]=iradar->tdiff[t].freq[0];
  itdiff->freq[1]=iradar->tdiff[t].freq[1];
  itdiff->tval[0]=iradar->tdiff[t].tval[0];
  itdiff->tval[1]=iradar->tdiff[t].tval[1];
  itdiff->tdiff=iradar->tdiff[t].tdiff;
  itdiff->tdiff_err=iradar->tdiff[t].tdiff_err;

  return (vtdiff);
}


static IDL_VPTR IDLRadarGetRadar(int argc,IDL_VPTR *argv) {

  IDL_LONG id;

  IDL_VPTR vradar;
  struct RadarIDLRadar *iradar=NULL;
  struct RadarIDLRadar *oradar=NULL;
  char *iptr=NULL;
  int num,sze;
  int r,c,s,t;

  IDL_ENSURE_ARRAY(argv[0]);
  IDL_ENSURE_SCALAR(argv[1]);

  id=IDL_LongScalar(argv[1]);

  num=argv[0]->value.s.arr->n_elts;
  sze=argv[0]->value.s.arr->elt_len;
  iptr=(char *) argv[0]->value.s.arr->data;

  id=IDL_LongScalar(argv[1]);

  for (r=0;r<num;r++) {
    iradar=(struct RadarIDLRadar *) (iptr+r*sze);
    if (iradar->id==id) break;
  }
  if (r==num) return (IDL_GettmpLong(0));

  oradar=IDLRadarMakeRadar(1,&vradar);

  oradar->id=iradar->id;
  oradar->status=iradar->status;
  oradar->cnum=iradar->cnum;
  for (c=0;c<iradar->cnum;c++) {
    oradar->code[c]=iradar->code[c];
    IDL_StrDup(&oradar->code[c],1);
  }
  oradar->name=iradar->name;
  IDL_StrDup(&oradar->name,1);
  oradar->operator=iradar->operator;
  IDL_StrDup(&oradar->operator,1);
  oradar->hdwfname=iradar->hdwfname;
  IDL_StrDup(&oradar->hdwfname,1);
  oradar->st_time=iradar->st_time;
  oradar->ed_time=iradar->ed_time;
  oradar->snum=iradar->snum;
  for (s=0;s<iradar->snum;s++) {
    oradar->site[s].status=iradar->site[s].status;
    oradar->site[s].tval=iradar->site[s].tval;
    oradar->site[s].geolat=iradar->site[s].geolat;
    oradar->site[s].geolon=iradar->site[s].geolon;
    oradar->site[s].alt=iradar->site[s].alt;
    oradar->site[s].boresite=iradar->site[s].boresite;
    oradar->site[s].bmoff=iradar->site[s].bmoff;
    oradar->site[s].bmsep=iradar->site[s].bmsep;
    oradar->site[s].vdir=iradar->site[s].vdir;
    oradar->site[s].phidiff=iradar->site[s].phidiff;
    oradar->site[s].tdiff[0]=iradar->site[s].tdiff[0];
    oradar->site[s].tdiff[1]=iradar->site[s].tdiff[1];
    oradar->site[s].interfer[0]=iradar->site[s].interfer[0];
    oradar->site[s].interfer[1]=iradar->site[s].interfer[1];
    oradar->site[s].interfer[2]=iradar->site[s].interfer[2];
    oradar->site[s].recrise=iradar->site[s].recrise;
    oradar->site[s].atten=iradar->site[s].atten;
    oradar->site[s].maxatten=iradar->site[s].maxatten;
    oradar->site[s].maxrange=iradar->site[s].maxrange;
    oradar->site[s].maxbeam=iradar->site[s].maxbeam;
  }
  oradar->tnum=iradar->tnum;
  for (t=0;t<iradar->tnum;t++) {
    oradar->tdiff[t].method=iradar->tdiff[t].method;
    oradar->tdiff[t].channel=iradar->tdiff[t].channel;
    oradar->tdiff[t].freq[0]=iradar->tdiff[t].freq[0];
    oradar->tdiff[t].freq[1]=iradar->tdiff[t].freq[1];
    oradar->tdiff[t].tval[0]=iradar->tdiff[t].tval[0];
    oradar->tdiff[t].tval[1]=iradar->tdiff[t].tval[1];
    oradar->tdiff[t].tdiff=iradar->tdiff[t].tdiff;
    oradar->tdiff[t].tdiff_err=iradar->tdiff[t].tdiff_err;
  }
  return (vradar);
}


static IDL_VPTR IDLRadarConvert(int type,int argc,IDL_VPTR *argv,char *argk) {
  int s=0,n=0;
  int center=0;
  int bcrd=0,rcrd=0;
  struct RadarSite site;
  struct RadarIDLSite *isite;
  int frang=180,rsep=45,rxrise=0;
  double height=300;
  double rho,lat,lng,srng;

  IDL_VPTR outargv[11];
  static IDL_LONG chisham;

  static IDL_KW_PAR kw_pars[]={IDL_KW_FAST_SCAN,
      {"CHISHAM",IDL_TYP_LONG,1,IDL_KW_ZERO,0,IDL_CHARA(chisham)},
      {NULL}};

  IDL_KWCleanup(IDL_KW_MARK);
  IDL_KWGetParams(argc,argv,argk,kw_pars,outargv,1);

  IDL_ENSURE_SCALAR(argv[0]);
  IDL_ENSURE_STRUCTURE(argv[3]);

  IDL_EXCLUDE_EXPR(argv[8]);
  IDL_EXCLUDE_EXPR(argv[9]);
  IDL_EXCLUDE_EXPR(argv[10]);

  center=IDL_LongScalar(argv[0]);

  isite=(struct RadarIDLSite *) argv[3]->value.s.arr->data;

  site.status=isite->status;
  site.tval=isite->tval;
  site.geolat=isite->geolat;
  site.geolon=isite->geolon;
  site.alt=isite->alt;
  site.boresite=isite->boresite;
  site.bmoff=isite->bmoff;
  site.bmsep=isite->bmsep;
  site.vdir=isite->vdir;
  site.phidiff=isite->phidiff;
  site.tdiff[0]=isite->tdiff[0];
  site.tdiff[1]=isite->tdiff[1];
  site.interfer[0]=isite->interfer[0];
  site.interfer[1]=isite->interfer[1];
  site.interfer[2]=isite->interfer[2];
  site.recrise=isite->recrise;
  site.atten=isite->atten;
  site.maxatten=isite->maxatten;
  site.maxrange=isite->maxrange;
  site.maxbeam=isite->maxbeam;

  if (argv[1]->flags & IDL_V_ARR) {
    union RadarIDLPtr b,r;
    union RadarIDLPtr fr,rs,rx,ht;
    int btyp=0,rtyp=0,frtyp=0,rstyp=0,rxtyp=0,httyp=0;

    IDL_VPTR vlat,vlon,vr;

    double *latptr=NULL,*lonptr=NULL,*rptr=NULL;
    int nval;

    fr.vptr=NULL;
    rs.vptr=NULL;
    rx.vptr=NULL;
    ht.vptr=NULL;

    nval=argv[1]->value.arr->n_elts;
    b.vptr=(void *) argv[1]->value.arr->data;
    btyp=argv[1]->type;

    IDL_ENSURE_ARRAY(argv[2]);

    if (argv[2]->value.arr->n_elts !=nval) {
      /* array dimensions mismatch */
      IDL_MessageFromBlock(msg_block,RPOS_BADRNGARR,IDL_MSG_LONGJMP,
                           "in RadarPos()");
    }

    r.vptr=(void *) argv[2]->value.arr->data;
    rtyp=argv[2]->type;

    if (argv[4]->flags & IDL_V_ARR) {
      if (argv[4]->value.arr->n_elts !=nval) 
        IDL_MessageFromBlock(msg_block,RPOS_BADFRANGARR,IDL_MSG_LONGJMP,
                           "in RadarPos()");

      fr.vptr=(void *) argv[4]->value.arr->data;
      frtyp=argv[4]->type;

    } else frang=IDL_LongScalar(argv[4]);

    if (argv[5]->flags & IDL_V_ARR) {
      if (argv[5]->value.arr->n_elts !=nval) 
        IDL_MessageFromBlock(msg_block,RPOS_BADRSEPARR,IDL_MSG_LONGJMP,
                           "in RadarPos()");
      rs.vptr=(void *) argv[5]->value.arr->data;
      rstyp=argv[5]->type;
    } else rsep=IDL_LongScalar(argv[5]);

    if (argv[6]->flags & IDL_V_ARR) {
      if (argv[6]->value.arr->n_elts !=nval) 
        IDL_MessageFromBlock(msg_block,RPOS_BADRXRISEARR,IDL_MSG_LONGJMP,
                           "in RadarPos()");
      rx.vptr=(void *) argv[6]->value.arr->data;
      rxtyp=argv[6]->type;
    } else rxrise=IDL_LongScalar(argv[6]);

    if (argv[7]->flags & IDL_V_ARR) {
      if (argv[7]->value.arr->n_elts !=nval) 
        IDL_MessageFromBlock(msg_block,RPOS_BADHEIGHTARR,IDL_MSG_LONGJMP,
                           "in RadarPos()");
      ht.vptr=(void *) argv[7]->value.arr->data;
      httyp=argv[7]->type;
    } else height=IDL_LongScalar(argv[7]);

    latptr=(double *) IDL_MakeTempArray(IDL_TYP_DOUBLE,
                                        argv[1]->value.arr->n_dim,
                                        argv[1]->value.arr->dim,
                                        IDL_ARR_INI_ZERO,&vlat);
    lonptr=(double *) IDL_MakeTempArray(IDL_TYP_DOUBLE,
                                        argv[1]->value.arr->n_dim,
                                        argv[1]->value.arr->dim,
                                        IDL_ARR_INI_ZERO,&vlon);

    rptr=(double *) IDL_MakeTempArray(IDL_TYP_DOUBLE,
                                      argv[1]->value.arr->n_dim,
                                      argv[1]->value.arr->dim,
                                      IDL_ARR_INI_ZERO,&vr);

    for (n=0;n<nval;n++) {

      switch (btyp) {
        case IDL_TYP_BYTE:
          bcrd=b.bptr[n];
          break;
        case IDL_TYP_INT:
          bcrd=b.iptr[n];
          break;
        case IDL_TYP_LONG:
          bcrd=b.lptr[n];
          break;
        case IDL_TYP_FLOAT:
          bcrd=b.fptr[n];
          break;
        case IDL_TYP_DOUBLE:
          bcrd=b.dptr[n];
          break;
        default:
          bcrd=0;
      }

      switch (rtyp) {
        case IDL_TYP_BYTE:
          rcrd=r.bptr[n];
          break;
        case IDL_TYP_INT:
          rcrd=r.iptr[n];
          break;
        case IDL_TYP_LONG:
          rcrd=r.lptr[n];
          break;
        case IDL_TYP_FLOAT:
          rcrd=r.fptr[n];
          break;
        case IDL_TYP_DOUBLE:
          rcrd=r.dptr[n];
          break;
        default:
          rcrd=0;
     }

     if (fr.vptr !=NULL) {
       switch (frtyp) {
         case IDL_TYP_BYTE:
           frang=fr.bptr[n];
           break;
         case IDL_TYP_INT:
           frang=fr.iptr[n];
           break;
         case IDL_TYP_LONG:
           frang=fr.lptr[n];
           break;
         case IDL_TYP_FLOAT:
           frang=fr.fptr[n];
           break;
         case IDL_TYP_DOUBLE:
           frang=fr.dptr[n];
           break;
         default:
           frang=180;
       }
     }

     if (rs.vptr !=NULL) {
       switch (rstyp) {
         case IDL_TYP_BYTE:
           rsep=rs.bptr[n];
           break;
         case IDL_TYP_INT:
           rsep=rs.iptr[n];
           break;
         case IDL_TYP_LONG:
           rsep=rs.lptr[n];
           break;
         case IDL_TYP_FLOAT:
           rsep=rs.fptr[n];
           break;
         case IDL_TYP_DOUBLE:
           rsep=rs.dptr[n];
           break;
         default:
           rsep=45;
       }
     }

    if (rx.vptr !=NULL) {
       switch (rxtyp) {
         case IDL_TYP_BYTE:
           rxrise=rx.bptr[n];
           break;
         case IDL_TYP_INT:
           rxrise=rx.iptr[n];
           break;
         case IDL_TYP_LONG:
           rxrise=rx.lptr[n];
           break;
         case IDL_TYP_FLOAT:
           rxrise=rx.fptr[n];
           break;
         case IDL_TYP_DOUBLE:
           rxrise=rx.dptr[n];
           break;
         default:
           rxrise=0;
       }
     }

    if (ht.vptr !=NULL) {
       switch (httyp) {
         case IDL_TYP_BYTE:
           height=ht.bptr[n];
           break;
         case IDL_TYP_INT:
           height=ht.iptr[n];
           break;
         case IDL_TYP_LONG:
           height=ht.lptr[n];
           break;
         case IDL_TYP_FLOAT:
           height=ht.fptr[n];
           break;
         case IDL_TYP_DOUBLE:
           height=ht.dptr[n];
           break;
         default:
           height=300;
       }
     }

    if (type !=0) RPosGeoGS(center,bcrd,rcrd,&site,frang,rsep,rxrise,height,
                            &rho,&lat,&lng);
    else RPosGeo(center,bcrd,rcrd,&site,frang,rsep,rxrise,height,
                 &rho,&lat,&lng,&srng,chisham);

     rptr[n]=rho;
     latptr[n]=lat;
     lonptr[n]=lng;

    }

    IDL_VarCopy(vr,argv[8]);
    IDL_VarCopy(vlat,argv[9]);
    IDL_VarCopy(vlon,argv[10]);

  } else {

    IDL_ENSURE_SCALAR(argv[2]);
    IDL_ENSURE_SCALAR(argv[4]);
    IDL_ENSURE_SCALAR(argv[5]);
    IDL_ENSURE_SCALAR(argv[6]);
    IDL_ENSURE_SCALAR(argv[7]);

    bcrd=IDL_LongScalar(argv[1]);
    rcrd=IDL_LongScalar(argv[2]);
    frang=IDL_LongScalar(argv[4]);
    rsep=IDL_LongScalar(argv[5]);
    rxrise=IDL_LongScalar(argv[6]);
    height=IDL_DoubleScalar(argv[7]);

    if (type !=0) RPosGeoGS(center,bcrd,rcrd,&site,frang,rsep,rxrise,height,
                            &rho,&lat,&lng);
    else RPosGeo(center,bcrd,rcrd,&site,frang,rsep,rxrise,height,
                 &rho,&lat,&lng,&srng,chisham);

    IDL_StoreScalar(argv[8],IDL_TYP_DOUBLE,(IDL_ALLTYPES *) &rho);
    IDL_StoreScalar(argv[9],IDL_TYP_DOUBLE,(IDL_ALLTYPES *) &lat);
    IDL_StoreScalar(argv[10],IDL_TYP_DOUBLE,(IDL_ALLTYPES *) &lng);

  }

  IDL_KWCleanup(IDL_KW_CLEAN);
  return (IDL_GettmpLong(s));
}


static IDL_VPTR IDLRadarPos(int argc,IDL_VPTR *argv,char *argk) {

  return IDLRadarConvert(0,argc,argv,argk);
}


static IDL_VPTR IDLRadarPosGS(int argc,IDL_VPTR *argv,char *argk) {

    return IDLRadarConvert(1,argc,argv,argk);
}


int IDL_Load(void) {

  static IDL_SYSFUN_DEF2 fnaddr[]={
    { {IDLRadarLoad},"RADARLOAD",1,1,0,0},
    { {IDLRadarLoadHardware},"RADARLOADHARDWARE",1,1,IDL_SYSFUN_DEF_F_KEYWORDS,0},
    { {IDLRadarLoadTdiff},"RADARLOADTDIFF",1,1,IDL_SYSFUN_DEF_F_KEYWORDS,0},
    { {IDLRadarEpochGetSite},"RADAREPOCHGETSITE",2,2,0,0},
    { {IDLRadarYMDHMSGetSite},"RADARYMDHMSGETSITE",7,7,0,0},
    { {IDLRadarEpochGetTdiff},"RADAREPOCHGETTDIFF",5,5,0,0},
    { {IDLRadarYMDHMSGetTdiff},"RADARYMDHMSGETTDIFF",10,10,0,0},
    { {IDLRadarGetRadar},"RADARGETRADAR",2,2,0,0},
    { {IDLRadarPos},"RADARPOS",11,11,IDL_SYSFUN_DEF_F_KEYWORDS,0},
    { {IDLRadarPosGS},"RADARPOSGS",11,11,IDL_SYSFUN_DEF_F_KEYWORDS,0},
  };

  if (!(msg_block = IDL_MessageDefineBlock("rpos",
                    IDL_CARRAY_ELTS(msg_arr), msg_arr)))
    return IDL_FALSE;

  return IDL_SysRtnAdd(fnaddr,TRUE,IDL_CARRAY_ELTS(fnaddr));

}
