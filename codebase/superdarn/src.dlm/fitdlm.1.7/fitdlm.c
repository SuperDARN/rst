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
#include "fitread.h"
#include "fitwrite.h"
#include "fitindex.h"
#include "fitseek.h"

#include "rprmidl.h"
#include "fitidl.h"

#define FIT_ERROR 0



static IDL_MSG_DEF msg_arr[] =
  {
    {  "FIT_ERROR",   "%NError: %s." }, 
  };

static IDL_MSG_BLOCK msg_block;


static IDL_VPTR IDLFitWrite(int argc,IDL_VPTR *argv) {

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

  s=FitFwrite(fp,prm,fit);

  RadarParmFree(prm);
  FitFree(fit);

  return (IDL_GettmpLong(s));


}

static IDL_VPTR IDLFitRead(int argc,IDL_VPTR *argv) {

  int s=0;


  IDL_VPTR vprm=NULL,vfit=NULL;;

  IDL_LONG unit=0;
  IDL_FILE_STAT stat;

  struct RadarIDLParm *iprm=NULL;
  struct FitIDLData *ifit=NULL;

  struct RadarParm *prm=NULL;
  struct FitData *fit=NULL;

  FILE *fp=NULL;

  IDL_ENSURE_SCALAR(argv[0]);
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
 
  prm=RadarParmMake();
  fit=FitMake(); 

  s=FitFread(fp,prm,fit);

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




static IDL_VPTR IDLFitLoadInx(int argc,IDL_VPTR *argv) {

  int s=0,n;

  IDL_VPTR vinx=NULL;

  IDL_LONG unit=0;
  IDL_FILE_STAT stat;

  FILE *fp=NULL;

  struct FitIDLInx *ifptr=NULL;
  struct FitIndex *finx=NULL;

  IDL_ENSURE_SCALAR(argv[0]);
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
 
  finx=FitIndexFload(fp);

  if (finx==NULL) {
    s=-1;
    return (IDL_GettmpLong(s));
  }

  ifptr=IDLMakeFitInx(finx->num,&vinx);
  
  for (n=0;n<finx->num;n++) {
    ifptr[n].time=finx->tme[n];
    ifptr[n].offset=finx->inx[n];
  }
  

  FitIndexFree(finx);
  
  IDL_VarCopy(vinx,argv[1]); 
   
  return (IDL_GettmpLong(s));
}


static IDL_VPTR IDLFitSeek(int argc,IDL_VPTR *argv,char *argk) {

  struct FitIDLInx *ifptr;

  int s=0,n;
  struct FitIndex *finx=NULL;
  static IDL_VPTR vatme;
  double atme=0;

  int outargc;
  IDL_VPTR outargv[8];
  static IDL_KW_PAR kw_pars[]={IDL_KW_FAST_SCAN,
			       {"ATME",IDL_TYP_UNDEF,1,
                                IDL_KW_OUT | IDL_KW_ZERO,0,
                                IDL_CHARA(vatme)},
				 {NULL}};


  IDL_LONG unit=0,yr=0,mo=0,dy=0,hr=0,mt=0,sc=0;
  IDL_FILE_STAT stat;

  FILE *fp=NULL;

  IDL_KWCleanup(IDL_KW_MARK);
  outargc=IDL_KWGetParams(argc,argv,argk,kw_pars,outargv,1);


  IDL_ENSURE_SCALAR(outargv[0]);
  IDL_ENSURE_SCALAR(outargv[1]);
  IDL_ENSURE_SCALAR(outargv[2]);
  IDL_ENSURE_SCALAR(outargv[3]);
  IDL_ENSURE_SCALAR(outargv[4]);
  IDL_ENSURE_SCALAR(outargv[5]);
  IDL_ENSURE_SCALAR(outargv[6]);

  if (outargc>7) IDL_ENSURE_ARRAY(outargv[7]);

  unit=IDL_LongScalar(outargv[0]);

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

  yr=IDL_LongScalar(outargv[1]);
  mo=IDL_LongScalar(outargv[2]);
  dy=IDL_LongScalar(outargv[3]);
  hr=IDL_LongScalar(outargv[4]);
  mt=IDL_LongScalar(outargv[5]);
  sc=IDL_LongScalar(outargv[6]);

  /* test for existence of index */



  if (outargc>7) {
    /* decode index here */
    finx=malloc(sizeof(struct FitIndex));
    if (finx==NULL) {
      s=-1;
      return (IDL_GettmpLong(s));
    }
    finx->num=outargv[7]->value.s.arr->n_elts;
    finx->tme=malloc(sizeof(double)*finx->num);
    if (finx->tme==NULL) {
      s=-1;
      free(finx);
      return (IDL_GettmpLong(s));
    } 
    finx->inx=malloc(sizeof(int)*finx->num);
    if (finx->inx==NULL) {
      s=-1;
      free(finx->tme);
      free(finx);
      return (IDL_GettmpLong(s));
    } 
    for (n=0;n<finx->num;n++) {
      ifptr=(struct FitIDLInx *) (outargv[7]->value.s.arr->data+
                                  n*outargv[7]->value.s.arr->elt_len);
      finx->tme[n]=ifptr->time;
      finx->inx[n]=ifptr->offset;
    }
  }
  s=FitFseek(fp,yr,mo,dy,hr,mt,sc,&atme,finx);

  if (vatme) IDL_StoreScalar(vatme,IDL_TYP_DOUBLE,(IDL_ALLTYPES *) &atme);
 
    

  if (finx !=NULL) FitIndexFree(finx);

  IDL_KWCleanup(IDL_KW_CLEAN);
  return (IDL_GettmpLong(s));


}



static IDL_VPTR IDLFitOpen(int argc,IDL_VPTR *argv,char *argk) {
  
  IDL_VARIABLE unit;
  IDL_VPTR fargv[2]; 

  
  IDL_VPTR outargv[1];
  static IDL_LONG iread;
  static IDL_LONG iwrite;
  static IDL_LONG iupdate;
  int access=0;

  static IDL_KW_PAR kw_pars[]={IDL_KW_FAST_SCAN,
                               {"READ",IDL_TYP_LONG,1,
                                IDL_KW_ZERO,0,
                                IDL_CHARA(iread)},
                               {"UPDATE",IDL_TYP_LONG,1,
                                IDL_KW_ZERO,0,
                                IDL_CHARA(iupdate)},
                               {"WRITE",IDL_TYP_LONG,1,
                                IDL_KW_ZERO,0,
                                IDL_CHARA(iwrite)},
                                 {NULL}};

  IDL_KWCleanup(IDL_KW_MARK);
  IDL_KWGetParams(argc,argv,argk,kw_pars,outargv,1);

  IDL_ENSURE_STRING(outargv[0]);

  unit.type=IDL_TYP_LONG;
  unit.flags=0;

  fargv[0]=&unit;
  fargv[1]=outargv[0];
  
  IDL_FileGetUnit(1,fargv);  

  if (iread !=0) access=access | IDL_OPEN_R;
  if (iwrite !=0) access=access | IDL_OPEN_W;
  if (iupdate !=0) access=access | IDL_OPEN_APND;

  if (access==0) access=IDL_OPEN_R;

  IDL_FileOpen(2,fargv,NULL,access,IDL_F_STDIO,1,0);

  IDL_KWCleanup(IDL_KW_CLEAN);
  
  return IDL_GettmpLong(IDL_LongScalar(&unit));
}


static IDL_VPTR IDLFitClose(int argc,IDL_VPTR *argv) {
  int s=0;
  IDL_ENSURE_SCALAR(argv[0]);
  IDL_FileFreeUnit(1,argv);
  return (IDL_GettmpLong(s));

}

int IDL_Load(void) {

  static IDL_SYSFUN_DEF2 fnaddr[]={
    { {IDLFitRead},"FITREAD",3,3,0,0},
    { {IDLFitWrite},"FITWRITE",3,3,0,0},
    { {IDLFitLoadInx},"FITLOADINX",2,2,0,0},
    { {IDLFitSeek},"FITSEEK",7,8,IDL_SYSFUN_DEF_F_KEYWORDS,0},
    { {IDLFitOpen},"FITOPEN",1,1,IDL_SYSFUN_DEF_F_KEYWORDS,0},
    { {IDLFitClose},"FITCLOSE",1,1,0,0},
  };


  if (!(msg_block = IDL_MessageDefineBlock("fit",
                    IDL_CARRAY_ELTS(msg_arr), msg_arr)))
    return IDL_FALSE;

  return IDL_SysRtnAdd(fnaddr,TRUE,IDL_CARRAY_ELTS(fnaddr));

}
