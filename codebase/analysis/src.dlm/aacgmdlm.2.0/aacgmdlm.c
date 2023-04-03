/* aacgmdlm.c
   =========== 
   Author R.J.Barnes
*/

/*
  Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

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
#include "aacgm.h"
#include "aacgmlib_v2.h"

#define AACGM_ERROR 0
#define AACGM_MISMATCHDIMS -1
#define AACGM_MISMATCHELMS -2
#define AACGM_BADTYPE -3
#define AACGM_DATENOTSET -4

static IDL_MSG_DEF msg_arr[] =
  {
    {  "AACGM_ERROR",   "%NError: %s." },
    {  "AACGM_MISMATCHDIMS","%NMismatched array dimensions %s"},
    {  "AACGM_MISMATCHELMS","%NNumber of array elements do not match %s"},
    {  "AACGM_BADTYPE","%NArrays of floating point type are allowed %s"},
    {  "AACGM_DATENOTSET","%NDate and Time are not currently set"},
  };

static IDL_MSG_BLOCK msg_block;


static IDL_VPTR IDLAACGMInit(int argc,IDL_VPTR *argv) {

  int s=0;

  IDL_LONG year=0;

  IDL_ENSURE_SCALAR(argv[0]);
  IDL_EXCLUDE_EXPR(argv[1]);

  year=IDL_LongScalar(argv[0]);

  s=AACGMInit(year);

  return (IDL_GettmpLong(s));
}


static IDL_VPTR IDLAACGMLoadCoef(int argc,IDL_VPTR *argv) {

  int s=0;

  IDL_LONG unit=0;
  IDL_FILE_STAT stat;

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

  AACGMLoadCoefFP(fp);
   
  return (IDL_GettmpLong(s));
}


static IDL_VPTR IDLAACGMConvert(int argc,IDL_VPTR *argv,char *argk) {
  
  int s=0,n;
  IDL_VPTR outargv[6];
  IDL_VPTR volat,volon,vr;

  double inlat,inlon,height;
  double outlat,outlon,r;
  double *latptr,*lonptr,*rptr;

  static IDL_LONG geo;

  static IDL_KW_PAR kw_pars[]={IDL_KW_FAST_SCAN,
                                {"GEO",IDL_TYP_LONG,1,IDL_KW_ZERO,0,
                                  IDL_CHARA(geo)}, {NULL}};

  IDL_KWCleanup(IDL_KW_MARK);
  IDL_KWGetParams(argc,argv,argk,kw_pars,outargv,1);
 
  IDL_EXCLUDE_EXPR(outargv[3]);
  IDL_EXCLUDE_EXPR(outargv[4]);
  IDL_EXCLUDE_EXPR(outargv[5]);

  if (argv[1]->flags & IDL_V_ARR) {
    int nval;
    IDL_ENSURE_ARRAY(outargv[1]);
    IDL_ENSURE_ARRAY(outargv[2]);
 
    nval=outargv[0]->value.arr->n_elts;
    if ( (outargv[1]->value.arr->n_elts !=nval) ||
         (outargv[2]->value.arr->n_elts !=nval)) {

      /* array size mismatch */

      IDL_MessageFromBlock(msg_block,AACGM_MISMATCHELMS,IDL_MSG_LONGJMP,
                           "in AACGMConvert()");
    }

    if ((outargv[0]->type !=IDL_TYP_FLOAT) && 
        (outargv[0]->type !=IDL_TYP_DOUBLE)) {

      /* bad type */

      IDL_MessageFromBlock(msg_block,AACGM_BADTYPE,IDL_MSG_LONGJMP,
                           "of lat in AACGMConvert()");
    }
   
    if ((outargv[1]->type !=IDL_TYP_FLOAT) && 
        (outargv[1]->type !=IDL_TYP_DOUBLE)) {

      /* bad type */

      IDL_MessageFromBlock(msg_block,AACGM_BADTYPE,IDL_MSG_LONGJMP,
                           "of lon in AACGMConvert()");
    }
   
    if ((outargv[2]->type !=IDL_TYP_FLOAT) && 
        (outargv[2]->type !=IDL_TYP_DOUBLE)) {

      /* bad type */

      IDL_MessageFromBlock(msg_block,AACGM_BADTYPE,IDL_MSG_LONGJMP,
                           "of height in AACGMConvert()");
    }
   
    latptr=(double *) IDL_MakeTempArray(IDL_TYP_DOUBLE,
                                        outargv[0]->value.arr->n_dim,
                                        outargv[0]->value.arr->dim,
                                        IDL_ARR_INI_ZERO,&volat);
    lonptr=(double *) IDL_MakeTempArray(IDL_TYP_DOUBLE,
                                        outargv[0]->value.arr->n_dim,
                                        outargv[0]->value.arr->dim,
                                        IDL_ARR_INI_ZERO,&volon);

    rptr=(double *) IDL_MakeTempArray(IDL_TYP_DOUBLE,
                                      outargv[0]->value.arr->n_dim,
                                      outargv[0]->value.arr->dim,
                                      IDL_ARR_INI_ZERO,&vr);

    for (n=0;n<nval;n++) {
      if (outargv[0]->type==IDL_TYP_FLOAT)
           inlat= ((float *) outargv[0]->value.arr->data)[n];
      else inlat=((double *) outargv[0]->value.arr->data)[n];
      if (outargv[1]->type==IDL_TYP_FLOAT)
           inlon= ((float *) outargv[1]->value.arr->data)[n];
      else inlon=((double *) outargv[1]->value.arr->data)[n];
      if (outargv[2]->type==IDL_TYP_FLOAT)
           height= ((float *) outargv[2]->value.arr->data)[n];
      else height=((double *) outargv[2]->value.arr->data)[n];

      s=AACGMConvert(inlat,inlon,height,&latptr[n],&lonptr[n],
                     &rptr[n],geo !=0);
    }

    IDL_VarCopy(volat,argv[3]);
    IDL_VarCopy(volon,argv[4]);
    IDL_VarCopy(vr,argv[5]);

  } else {
    /* scalar */

    IDL_ENSURE_SCALAR(outargv[1]);
    IDL_ENSURE_SCALAR(outargv[2]);

    inlat=IDL_DoubleScalar(outargv[0]);
    inlon=IDL_DoubleScalar(outargv[1]);
    height=IDL_DoubleScalar(outargv[2]);

    s=AACGMConvert(inlat,inlon,height,&outlat,&outlon,&r,geo !=0);
    IDL_StoreScalar(outargv[3],IDL_TYP_DOUBLE,(IDL_ALLTYPES *) &outlat);
    IDL_StoreScalar(outargv[4],IDL_TYP_DOUBLE,(IDL_ALLTYPES *) &outlon);
    IDL_StoreScalar(outargv[5],IDL_TYP_DOUBLE,(IDL_ALLTYPES *) &r);
  }
 
  IDL_KWCleanup(IDL_KW_CLEAN);
  return (IDL_GettmpLong(s));

}


static IDL_VPTR IDLAACGM_v2_GetDateTime(int argc,IDL_VPTR *argv,char *argk) {

    int s=0;
    int yr,mo,dy,hr,mt,sc,dayno;
    IDL_VPTR outargv[7];

    static IDL_VPTR month;
    static IDL_VPTR day;
    static IDL_VPTR hour;
    static IDL_VPTR minute;
    static IDL_VPTR second;
    static IDL_VPTR dyno;
    static IDL_LONG silent;
    
    static IDL_KW_PAR kw_pars[]={IDL_KW_FAST_SCAN,
        {"DAY",IDL_TYP_UNDEF,1,IDL_KW_OUT|IDL_KW_ZERO,0,IDL_CHARA(day)},
        {"DYNO",IDL_TYP_UNDEF,1,IDL_KW_OUT|IDL_KW_ZERO,0,IDL_CHARA(dyno)},
        {"HOUR",IDL_TYP_UNDEF,1,IDL_KW_OUT|IDL_KW_ZERO,0,IDL_CHARA(hour)},
        {"MINUTE",IDL_TYP_UNDEF,1,IDL_KW_OUT|IDL_KW_ZERO,0,IDL_CHARA(minute)},
        {"MONTH",IDL_TYP_UNDEF,1,IDL_KW_OUT|IDL_KW_ZERO,0,IDL_CHARA(month)},
        {"SECOND",IDL_TYP_UNDEF,1,IDL_KW_OUT|IDL_KW_ZERO,0,IDL_CHARA(second)},
        {"SILENT",IDL_TYP_LONG,1,IDL_KW_ZERO,0,IDL_CHARA(silent)},
        {NULL}};

    IDL_KWCleanup(IDL_KW_MARK);
    IDL_KWGetParams(argc,argv,argk,kw_pars,outargv,1);

    s=AACGM_v2_GetDateTime(&yr,&mo,&dy,&hr,&mt,&sc,&dayno);

    if (yr==-1) {
        if (!silent) IDL_MessageFromBlock(msg_block,AACGM_DATENOTSET,IDL_MSG_RET);
        s=-1;
        IDL_KWCleanup(IDL_KW_CLEAN);
        return (IDL_GettmpLong(s));
    }

    IDL_StoreScalar(outargv[0],IDL_TYP_LONG,(IDL_ALLTYPES *) &yr);
    if (month) IDL_StoreScalar(month,IDL_TYP_LONG,(IDL_ALLTYPES *) &mo);
    if (day) IDL_StoreScalar(day,IDL_TYP_LONG,(IDL_ALLTYPES *) &dy);
    if (hour) IDL_StoreScalar(hour,IDL_TYP_LONG,(IDL_ALLTYPES *) &hr);
    if (minute) IDL_StoreScalar(minute,IDL_TYP_LONG,(IDL_ALLTYPES *) &mt);
    if (second) IDL_StoreScalar(second,IDL_TYP_LONG,(IDL_ALLTYPES *) &sc);
    if (dyno) IDL_StoreScalar(dyno,IDL_TYP_LONG,(IDL_ALLTYPES *) &dayno);

    IDL_KWCleanup(IDL_KW_CLEAN);
    return (IDL_GettmpLong(s));
}


static IDL_VPTR IDLAACGM_v2_SetDateTime(int argc,IDL_VPTR *argv) {

    int s=0;
    int yr,mo,dy,hr,mt,sc;

    IDL_ENSURE_SCALAR(argv[0]);
    yr=IDL_LongScalar(argv[0]);
    mo=1;
    dy=1;
    hr=0;
    mt=0;
    sc=0;

    if (argc > 5) {
        IDL_ENSURE_SCALAR(argv[5]);
        sc=IDL_LongScalar(argv[5]);
    }

    if (argc > 4) {
        IDL_ENSURE_SCALAR(argv[4]);
        mt=IDL_LongScalar(argv[4]);
    }

    if (argc > 3) {
        IDL_ENSURE_SCALAR(argv[3]);
        hr=IDL_LongScalar(argv[3]);
    }

    if (argc > 2) {
        IDL_ENSURE_SCALAR(argv[2]);
        dy=IDL_LongScalar(argv[2]);
    }

    if (argc > 1) {
        IDL_ENSURE_SCALAR(argv[1]);
        mo=IDL_LongScalar(argv[1]);
    }

    s=AACGM_v2_SetDateTime(yr,mo,dy,hr,mt,sc);

    return (IDL_GettmpLong(s));
}


static IDL_VPTR IDLAACGM_v2_SetNow(int argc,IDL_VPTR *argv) {

    int s=0;
    s=AACGM_v2_SetNow();

    return (IDL_GettmpLong(s));
}


static IDL_VPTR IDLAACGM_v2_Convert(int argc,IDL_VPTR *argv,char *argk) {

    int s=0,n;
    IDL_VPTR outargv[6];
    IDL_VPTR volat,volon,vr;

    double inlat,inlon,height;
    double outlat,outlon,r;
    double *latptr,*lonptr,*rptr;

    int code=0;

    static IDL_LONG allowtrace;
    static IDL_LONG badidea;
    static IDL_LONG gcentric;
    static IDL_LONG geo;
    static IDL_LONG trace;
    static IDL_LONG verbose;

    static IDL_KW_PAR kw_pars[]={IDL_KW_FAST_SCAN,
        {"ALLOW_TRACE",IDL_TYP_LONG,1,IDL_KW_ZERO,0,IDL_CHARA(allowtrace)},
        {"BAD_IDEA",IDL_TYP_LONG,1,IDL_KW_ZERO,0,IDL_CHARA(badidea)},
        {"GCENTRIC",IDL_TYP_LONG,1,IDL_KW_ZERO,0,IDL_CHARA(gcentric)},
        {"GEO",IDL_TYP_LONG,1,IDL_KW_ZERO,0,IDL_CHARA(geo)},
        {"TRACE",IDL_TYP_LONG,1,IDL_KW_ZERO,0,IDL_CHARA(trace)},
        {"VERBOSE",IDL_TYP_LONG,1,IDL_KW_ZERO,0,IDL_CHARA(verbose)},
        {NULL}};

    IDL_KWCleanup(IDL_KW_MARK);
    IDL_KWGetParams(argc,argv,argk,kw_pars,outargv,1);

    IDL_EXCLUDE_EXPR(outargv[3]);
    IDL_EXCLUDE_EXPR(outargv[4]);
    IDL_EXCLUDE_EXPR(outargv[5]);

    if (geo)        code|=1;
    if (trace)      code|=2;
    if (allowtrace) code|=4;
    if (badidea)    code|=8;
    if (gcentric)   code|=16;
    if (verbose)    code|=32;

    if (argv[1]->flags & IDL_V_ARR) {
        int nval;
        IDL_ENSURE_ARRAY(outargv[1]);
        IDL_ENSURE_ARRAY(outargv[2]);

        nval=outargv[0]->value.arr->n_elts;
        if ( (outargv[1]->value.arr->n_elts !=nval) ||
             (outargv[2]->value.arr->n_elts !=nval)) {

            /* array size mismatch */
            IDL_MessageFromBlock(msg_block,AACGM_MISMATCHELMS,IDL_MSG_LONGJMP,
                                    "in AACGM_v2_Convert()");
        }

        if ((outargv[0]->type !=IDL_TYP_FLOAT) && 
            (outargv[0]->type !=IDL_TYP_DOUBLE)) {

            /* bad type */
            IDL_MessageFromBlock(msg_block,AACGM_BADTYPE,IDL_MSG_LONGJMP,
                                    "of lat in AACGM_v2_Convert()");
        }

        if ((outargv[1]->type !=IDL_TYP_FLOAT) && 
            (outargv[1]->type !=IDL_TYP_DOUBLE)) {

            /* bad type */
            IDL_MessageFromBlock(msg_block,AACGM_BADTYPE,IDL_MSG_LONGJMP,
                                    "of lon in AACGM_v2_Convert()");
        }

        if ((outargv[2]->type !=IDL_TYP_FLOAT) && 
            (outargv[2]->type !=IDL_TYP_DOUBLE)) {

            /* bad type */
            IDL_MessageFromBlock(msg_block,AACGM_BADTYPE,IDL_MSG_LONGJMP,
                                    "of height in AACGM_v2_Convert()");
        }

        latptr=(double *) IDL_MakeTempArray(IDL_TYP_DOUBLE,
                                            outargv[0]->value.arr->n_dim,
                                            outargv[0]->value.arr->dim,
                                            IDL_ARR_INI_ZERO,&volat);

        lonptr=(double *) IDL_MakeTempArray(IDL_TYP_DOUBLE,
                                            outargv[0]->value.arr->n_dim,
                                            outargv[0]->value.arr->dim,
                                            IDL_ARR_INI_ZERO,&volon);

        rptr=(double *) IDL_MakeTempArray(IDL_TYP_DOUBLE,
                                          outargv[0]->value.arr->n_dim,
                                          outargv[0]->value.arr->dim,
                                          IDL_ARR_INI_ZERO,&vr);

        for (n=0;n<nval;n++) {
            if (outargv[0]->type==IDL_TYP_FLOAT)
                 inlat= ((float *) outargv[0]->value.arr->data)[n];
            else inlat=((double *) outargv[0]->value.arr->data)[n];
            if (outargv[1]->type==IDL_TYP_FLOAT)
                 inlon= ((float *) outargv[1]->value.arr->data)[n];
            else inlon=((double *) outargv[1]->value.arr->data)[n];
            if (outargv[2]->type==IDL_TYP_FLOAT)
                 height= ((float *) outargv[2]->value.arr->data)[n];
            else height=((double *) outargv[2]->value.arr->data)[n];

            s=AACGM_v2_Convert(inlat,inlon,height,&latptr[n],&lonptr[n],
                               &rptr[n],code);
        }

        IDL_VarCopy(volat,argv[3]);
        IDL_VarCopy(volon,argv[4]);
        IDL_VarCopy(vr,argv[5]);

    } else {
        /* scalar */

        IDL_ENSURE_SCALAR(outargv[1]);
        IDL_ENSURE_SCALAR(outargv[2]);

        inlat=IDL_DoubleScalar(outargv[0]);
        inlon=IDL_DoubleScalar(outargv[1]);
        height=IDL_DoubleScalar(outargv[2]);

        s=AACGM_v2_Convert(inlat,inlon,height,&outlat,&outlon,&r,code);
        IDL_StoreScalar(outargv[3],IDL_TYP_DOUBLE,(IDL_ALLTYPES *) &outlat);
        IDL_StoreScalar(outargv[4],IDL_TYP_DOUBLE,(IDL_ALLTYPES *) &outlon);
        IDL_StoreScalar(outargv[5],IDL_TYP_DOUBLE,(IDL_ALLTYPES *) &r);
    }

    IDL_KWCleanup(IDL_KW_CLEAN);
    return (IDL_GettmpLong(s));
}


int IDL_Load(void) {

  static IDL_SYSFUN_DEF2 fnaddr[]={
    { {IDLAACGMInit},"AACGMINIT",1,1,0,0},
    { {IDLAACGMLoadCoef},"AACGMLOADCOEF",1,1,0,0},
    { {IDLAACGMConvert},"AACGMCONVERT",6,6,IDL_SYSFUN_DEF_F_KEYWORDS,0},
    { {IDLAACGM_v2_GetDateTime},"AACGM_V2_GETDATETIME",1,1,IDL_SYSFUN_DEF_F_KEYWORDS,0},
    { {IDLAACGM_v2_SetDateTime},"AACGM_V2_SETDATETIME",1,6,0,0},
    { {IDLAACGM_v2_SetNow},"AACGM_V2_SETNOW",0,0,0,0},
    { {IDLAACGM_v2_Convert},"AACGM_V2_CONVERT",6,6,IDL_SYSFUN_DEF_F_KEYWORDS,0},
  };


  if (!(msg_block = IDL_MessageDefineBlock("aacgm",
                    IDL_CARRAY_ELTS(msg_arr), msg_arr)))
    return IDL_FALSE;

  return IDL_SysRtnAdd(fnaddr,TRUE,IDL_CARRAY_ELTS(fnaddr));

}
