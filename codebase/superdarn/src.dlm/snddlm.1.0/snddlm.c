/* snddlm.c
   ========
   Author E.G.Thomas


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
#include "snddata.h"
#include "sndread.h"
#include "sndwrite.h"
#include "sndseek.h"

#include "sndidl.h"

#define SND_ERROR 0


static IDL_MSG_DEF msg_arr[] =
  {
    {  "SND_ERROR",   "%NError: %s." },
  };

static IDL_MSG_BLOCK msg_block;


static IDL_VPTR IDLSndWrite(int argc,IDL_VPTR *argv) {

  int s=0;

  IDL_LONG unit=0;
  IDL_FILE_STAT stat;

  struct SndIDLData *isnd=NULL;

  struct SndData *snd=NULL;

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

  isnd=(struct SndIDLData *) argv[1]->value.s.arr->data;

  snd=SndMake(); 

  IDLCopySndDataFromIDL(snd->nrang,isnd,snd);

  s=SndFwrite(fp,snd);

  SndFree(snd);

  return (IDL_GettmpLong(s));

}


static IDL_VPTR IDLSndRead(int argc,IDL_VPTR *argv) {

  int s=0;

  IDL_VPTR vsnd=NULL;

  IDL_LONG unit=0;
  IDL_FILE_STAT stat;

  struct SndIDLData *isnd=NULL;

  struct SndData *snd=NULL;

  FILE *fp=NULL;

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

  snd=SndMake(); 

  s=SndFread(fp,snd);

  if (s==-1) {
    SndFree(snd);
    return (IDL_GettmpLong(s));
  }

  isnd=IDLMakeSndData(&vsnd);

  /* copy data here */

  IDLCopySndDataToIDL(snd->nrang,snd,isnd);

  SndFree(snd);

  IDL_VarCopy(vsnd,argv[1]);

  return (IDL_GettmpLong(s));

}


static IDL_VPTR IDLSndSeek(int argc,IDL_VPTR *argv,char *argk) {

  int s=0;
  static IDL_VPTR vatme;
  double atme=0;

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
  IDL_KWGetParams(argc,argv,argk,kw_pars,outargv,1);

  IDL_ENSURE_SCALAR(outargv[0]);
  IDL_ENSURE_SCALAR(outargv[1]);
  IDL_ENSURE_SCALAR(outargv[2]);
  IDL_ENSURE_SCALAR(outargv[3]);
  IDL_ENSURE_SCALAR(outargv[4]);
  IDL_ENSURE_SCALAR(outargv[5]);
  IDL_ENSURE_SCALAR(outargv[6]);

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

  s=SndFseek(fp,yr,mo,dy,hr,mt,sc,&atme);

  if (vatme) IDL_StoreScalar(vatme,IDL_TYP_DOUBLE,(IDL_ALLTYPES *) &atme);

  IDL_KWCleanup(IDL_KW_CLEAN);
  return (IDL_GettmpLong(s));

}


static IDL_VPTR IDLSndOpen(int argc,IDL_VPTR *argv,char *argk) {

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


static IDL_VPTR IDLSndClose(int argc,IDL_VPTR *argv) {
  int s=0;
  IDL_ENSURE_SCALAR(argv[0]);
  IDL_FileFreeUnit(1,argv);
  return (IDL_GettmpLong(s));

}


int IDL_Load(void) {

  static IDL_SYSFUN_DEF2 fnaddr[]={
    { {IDLSndRead},"SNDREAD",2,2,0,0},
    { {IDLSndWrite},"SNDWRITE",2,2,0,0},
    { {IDLSndSeek},"SNDSEEK",6,7,IDL_SYSFUN_DEF_F_KEYWORDS,0},
    { {IDLSndOpen},"SNDOPEN",1,1,IDL_SYSFUN_DEF_F_KEYWORDS,0},
    { {IDLSndClose},"SNDCLOSE",1,1,0,0},
  };

  if (!(msg_block = IDL_MessageDefineBlock("snd",
                    IDL_CARRAY_ELTS(msg_arr), msg_arr)))
    return IDL_FALSE;

  return IDL_SysRtnAdd(fnaddr,TRUE,IDL_CARRAY_ELTS(fnaddr));

}

