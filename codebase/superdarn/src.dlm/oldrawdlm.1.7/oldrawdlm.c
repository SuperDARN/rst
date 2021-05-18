/* oldrawdlm.c
   =========== 
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
#include "rawdata.h"
#include "oldrawread.h"
#include "oldrawwrite.h"

#include "rprmidl.h"
#include "rawidl.h"

#include "oldrawidl.h"

#define OLDRAW_ERROR 0

static IDL_MSG_DEF msg_arr[] = {
    {  "OLDRAW_ERROR",   "%NError: %s." }, 
  };

static IDL_MSG_BLOCK msg_block;


void OldRawIDLToRawFp(struct OldRawIDLFp *irawfp,struct OldRawFp *rawfp) {

  rawfp->ctime=irawfp->ctime;
  rawfp->stime=irawfp->stime;
  rawfp->frec=irawfp->frec;
  rawfp->rlen=irawfp->rlen;
  rawfp->ptr=irawfp->ptr;
  rawfp->thr=irawfp->thr;
  rawfp->major_rev=irawfp->major_rev;
  rawfp->minor_rev=irawfp->minor_rev;
  rawfp->rawread=irawfp->rawread;
}

void OldRawRawFpToIDL(struct OldRawFp *rawfp,struct OldRawIDLFp *irawfp) {
  irawfp->ctime=rawfp->ctime;
  irawfp->stime=rawfp->stime;
  irawfp->frec=rawfp->frec;
  irawfp->rlen=rawfp->rlen;
  irawfp->ptr=rawfp->ptr;
  irawfp->thr=rawfp->thr;
  irawfp->major_rev=rawfp->major_rev;
  irawfp->minor_rev=rawfp->minor_rev;
  irawfp->rawread=rawfp->rawread;
}

static IDL_VPTR IDLOldRawWriteHeader(int argc,IDL_VPTR *argv) {
  int s=0;
  IDL_LONG unit=0;
  IDL_FILE_STAT stat;

  struct RadarIDLParm *iprm=NULL;
  struct RawIDLData *iraw=NULL;
  FILE *fp;

  char vstring[256];

  IDL_ENSURE_SCALAR(argv[0]);
  IDL_ENSURE_STRUCTURE(argv[1]);
  IDL_ENSURE_STRUCTURE(argv[2]);
  IDL_EXCLUDE_EXPR(argv[1]);
  IDL_EXCLUDE_EXPR(argv[2]);

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

  iprm=(struct RadarIDLParm *) argv[1]->value.s.arr->data;
  iraw=(struct RawIDLData *) argv[2]->value.s.arr->data;

  sprintf(vstring,"%d.%.3d",(int) iraw->revision.major,
	                    (int) iraw->revision.minor);
  s=OldRawHeaderFwrite(fp,"rawwrite",vstring,iraw->thr,"IDL output");

  return (IDL_GettmpLong(s));
}



static IDL_VPTR IDLOldRawWrite(int argc,IDL_VPTR *argv) {
  int s=0;
  IDL_LONG unit=0,recnum;
  IDL_FILE_STAT stat;

  struct RadarIDLParm *iprm=NULL;
  struct RawIDLData *iraw=NULL;

  struct RadarParm *prm=NULL;
  struct RawData *raw=NULL;

  FILE *fp;

  IDL_ENSURE_SCALAR(argv[0]);
  IDL_ENSURE_STRUCTURE(argv[1]);
  IDL_ENSURE_STRUCTURE(argv[2]);
  IDL_ENSURE_SCALAR(argv[3]);
  IDL_EXCLUDE_EXPR(argv[1]);
  IDL_EXCLUDE_EXPR(argv[2]);

  unit=IDL_LongScalar(argv[0]);
  recnum=IDL_LongScalar(argv[3]);

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

  iprm=(struct RadarIDLParm *) argv[1]->value.s.arr->data;
  iraw=(struct RawIDLData *) argv[2]->value.s.arr->data;

  prm=RadarParmMake();
  raw=RawMake();

  IDLCopyRadarParmFromIDL(iprm,prm);
  IDLCopyRawDataFromIDL(prm->nrang,prm->mplgs,prm->xcf,iraw,raw);

  s=OldRawFwrite(fp,"rawwrite",prm,raw,recnum,NULL);

  RadarParmFree(prm);
  RawFree(raw);



  return (IDL_GettmpLong(s));
}

static IDL_VPTR IDLOldRawRead(int argc,IDL_VPTR *argv) {

  int s=0;

  IDL_VPTR vprm=NULL,vraw=NULL;

  IDL_FILE_STAT stat;

  struct OldRawIDLFp *irawfp;
  struct OldRawFp rawfp;

  struct RadarIDLParm *iprm=NULL;
  struct RawIDLData *iraw=NULL;

  struct RadarParm *prm=NULL;
  struct RawData *raw=NULL;

  FILE *ffp=NULL,*ifp=NULL;

  IDL_ENSURE_STRUCTURE(argv[0]);
  IDL_EXCLUDE_EXPR(argv[1]);
  IDL_EXCLUDE_EXPR(argv[2]);

  
  irawfp=(struct OldRawIDLFp *) argv[0]->value.s.arr->data;
  
  s=IDL_FileEnsureStatus(IDL_MSG_RET,irawfp->rawunit,IDL_EFS_USER);

  if (s==FALSE) {
    s=-1;
    return (IDL_GettmpLong(s));
  }

  IDL_FileFlushUnit(irawfp->rawunit);
  IDL_FileStat(irawfp->rawunit,&stat);
 

  ffp=stat.fptr;
  
  if (ffp==NULL) {
    s=-1;
    return (IDL_GettmpLong(s));
  }
 
  if (irawfp->inxunit !=-1) {
    IDL_FileFlushUnit(irawfp->inxunit);
    IDL_FileStat(irawfp->inxunit,&stat);
    ifp=stat.fptr;
  }

  if (ifp !=NULL) fflush(ifp);

  rawfp.rawfp=fileno(ffp);
  if (ifp !=NULL) rawfp.inxfp=fileno(ifp);
  else rawfp.inxfp=-1;

  OldRawIDLToRawFp(irawfp,&rawfp);

  prm=RadarParmMake();
  raw=RawMake(); 

  s=OldRawRead(&rawfp,prm,raw);

  OldRawRawFpToIDL(&rawfp,irawfp);
 
  if (s==-1) {
    RadarParmFree(prm);
    RawFree(raw);
    return (IDL_GettmpLong(s));
  }

  iprm=IDLMakeRadarParm(&vprm);
  iraw=IDLMakeRawData(&vraw);

  /* copy data here */



  IDLCopyRadarParmToIDL(prm,iprm);
  IDLCopyRawDataToIDL(prm->nrang,prm->mplgs,prm->xcf,raw,iraw);

  RadarParmFree(prm);
  RawFree(raw);

  IDL_VarCopy(vprm,argv[1]);  
  IDL_VarCopy(vraw,argv[2]);
    
  return (IDL_GettmpLong(s));

}


static IDL_VPTR IDLOldRawSeek(int argc,IDL_VPTR *argv,char *argk) {

  IDL_FILE_STAT stat;
  struct OldRawIDLFp *irawfp;
  struct OldRawFp rawfp;
  int s=0;
  static IDL_VPTR vatme;
  double atme=0;

  IDL_VPTR outargv[8];
  static IDL_KW_PAR kw_pars[]={IDL_KW_FAST_SCAN,
			       {"ATME",IDL_TYP_UNDEF,1,
                                IDL_KW_OUT | IDL_KW_ZERO,0,
                                IDL_CHARA(vatme)},
				 {NULL}};


  IDL_LONG yr=0,mo=0,dy=0,hr=0,mt=0,sc=0;
  FILE *ffp=NULL,*ifp=NULL;

  IDL_KWCleanup(IDL_KW_MARK);
  IDL_KWGetParams(argc,argv,argk,kw_pars,outargv,1);

  IDL_ENSURE_STRUCTURE(outargv[0]);
  IDL_ENSURE_SCALAR(outargv[1]);
  IDL_ENSURE_SCALAR(outargv[2]);
  IDL_ENSURE_SCALAR(outargv[3]);
  IDL_ENSURE_SCALAR(outargv[4]);
  IDL_ENSURE_SCALAR(outargv[5]);
  IDL_ENSURE_SCALAR(outargv[6]);

  yr=IDL_LongScalar(outargv[1]);
  mo=IDL_LongScalar(outargv[2]);
  dy=IDL_LongScalar(outargv[3]);
  hr=IDL_LongScalar(outargv[4]);
  mt=IDL_LongScalar(outargv[5]);
  sc=IDL_LongScalar(outargv[6]);

  irawfp=(struct OldRawIDLFp *) outargv[0]->value.s.arr->data;
  
  s=IDL_FileEnsureStatus(IDL_MSG_RET,irawfp->rawunit,IDL_EFS_USER);

  if (s==FALSE) {
    s=-1;
    return (IDL_GettmpLong(s));
  }

  IDL_FileFlushUnit(irawfp->rawunit);
  IDL_FileStat(irawfp->rawunit,&stat);
 

  ffp=stat.fptr;
  
  if (ffp==NULL) {
    s=-1;
    return (IDL_GettmpLong(s));
  }
 
  if (irawfp->inxunit !=-1) {
    IDL_FileFlushUnit(irawfp->inxunit);
    IDL_FileStat(irawfp->inxunit,&stat);
    ifp=stat.fptr;
  }

  if (ifp !=NULL) fflush(ifp);

  rawfp.rawfp=fileno(ffp);
  if (ifp !=NULL) rawfp.inxfp=fileno(ifp);
  else rawfp.inxfp=-1;

  OldRawIDLToRawFp(irawfp,&rawfp);

  s=OldRawSeek(&rawfp,yr,mo,dy,hr,mt,sc,&atme);

  OldRawRawFpToIDL(&rawfp,irawfp);


  if (vatme) IDL_StoreScalar(vatme,IDL_TYP_DOUBLE,(IDL_ALLTYPES *) &atme);
 
  IDL_KWCleanup(IDL_KW_CLEAN);
  return (IDL_GettmpLong(s));
}

static IDL_VPTR IDLOldRawOpen(int argc,IDL_VPTR *argv) {

  IDL_VPTR vrawfp;  
  struct OldRawIDLFp *irawfp;
  struct OldRawFp *rawfp;
  void *s;
  int st;

  static IDL_STRUCT_TAG_DEF trawfp[]={
    {"RAWUNIT",0,(void *) IDL_TYP_LONG},
    {"INXUNIT",0,(void *) IDL_TYP_LONG},
    {"CTIME",0,(void *) IDL_TYP_DOUBLE},
    {"STIME",0,(void *) IDL_TYP_DOUBLE},
    {"FREC",0,(void *) IDL_TYP_LONG},
    {"RLEN",0,(void *) IDL_TYP_LONG},
    {"PTR",0,(void *) IDL_TYP_LONG},
    {"THR",0,(void *) IDL_TYP_LONG},
    {"MAJOR_REV",0,(void *) IDL_TYP_LONG},
    {"MINOR_REV",0,(void *) IDL_TYP_LONG},
    {"RAWREAD",0,(void *) IDL_TYP_LONG},
    {0}};

  static IDL_MEMINT ilDims[IDL_MAX_ARRAY_DIM];

  IDL_VARIABLE unit;
  IDL_VPTR fargv[2];
  IDL_FILE_STAT stat;


  int ffd=-1,ifd=-1;

  IDL_ENSURE_STRING(argv[0]);
  if (argc>1) IDL_ENSURE_STRING(argv[1]);
  
  s=IDL_MakeStruct("RAWFP",trawfp);
  ilDims[0]=1;
  irawfp=(struct OldRawIDLFp *) IDL_MakeTempStruct(s,1,ilDims,&vrawfp,TRUE);

  unit.type=IDL_TYP_LONG;
  unit.flags=0;

  fargv[0]=&unit;
  fargv[1]=argv[0];

  IDL_FileGetUnit(1,fargv);
  IDL_FileOpen(2,fargv,NULL,IDL_OPEN_R,IDL_F_STDIO,1,0);
  irawfp->rawunit=IDL_LongScalar(&unit);
  irawfp->inxunit=-1;

  st=IDL_FileEnsureStatus(IDL_MSG_RET,irawfp->rawunit,IDL_EFS_USER);

  if (st==FALSE) {
    st=0;
    IDL_Deltmp(vrawfp);
    return (IDL_GettmpLong(st));
  }
  
  if (argc>1) {
    fargv[1]=argv[1];
    IDL_FileGetUnit(1,fargv);
    IDL_FileOpen(2,fargv,NULL,IDL_OPEN_R,IDL_F_STDIO,1,0);
    irawfp->inxunit=IDL_LongScalar(&unit); 
    st=IDL_FileEnsureStatus(IDL_MSG_RET,irawfp->rawunit,IDL_EFS_USER);
    if (st==FALSE) { /* free unit for index but continue */
      IDL_FileFreeUnit(1,argv);
      irawfp->inxunit=-1;
    }
  }
 

  IDL_FileStat(irawfp->rawunit,&stat);
  ffd=fileno(stat.fptr);

  if (irawfp->inxunit !=-1) {
    IDL_FileStat(irawfp->inxunit,&stat);
    ifd=fileno(stat.fptr);
  }

  rawfp=OldRawOpenFd(ffd,ifd);
  OldRawRawFpToIDL(rawfp,irawfp);

  free(rawfp);
  
  return vrawfp;
}

static IDL_VPTR IDLOldRawClose(int argc,IDL_VPTR *argv) {
  int s=0;
  struct OldRawIDLFp *irawfp;
  IDL_VARIABLE unit;
  IDL_VPTR farg[1];

  IDL_ENSURE_STRUCTURE(argv[0]);
 
  irawfp=(struct OldRawIDLFp *) argv[0]->value.s.arr->data;

  unit.type=IDL_TYP_LONG;
  unit.flags=0;
  farg[0]=&unit;
  IDL_StoreScalar(farg[0],IDL_TYP_LONG,(IDL_ALLTYPES *) &irawfp->rawunit);

  IDL_FileFreeUnit(1,farg);

  if (irawfp->inxunit !=-1) {
    IDL_StoreScalar(farg[0],IDL_TYP_LONG,(IDL_ALLTYPES *) &irawfp->inxunit);
    IDL_FileFreeUnit(1,farg);
  }

  return (IDL_GettmpLong(s));

}

int IDL_Load(void) {

  static IDL_SYSFUN_DEF2 fnaddr[]={
    { {IDLOldRawRead},"OLDRAWREAD",3,3,0,0},
    { {IDLOldRawSeek},"OLDRAWSEEK",7,7,IDL_SYSFUN_DEF_F_KEYWORDS,0},
    { {IDLOldRawWrite},"OLDRAWWRITE",4,4,0,0},
    { {IDLOldRawOpen},"OLDRAWOPEN",1,2,0,0},
    { {IDLOldRawClose},"OLDRAWCLOSE",1,1,0,0},
    { {IDLOldRawWriteHeader},"OLDRAWWRITEHEADER",3,3,0,0},
  };


  if (!(msg_block = IDL_MessageDefineBlock("oldraw",
                    IDL_CARRAY_ELTS(msg_arr), msg_arr)))
    return IDL_FALSE;

  return IDL_SysRtnAdd(fnaddr,TRUE,IDL_CARRAY_ELTS(fnaddr));

}
