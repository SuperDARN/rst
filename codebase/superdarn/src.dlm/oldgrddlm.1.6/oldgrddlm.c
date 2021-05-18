/* oldgrddlm.c
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
#include "rfile.h"
#include "griddata.h"
#include "oldgridread.h"
#include "oldgridwrite.h"

#include "grdidl.h"
#include "oldgrdidl.h"


#define GRD_ERROR 0

static IDL_MSG_DEF msg_arr[] =
  {
    {  "GRD_ERROR",   "%NError: %s." }, 
  };

static IDL_MSG_BLOCK msg_block;


struct OldGridIDLInx *IDLMakeOldGridInx(int num,IDL_VPTR *vptr) {
  
  void *s=NULL;
  
  static IDL_MEMINT idim[1]={0};
 
  static IDL_STRUCT_TAG_DEF grdinx[]={
    {"ST_TIME",0,(void *) IDL_TYP_DOUBLE},
    {"ED_TIME",0,(void *) IDL_TYP_DOUBLE},
    {"OFFSET",0,(void *) IDL_TYP_LONG},
    {0}};

   s=IDL_MakeStruct("OLDGRIDINX",grdinx);  
   idim[0]=num;

   return (struct OldGridIDLInx *) IDL_MakeTempStruct(s,1,idim,vptr,TRUE);
}
  


static IDL_VPTR IDLOldGridWrite(int argc,IDL_VPTR *argv) {

  int s=0;

  IDL_LONG unit=0;
  IDL_FILE_STAT stat;

  struct GridData *grd=NULL;
  struct GridIDLPrm *iprm=NULL;
  struct GridIDLStVec *istvec=NULL;
  struct GridIDLGVec *igvec=NULL;
 
  FILE *fp;

  IDL_ENSURE_SCALAR(argv[0]);
  IDL_ENSURE_STRUCTURE(argv[1]);
  IDL_ENSURE_ARRAY(argv[2]);
  IDL_ENSURE_ARRAY(argv[3]);
  IDL_EXCLUDE_EXPR(argv[1]);
  IDL_EXCLUDE_EXPR(argv[2]);
  IDL_EXCLUDE_EXPR(argv[3]);

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

  iprm=(struct GridIDLPrm *) argv[1]->value.s.arr->data;
  if (argv[2]->type==IDL_TYP_STRUCT) 
     istvec=(struct GridIDLStVec *) argv[2]->value.s.arr->data;
  if (argv[3]->type==IDL_TYP_STRUCT) 
     igvec=(struct GridIDLGVec *) argv[3]->value.s.arr->data;


  grd=GridMake(); 

  IDLCopyGridPrmFromIDL(iprm,grd);
  if (istvec !=NULL)  IDLCopyGridStVecFromIDL(istvec,iprm->stnum,
                        argv[2]->value.s.arr->elt_len,grd);
  else grd->stnum=0;

  if (igvec !=NULL) IDLCopyGridGVecFromIDL(igvec,iprm->vcnum,
       argv[3]->value.s.arr->elt_len,grd);
  else grd->vcnum=0;

  s=OldGridFwrite(fp,grd);

  GridFree(grd);

  return (IDL_GettmpLong(s));


}

static IDL_VPTR IDLOldGridRead(int argc,IDL_VPTR *argv) {

  int s=0;


  IDL_VPTR vprm=NULL,vstvec=NULL,vgvec=NULL;

  IDL_LONG unit=0;
  IDL_FILE_STAT stat;


  struct GridData *grd=NULL;
  struct GridIDLPrm *iprm=NULL;
  struct GridIDLStVec *istvec=NULL;
  struct GridIDLGVec *igvec=NULL; 

  FILE *fp=NULL;

  IDL_ENSURE_SCALAR(argv[0]);
  IDL_EXCLUDE_EXPR(argv[1]);
  IDL_EXCLUDE_EXPR(argv[2]);
  IDL_EXCLUDE_EXPR(argv[3]);


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
 

  grd=GridMake(); 

  s=OldGridFread(fp,grd);

  if (s==-1) {
    GridFree(grd);
    return (IDL_GettmpLong(s));
  }

  iprm=IDLMakeGridPrm(&vprm);
  if (grd->stnum !=0) istvec=IDLMakeGridStVec(grd->stnum,&vstvec);
  if (grd->vcnum !=0) igvec=IDLMakeGridGVec(grd->vcnum,&vgvec);

  /* copy the data here */
   
  IDLCopyGridPrmToIDL(grd,iprm);
  if (istvec !=NULL) IDLCopyGridStVecToIDL(grd,grd->stnum,
                                    vstvec->value.s.arr->elt_len,istvec);
  if (igvec !=NULL) IDLCopyGridGVecToIDL(grd,grd->vcnum,
                                    vgvec->value.s.arr->elt_len,igvec);


  
  GridFree(grd);

  IDL_VarCopy(vprm,argv[1]);  
  if (vstvec !=NULL) IDL_VarCopy(vstvec,argv[2]);
  if (vgvec !=NULL) IDL_VarCopy(vgvec,argv[3]);

  return (IDL_GettmpLong(s));

}




static IDL_VPTR IDLOldGridLoadInx(int argc,IDL_VPTR *argv) {

  int s=0,n;

  IDL_VPTR vinx=NULL;

  IDL_LONG unit=0;
  IDL_FILE_STAT stat;

  FILE *fp=NULL;

  struct OldGridIDLInx *ifptr=NULL;
  struct RfileIndex *finx=NULL;

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
 
  finx=RfileLoadIndex(fp);

  if (finx==NULL) {
    s=-1;
    return (IDL_GettmpLong(s));
  }

  ifptr=IDLMakeOldGridInx(finx->num,&vinx);
  
  for (n=0;n<finx->num;n++) {
    ifptr[n].st_time=finx->time.start[n];
    ifptr[n].ed_time=finx->time.end[n];
    ifptr[n].offset=finx->offset[n];
  }
  
  free(finx->time.start);
  free(finx->time.end);
  free(finx->offset);
  free(finx);


  IDL_VarCopy(vinx,argv[1]); 
   
  return (IDL_GettmpLong(s));
}


static IDL_VPTR IDLOldGridSeek(int argc,IDL_VPTR *argv,char *argk) {

  struct OldGridIDLInx *ifptr;

  int s=0,n;
  struct RfileIndex *finx=NULL;
  static IDL_VPTR vatme;
  double atme=0;

  int outargc=0;
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
    finx=malloc(sizeof(struct RfileIndex));
    if (finx==NULL) {
      s=-1;
      return (IDL_GettmpLong(s));
    }
    finx->num=outargv[7]->value.s.arr->n_elts;
    finx->time.start=malloc(sizeof(double)*finx->num);
    if (finx->time.start==NULL) {
      s=-1;
      free(finx);
      return (IDL_GettmpLong(s));
    } 
    finx->time.end=malloc(sizeof(double)*finx->num);
    if (finx->time.end==NULL) {
      s=-1;
      free(finx->time.start);
      free(finx);
      return (IDL_GettmpLong(s));
    } 

    finx->offset=malloc(sizeof(int)*finx->num);
    if (finx->offset==NULL) {
      s=-1;
      free(finx->time.start);
      free(finx->time.end);
      free(finx);
      return (IDL_GettmpLong(s));
    } 
    for (n=0;n<finx->num;n++) {
      ifptr=(struct OldGridIDLInx *) (outargv[7]->value.s.arr->data+
                                    n*outargv[7]->value.s.arr->elt_len);
      finx->time.start[n]=ifptr->st_time;
      finx->time.end[n]=ifptr->ed_time;
      finx->offset[n]=ifptr->offset;
    }
    finx->st_time=finx->time.start[0];
    finx->ed_time=finx->time.end[finx->num-1];

  }


  s=OldGridFseek(fp,yr,mo,dy,hr,mt,sc,finx,&atme);

  if (vatme) IDL_StoreScalar(vatme,IDL_TYP_DOUBLE,(IDL_ALLTYPES *) &atme);
 
    

  if (finx !=NULL) {
    free(finx->time.start);
    free(finx->time.end);
    free(finx->offset);
    free(finx);
  }

  IDL_KWCleanup(IDL_KW_CLEAN);
  return (IDL_GettmpLong(s));


}



static IDL_VPTR IDLOldGridOpen(int argc,IDL_VPTR *argv,char *argk) {
  
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


static IDL_VPTR IDLOldGridClose(int argc,IDL_VPTR *argv) {
  int s=0;
  IDL_ENSURE_SCALAR(argv[0]);
  IDL_FileFreeUnit(1,argv);
  return (IDL_GettmpLong(s));

}

int IDL_Load(void) {

  static IDL_SYSFUN_DEF2 fnaddr[]={
    { {IDLOldGridRead},"OLDGRIDREAD",4,4,0,0},
    { {IDLOldGridWrite},"OLDGRIDWRITE",4,4,0,0},
    { {IDLOldGridLoadInx},"OLDGRIDLOADINX",2,2,0,0},
    { {IDLOldGridSeek},"OLDGRIDSEEK",7,8,IDL_SYSFUN_DEF_F_KEYWORDS,0},
    { {IDLOldGridOpen},"OLDGRIDOPEN",1,1,IDL_SYSFUN_DEF_F_KEYWORDS,0},
    { {IDLOldGridClose},"OLDGRIDCLOSE",1,1,0,0},
  };


  if (!(msg_block = IDL_MessageDefineBlock("grd",
                    IDL_CARRAY_ELTS(msg_arr), msg_arr)))
    return IDL_FALSE;

  return IDL_SysRtnAdd(fnaddr,TRUE,IDL_CARRAY_ELTS(fnaddr));

}
