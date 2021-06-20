/* fitdlm.c
   ========== 
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
#include "fitdata.h"
#include "oldfitread.h"
#include "oldfitwrite.h"

#include "rprmidl.h"
#include "fitidl.h"

#include "oldfitidl.h"

#define OLDFIT_ERROR 0

static IDL_MSG_DEF msg_arr[] = {
    {  "OLDFIT_ERROR",   "%NError: %s." }, 
  };

static IDL_MSG_BLOCK msg_block;


void OldFitIDLToFitFp(struct OldFitIDLFp *ifitfp,struct OldFitFp *fitfp) {

  fitfp->fit_recl=ifitfp->fit_recl;
  fitfp->inx_recl=ifitfp->inx_recl;
  fitfp->blen=ifitfp->blen;
  fitfp->inx_srec=ifitfp->inx_srec;
  fitfp->inx_erec=ifitfp->inx_erec;
  fitfp->ctime=ifitfp->ctime;
  fitfp->stime=ifitfp->stime;
  fitfp->etime=ifitfp->etime;
  fitfp->time=ifitfp->time;
  strncpy(fitfp->header,ifitfp->header,80);
  strncpy(fitfp->date,ifitfp->date,32);
  strncpy(fitfp->extra,ifitfp->extra,256);
  fitfp->major_rev=ifitfp->major_rev;
  fitfp->minor_rev=ifitfp->minor_rev;
  fitfp->fitread=ifitfp->fitread;

}

void OldFitFitFpToIDL(struct OldFitFp *fitfp,struct OldFitIDLFp *ifitfp) {

  ifitfp->fit_recl=fitfp->fit_recl;
  ifitfp->inx_recl=fitfp->inx_recl;
  ifitfp->blen=fitfp->blen;
  ifitfp->inx_srec=fitfp->inx_srec;
  ifitfp->inx_erec=fitfp->inx_erec;
  ifitfp->ctime=fitfp->ctime;
  ifitfp->stime=fitfp->stime;
  ifitfp->etime=fitfp->etime;
  ifitfp->time=fitfp->time;
  strncpy(ifitfp->header,fitfp->header,80);
  strncpy(ifitfp->date,fitfp->date,32);
  strncpy(ifitfp->extra,fitfp->extra,256);
  ifitfp->major_rev=fitfp->major_rev;
  ifitfp->minor_rev=fitfp->minor_rev;
  ifitfp->fitread=fitfp->fitread;
}



static IDL_VPTR IDLOldFitInxWriteHeader(int argc,IDL_VPTR *argv) {
  int s=0;
  IDL_LONG unit=0;
  IDL_FILE_STAT stat;

  struct RadarIDLParm *iprm=NULL;
  struct RadarParm *prm=NULL;

  FILE *fp;

  IDL_ENSURE_SCALAR(argv[0]);
  IDL_ENSURE_STRUCTURE(argv[1]);
  IDL_EXCLUDE_EXPR(argv[1]);

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

  prm=RadarParmMake();
  IDLCopyRadarParmFromIDL(iprm,prm);

  s=OldFitInxHeaderFwrite(fp,prm);

  RadarParmFree(prm);

  return (IDL_GettmpLong(s));
}




static IDL_VPTR IDLOldFitInxWrite(int argc,IDL_VPTR *argv) {
  int s=0;
  IDL_LONG unit=0,drec=0,dnum=0;
  IDL_FILE_STAT stat;

  struct RadarIDLParm *iprm=NULL;
  struct RadarParm *prm=NULL;

  FILE *fp;

  IDL_ENSURE_SCALAR(argv[0]);
  IDL_ENSURE_SCALAR(argv[1]);
  IDL_ENSURE_SCALAR(argv[2]);
  IDL_ENSURE_STRUCTURE(argv[3]);
  IDL_EXCLUDE_EXPR(argv[1]);
  IDL_EXCLUDE_EXPR(argv[2]);
  IDL_EXCLUDE_EXPR(argv[3]);

  unit=IDL_LongScalar(argv[0]);
  drec=IDL_LongScalar(argv[1]);
  dnum=IDL_LongScalar(argv[2]);

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

  iprm=(struct RadarIDLParm *) argv[3]->value.s.arr->data;

  prm=RadarParmMake();
  IDLCopyRadarParmFromIDL(iprm,prm);

  s=OldFitInxFwrite(fp,drec,dnum,prm);

  RadarParmFree(prm);

  return (IDL_GettmpLong(s));
}


static IDL_VPTR IDLOldFitInxOpen(int argc,IDL_VPTR *argv) {

  IDL_VARIABLE unit;
  IDL_VPTR fargv[2];

  IDL_ENSURE_STRING(argv[0]);

  unit.type=IDL_TYP_LONG;
  unit.flags=0;

  fargv[0]=&unit;
  fargv[1]=argv[0];

  IDL_FileGetUnit(1,fargv);
  IDL_FileOpen(2,fargv,NULL,IDL_OPEN_W,IDL_F_STDIO,1,0);
  return IDL_GettmpLong(IDL_LongScalar(&unit));
}

static IDL_VPTR IDLOldFitInxClose(int argc,IDL_VPTR *argv) {
  int s=0;
  IDL_LONG unit=0,irec=0;
  IDL_FILE_STAT stat;

  struct RadarIDLParm *iprm=NULL;
  struct RadarParm *prm=NULL;

  FILE *fp;

  IDL_ENSURE_SCALAR(argv[0]);
  IDL_ENSURE_STRUCTURE(argv[1]);
  IDL_ENSURE_SCALAR(argv[2]);

  IDL_EXCLUDE_EXPR(argv[1]);

  unit=IDL_LongScalar(argv[0]);
  irec=IDL_LongScalar(argv[2]);

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

  prm=RadarParmMake();
  IDLCopyRadarParmFromIDL(iprm,prm);

  s=OldFitInxFclose(fp,prm,irec);
  

  RadarParmFree(prm);
  IDL_FileFreeUnit(1,argv);



  return (IDL_GettmpLong(s));
}





static IDL_VPTR IDLOldFitWriteHeader(int argc,IDL_VPTR *argv) {
  int s=0;
  IDL_LONG unit=0;
  IDL_FILE_STAT stat;

  struct RadarIDLParm *iprm=NULL;
  struct FitIDLData *ifit=NULL;
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
  ifit=(struct FitIDLData *) argv[2]->value.s.arr->data;

  sprintf(vstring,"%d.%.3d",(int) ifit->revision.major,
	                    (int) ifit->revision.minor);
  s=OldFitHeaderFwrite(fp,"IDL output","fitwrite",vstring);

  return (IDL_GettmpLong(s));
}



static IDL_VPTR IDLOldFitWrite(int argc,IDL_VPTR *argv) {
  int s=0;
  IDL_LONG unit=0;
  IDL_FILE_STAT stat;

  struct RadarIDLParm *iprm=NULL;
  struct FitIDLData *ifit=NULL;

  struct RadarParm *prm=NULL;
  struct FitData *fit=NULL;

  FILE *fp;

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
  ifit=(struct FitIDLData *) argv[2]->value.s.arr->data;

  prm=RadarParmMake();
  fit=FitMake();

  IDLCopyRadarParmFromIDL(iprm,prm);
  IDLCopyFitDataFromIDL(prm->nrang,prm->xcf,ifit,fit);

  s=OldFitFwrite(fp,prm,fit,NULL);

  RadarParmFree(prm);
  FitFree(fit);



  return (IDL_GettmpLong(s));
}

static IDL_VPTR IDLOldFitRead(int argc,IDL_VPTR *argv) {

  int s=0;

  IDL_VPTR vprm=NULL,vfit=NULL;

  IDL_FILE_STAT stat;

  struct OldFitIDLFp *ifitfp;
  struct OldFitFp fitfp;

  struct RadarIDLParm *iprm=NULL;
  struct FitIDLData *ifit=NULL;

  struct RadarParm *prm=NULL;
  struct FitData *fit=NULL;

  FILE *ffp=NULL,*ifp=NULL;

  IDL_ENSURE_STRUCTURE(argv[0]);
  IDL_EXCLUDE_EXPR(argv[1]);
  IDL_EXCLUDE_EXPR(argv[2]);

  
  ifitfp=(struct OldFitIDLFp *) argv[0]->value.s.arr->data;
  
  s=IDL_FileEnsureStatus(IDL_MSG_RET,ifitfp->fitunit,IDL_EFS_USER);

  if (s==FALSE) {
    s=-1;
    return (IDL_GettmpLong(s));
  }

  IDL_FileFlushUnit(ifitfp->fitunit);
  IDL_FileStat(ifitfp->fitunit,&stat);
 

  ffp=stat.fptr;
  
  if (ffp==NULL) {
    s=-1;
    return (IDL_GettmpLong(s));
  }
 
  if (ifitfp->inxunit !=-1) {
    IDL_FileFlushUnit(ifitfp->inxunit);
    IDL_FileStat(ifitfp->inxunit,&stat);
    ifp=stat.fptr;
  }

  if (ifp !=NULL) fflush(ifp);

  fitfp.fitfp=fileno(ffp);
  if (ifp !=NULL) fitfp.inxfp=fileno(ifp);
  else fitfp.inxfp=-1;

  OldFitIDLToFitFp(ifitfp,&fitfp);

  prm=RadarParmMake();
  fit=FitMake(); 

  s=OldFitRead(&fitfp,prm,fit);

  OldFitFitFpToIDL(&fitfp,ifitfp);
 
  if (s==-1) {
    RadarParmFree(prm);
    FitFree(fit);
    return (IDL_GettmpLong(s));
  }

  iprm=IDLMakeRadarParm(&vprm);
  ifit=IDLMakeFitData(&vfit);

  /* copy data here */



  IDLCopyRadarParmToIDL(prm,iprm);
  IDLCopyFitDataToIDL(prm->nrang,prm->xcf,fit,ifit);

  RadarParmFree(prm);
  FitFree(fit);

  IDL_VarCopy(vprm,argv[1]);  
  IDL_VarCopy(vfit,argv[2]);
    
  return (IDL_GettmpLong(s));

}


static IDL_VPTR IDLOldFitSeek(int argc,IDL_VPTR *argv,char *argk) {

  IDL_FILE_STAT stat;
  struct OldFitIDLFp *ifitfp;
  struct OldFitFp fitfp;
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

  ifitfp=(struct OldFitIDLFp *) outargv[0]->value.s.arr->data;
  
  s=IDL_FileEnsureStatus(IDL_MSG_RET,ifitfp->fitunit,IDL_EFS_USER);

  if (s==FALSE) {
    s=-1;
    return (IDL_GettmpLong(s));
  }

  IDL_FileFlushUnit(ifitfp->fitunit);
  IDL_FileStat(ifitfp->fitunit,&stat);
 

  ffp=stat.fptr;
  
  if (ffp==NULL) {
    s=-1;
    return (IDL_GettmpLong(s));
  }
 
  if (ifitfp->inxunit !=-1) {
    IDL_FileFlushUnit(ifitfp->inxunit);
    IDL_FileStat(ifitfp->inxunit,&stat);
    ifp=stat.fptr;
  }

  if (ifp !=NULL) fflush(ifp);

  fitfp.fitfp=fileno(ffp);
  if (ifp !=NULL) fitfp.inxfp=fileno(ifp);
  else fitfp.inxfp=-1;

  OldFitIDLToFitFp(ifitfp,&fitfp);
 
  s=OldFitSeek(&fitfp,yr,mo,dy,hr,mt,sc,&atme);

  OldFitFitFpToIDL(&fitfp,ifitfp);


  if (vatme) IDL_StoreScalar(vatme,IDL_TYP_DOUBLE,(IDL_ALLTYPES *) &atme);
 
  IDL_KWCleanup(IDL_KW_CLEAN);
  return (IDL_GettmpLong(s));
}

static IDL_VPTR IDLOldFitOpen(int argc,IDL_VPTR *argv) {

  IDL_VPTR vfitfp;  
  struct OldFitIDLFp *ifitfp;
  struct OldFitFp *fitfp;
  void *s;
  int st;

  static IDL_MEMINT hdim[]={1,80};
  static IDL_MEMINT ddim[]={1,32};
  static IDL_MEMINT edim[]={1,256};

  static IDL_STRUCT_TAG_DEF tfitfp[]={
    {"FITUNIT",0,(void *) IDL_TYP_LONG},
    {"INXUNIT",0,(void *) IDL_TYP_LONG},
    {"FIT_RECL",0,(void *) IDL_TYP_LONG},
    {"INX_RECL",0,(void *) IDL_TYP_LONG},
    {"BLEN",0,(void *) IDL_TYP_LONG},
    {"INX_SREC",0,(void *) IDL_TYP_LONG},
    {"INX_EREC",0,(void *) IDL_TYP_LONG},
    {"CTIME",0,(void *) IDL_TYP_DOUBLE},
    {"STIME",0,(void *) IDL_TYP_DOUBLE},
    {"ETIME",0,(void *) IDL_TYP_DOUBLE},
    {"TIME",0,(void *) IDL_TYP_LONG},
    {"HEADER",hdim,(void *) IDL_TYP_BYTE},
    {"DATE",ddim,(void *) IDL_TYP_BYTE},
    {"EXTRA",edim,(void *) IDL_TYP_BYTE},
    {"MAJOR_REV",0,(void *) IDL_TYP_BYTE},
    {"MINOR_REV",0,(void *) IDL_TYP_BYTE},
    {"FITREAD",0,(void *) IDL_TYP_LONG},
    {"BNUM",0,(void *) IDL_TYP_LONG},
    {0}};

  static IDL_MEMINT ilDims[IDL_MAX_ARRAY_DIM];

  IDL_VARIABLE unit;
  IDL_VPTR fargv[2];
  IDL_FILE_STAT stat;


  int ffd=-1,ifd=-1;

  IDL_ENSURE_STRING(argv[0]);
  if (argc>1) IDL_ENSURE_STRING(argv[1]);
  
  s=IDL_MakeStruct("FITFP",tfitfp);
  ilDims[0]=1;
  ifitfp=(struct OldFitIDLFp *) IDL_MakeTempStruct(s,1,ilDims,&vfitfp,TRUE);

  unit.type=IDL_TYP_LONG;
  unit.flags=0;

  fargv[0]=&unit;
  fargv[1]=argv[0];

  IDL_FileGetUnit(1,fargv);
  IDL_FileOpen(2,fargv,NULL,IDL_OPEN_R,IDL_F_STDIO,1,0);
  ifitfp->fitunit=IDL_LongScalar(&unit);
  ifitfp->inxunit=-1;

  st=IDL_FileEnsureStatus(IDL_MSG_RET,ifitfp->fitunit,IDL_EFS_USER);

  if (st==FALSE) {
    st=0;
    IDL_Deltmp(vfitfp);
    return (IDL_GettmpLong(st));
  }
  
  if (argc>1) {
    fargv[1]=argv[1];
    IDL_FileGetUnit(1,fargv);
    IDL_FileOpen(2,fargv,NULL,IDL_OPEN_R,IDL_F_STDIO,1,0);
    ifitfp->inxunit=IDL_LongScalar(&unit); 
    st=IDL_FileEnsureStatus(IDL_MSG_RET,ifitfp->fitunit,IDL_EFS_USER);
    if (st==FALSE) { /* free unit for index but continue */
      IDL_FileFreeUnit(1,argv);
      ifitfp->inxunit=-1;
    }
  }
 

  IDL_FileStat(ifitfp->fitunit,&stat);
  ffd=fileno(stat.fptr);

  if (ifitfp->inxunit !=-1) {
    IDL_FileStat(ifitfp->inxunit,&stat);
    ifd=fileno(stat.fptr);
  }

  fitfp=OldFitOpenFd(ffd,ifd);
  OldFitFitFpToIDL(fitfp,ifitfp);

  free(fitfp);
  
  return vfitfp;
}

static IDL_VPTR IDLOldFitClose(int argc,IDL_VPTR *argv) {
  int s=0;
  struct OldFitIDLFp *ifitfp;
  IDL_VARIABLE unit;
  IDL_VPTR farg[1];

  IDL_ENSURE_STRUCTURE(argv[0]);
 
  ifitfp=(struct OldFitIDLFp *) argv[0]->value.s.arr->data;

  unit.type=IDL_TYP_LONG;
  unit.flags=0;
  farg[0]=&unit;
  IDL_StoreScalar(farg[0],IDL_TYP_LONG,(IDL_ALLTYPES *) &ifitfp->fitunit);

  IDL_FileFreeUnit(1,farg);

  if (ifitfp->inxunit !=-1) {
    IDL_StoreScalar(farg[0],IDL_TYP_LONG,(IDL_ALLTYPES *) &ifitfp->inxunit);
    IDL_FileFreeUnit(1,farg);
  }

  return (IDL_GettmpLong(s));

}

int IDL_Load(void) {

  static IDL_SYSFUN_DEF2 fnaddr[]={
    { {IDLOldFitRead},"OLDFITREAD",3,3,0,0},
    { {IDLOldFitSeek},"OLDFITSEEK",7,7,IDL_SYSFUN_DEF_F_KEYWORDS,0},
    { {IDLOldFitWrite},"OLDFITWRITE",3,3,0,0},
    { {IDLOldFitOpen},"OLDFITOPEN",1,2,0,0},
    { {IDLOldFitClose},"OLDFITCLOSE",1,1,0,0},
    { {IDLOldFitWriteHeader},"OLDFITWRITEHEADER",3,3,0,0},
    { {IDLOldFitInxWrite},"OLDFITINXWRITE",4,4,0,0},
    { {IDLOldFitInxWriteHeader},"OLDFITINXWRITEHEADER",2,2,0,0},
    { {IDLOldFitInxOpen},"OLDFITINXOPEN",1,1,0,0},
    { {IDLOldFitInxClose},"OLDFITINXCLOSE",3,3,0,0},
  };


  if (!(msg_block = IDL_MessageDefineBlock("oldfit",
                    IDL_CARRAY_ELTS(msg_arr), msg_arr)))
    return IDL_FALSE;

  return IDL_SysRtnAdd(fnaddr,TRUE,IDL_CARRAY_ELTS(fnaddr));

}
