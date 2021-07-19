/* cnvmapdlm.c
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
#include "griddata.h"
#include "cnvmap.h"
#include "cnvmapread.h"
#include "cnvmapwrite.h"
#include "cnvmapindex.h"
#include "cnvmapseek.h"

#include "grdidl.h"
#include "cnvmapidl.h"

#define CNVMAP_ERROR 0



static IDL_MSG_DEF msg_arr[] =
  {
    {  "CNVMAP_ERROR",   "%NError: %s." }, 
  };

static IDL_MSG_BLOCK msg_block;


static IDL_VPTR IDLCnvMapWrite(int argc,IDL_VPTR *argv) {

  int s=0;

  IDL_LONG unit=0;
  IDL_FILE_STAT stat;

  struct CnvMapData *map=NULL;
  struct GridData *grd=NULL;

  struct CnvMapIDLPrm *iprm=NULL;
  struct GridIDLStVec *istvec=NULL;
  struct GridIDLGVec *igvec=NULL;
  struct GridIDLGVec *imvec=NULL;
  struct CnvMapIDLBnd *ibnd=NULL;
  double *icoef=NULL; 

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

  iprm=(struct CnvMapIDLPrm *) argv[1]->value.s.arr->data;
  if (argv[2]->type==IDL_TYP_STRUCT) 
     istvec=(struct GridIDLStVec *) argv[2]->value.s.arr->data;
  if (argv[3]->type==IDL_TYP_STRUCT) 
     igvec=(struct GridIDLGVec *) argv[3]->value.s.arr->data;
  if (argv[4]->type==IDL_TYP_STRUCT)
     imvec=(struct GridIDLGVec *) argv[4]->value.s.arr->data;
  if (argv[5]->type==IDL_TYP_DOUBLE) 
     icoef=(double *) argv[5]->value.s.arr->data;
  if (argv[6]->type==IDL_TYP_STRUCT) 
     ibnd=(struct CnvMapIDLBnd *) argv[6]->value.s.arr->data;

  map=CnvMapMake(); 
  grd=GridMake();

  IDLCopyCnvMapPrmFromIDL(iprm,map,grd);
  if (istvec !=NULL)  IDLCopyGridStVecFromIDL(istvec,iprm->stnum,
                        argv[2]->value.s.arr->elt_len,grd);
  else grd->stnum=0;

  if (igvec !=NULL) IDLCopyGridGVecFromIDL(igvec,iprm->vcnum,
       argv[3]->value.s.arr->elt_len,grd);
  else grd->vcnum=0;

  if (imvec !=NULL) IDLCopyCnvMapGVecFromIDL(imvec,
                        iprm->modnum,argv[4]->value.s.arr->elt_len,map);
  else map->num_model=0;

  if (ibnd !=NULL) IDLCopyCnvMapBndFromIDL(ibnd,
                        iprm->bndnum,argv[6]->value.s.arr->elt_len,map);
  else map->num_bnd=0;

  if (icoef !=NULL) {
    int n=0;
    map->coef=malloc(sizeof(double)*map->num_coef*4);
    for (n=0;n<map->num_coef;n++) {
      map->coef[4*n]=icoef[n];
      map->coef[4*n+1]= icoef[map->num_coef+n];
      map->coef[4*n+2]=icoef[2*map->num_coef+n];
      map->coef[4*n+3]=icoef[3*map->num_coef+n];
    }
  } else map->num_coef=0;


  s=CnvMapFwrite(fp,map,grd);

  CnvMapFree(map);
  GridFree(grd);

  return (IDL_GettmpLong(s));


}

static IDL_VPTR IDLCnvMapRead(int argc,IDL_VPTR *argv) {

  int s=0;


  IDL_VPTR vprm=NULL,vstvec=NULL,vgvec=NULL,vmvec=NULL,vcoef=NULL,vbnd=NULL;

  IDL_LONG unit=0;
  IDL_FILE_STAT stat;


  struct CnvMapData *map=NULL;
  struct GridData *grd=NULL;

  struct CnvMapIDLPrm *iprm=NULL;
  struct GridIDLStVec *istvec=NULL;
  struct GridIDLGVec *igvec=NULL;
  struct GridIDLGVec *imvec=NULL;
  struct CnvMapIDLBnd *ibnd=NULL;
  double *icoef=NULL; 

  IDL_MEMINT cdim[2];

  FILE *fp=NULL;

  IDL_ENSURE_SCALAR(argv[0]);
  IDL_EXCLUDE_EXPR(argv[1]);
  IDL_EXCLUDE_EXPR(argv[2]);
  IDL_EXCLUDE_EXPR(argv[4]);
  IDL_EXCLUDE_EXPR(argv[5]);
  IDL_EXCLUDE_EXPR(argv[6]);

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
 

  map=CnvMapMake(); 
  grd=GridMake();

  s=CnvMapFread(fp,map,grd);

  if (s==-1) {
    CnvMapFree(map);
    GridFree(grd);
    return (IDL_GettmpLong(s));
  }

  iprm=IDLMakeCnvMapPrm(&vprm);
  if (grd->stnum !=0) istvec=IDLMakeGridStVec(grd->stnum,&vstvec);
  if (grd->vcnum !=0) igvec=IDLMakeGridGVec(grd->vcnum,&vgvec);
  if (map->num_model !=0) imvec=IDLMakeGridGVec(map->num_model,&vmvec);
  if (map->num_bnd !=0) ibnd=IDLMakeCnvMapBnd(map->num_bnd,&vbnd);
  if (map->num_coef !=0) {
    int n=0;
    cdim[1]=4;
    cdim[0]=map->num_coef;
    icoef=(double *) 
      IDL_MakeTempArray(IDL_TYP_DOUBLE,2,cdim,IDL_ARR_INI_ZERO,&vcoef);
   
      for (n=0;n<map->num_coef;n++) {
       icoef[n]=map->coef[4*n];
       icoef[map->num_coef+n]=map->coef[4*n+1];
       icoef[2*map->num_coef+n]=map->coef[4*n+2];
       icoef[3*map->num_coef+n]=map->coef[4*n+3];
      }

  }

  /* copy the data here */
   
  IDLCopyCnvMapPrmToIDL(map,grd,iprm);
  if (istvec !=NULL) IDLCopyGridStVecToIDL(grd,grd->stnum,
                                    vstvec->value.s.arr->elt_len,istvec);
  if (igvec !=NULL) IDLCopyGridGVecToIDL(grd,grd->vcnum,
                                    vgvec->value.s.arr->elt_len,igvec);
  if (imvec !=NULL) IDLCopyCnvMapGVecToIDL(map,map->num_model,
                                           vmvec->value.s.arr->elt_len,imvec);
  if (ibnd !=NULL) IDLCopyCnvMapBndToIDL(map,map->num_bnd,
                                         vbnd->value.s.arr->elt_len,ibnd);

  CnvMapFree(map);
  GridFree(grd);

  IDL_VarCopy(vprm,argv[1]);
  if (vstvec !=NULL) IDL_VarCopy(vstvec,argv[2]);
  if (vgvec !=NULL) IDL_VarCopy(vgvec,argv[3]);
  if (vmvec !=NULL) IDL_VarCopy(vmvec,argv[4]);
  if (vcoef !=NULL) IDL_VarCopy(vcoef,argv[5]);
  if (vbnd !=NULL) IDL_VarCopy(vbnd,argv[6]);


  return (IDL_GettmpLong(s));

}




static IDL_VPTR IDLCnvMapLoadInx(int argc,IDL_VPTR *argv) {

  int s=0,n;

  IDL_VPTR vinx=NULL;

  IDL_LONG unit=0;
  IDL_FILE_STAT stat;

  FILE *fp=NULL;

  struct CnvMapIDLInx *ifptr=NULL;
  struct CnvMapIndex *finx=NULL;

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
 
  finx=CnvMapIndexFload(fp);

  if (finx==NULL) {
    s=-1;
    return (IDL_GettmpLong(s));
  }

  ifptr=IDLMakeCnvMapInx(finx->num,&vinx);
  
  for (n=0;n<finx->num;n++) {
    ifptr[n].time=finx->tme[n];
    ifptr[n].offset=finx->inx[n];
  }
  

  CnvMapIndexFree(finx);
  
  IDL_VarCopy(vinx,argv[1]); 
   
  return (IDL_GettmpLong(s));
}


static IDL_VPTR IDLCnvMapSeek(int argc,IDL_VPTR *argv,char *argk) {

  struct CnvMapIDLInx *ifptr;

  int s=0,n;
  struct CnvMapIndex *finx=NULL;
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
    finx=malloc(sizeof(struct CnvMapIndex));
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
      ifptr=(struct CnvMapIDLInx *) (outargv[7]->value.s.arr->data+
                                  n*outargv[7]->value.s.arr->elt_len);
      finx->tme[n]=ifptr->time;
      finx->inx[n]=ifptr->offset;
    }
  }

  s=CnvMapFseek(fp,yr,mo,dy,hr,mt,sc,&atme,finx);

  if (vatme) IDL_StoreScalar(vatme,IDL_TYP_DOUBLE,(IDL_ALLTYPES *) &atme);
 
    

  if (finx !=NULL) CnvMapIndexFree(finx);

  IDL_KWCleanup(IDL_KW_CLEAN);
  return (IDL_GettmpLong(s));


}



static IDL_VPTR IDLCnvMapOpen(int argc,IDL_VPTR *argv,char *argk) {
  
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


static IDL_VPTR IDLCnvMapClose(int argc,IDL_VPTR *argv) {
  int s=0;
  IDL_ENSURE_SCALAR(argv[0]);
  IDL_FileFreeUnit(1,argv);
  return (IDL_GettmpLong(s));

}

int IDL_Load(void) {

  static IDL_SYSFUN_DEF2 fnaddr[]={
    { {IDLCnvMapRead},"CNVMAPREAD",7,7,0,0},
    { {IDLCnvMapWrite},"CNVMAPWRITE",7,7,0,0},
    { {IDLCnvMapLoadInx},"CNVMAPLOADINX",2,2,0,0},
    { {IDLCnvMapSeek},"CNVMAPSEEK",7,8,IDL_SYSFUN_DEF_F_KEYWORDS,0},
    { {IDLCnvMapOpen},"CNVMAPOPEN",1,1,IDL_SYSFUN_DEF_F_KEYWORDS,0},
    { {IDLCnvMapClose},"CNVMAPCLOSE",1,1,0,0},
  };


  if (!(msg_block = IDL_MessageDefineBlock("cnvmap",
                    IDL_CARRAY_ELTS(msg_arr), msg_arr)))
    return IDL_FALSE;

  return IDL_SysRtnAdd(fnaddr,TRUE,IDL_CARRAY_ELTS(fnaddr));

}
