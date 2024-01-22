/* igrfdlm.c
   =========
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
#include "rtime.h"
#include "igrfcall.h"
#include "igrflib.h"

#define IGRF_ERROR 0
#define IGRF_MISMATCHDIMS -1
#define IGRF_MISMATCHELMS -2
#define IGRF_BADTYPE -3
#define IGRF_DATENOTSET -4
#define IGRF_BADRTP -5

static IDL_MSG_DEF msg_arr[] =
  {
    {  "IGRF_ERROR",   "%NError: %s." },
    {  "IGRF_MISMATCHDIMS","%NMismatched array dimensions %s"},
    {  "IGRF_MISMATCHELMS","%NNumber of array elements do not match %s"},
    {  "IGRF_BADTYPE","%NArrays of floating point type are allowed %s"},
    {  "IGRF_DATENOTSET","%NDate and Time are not currently set"},
    {  "IGRF_BADRTP","%NNumber of input rtp array elements must be 3 %s"},
  };

static IDL_MSG_BLOCK msg_block;

union IDLIGRFPtr {
  unsigned char *bptr;
  short *iptr;
  int *lptr;
  float *fptr;
  double *dptr;
  void *vptr;
};


static IDL_VPTR IDLIGRF_GetDateTime(int argc,IDL_VPTR *argv,char *argk) {

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

  s=IGRF_GetDateTime(&yr,&mo,&dy,&hr,&mt,&sc,&dayno);

  if (yr==-1) {
    if (!silent) IDL_MessageFromBlock(msg_block,IGRF_DATENOTSET,IDL_MSG_RET);
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


static IDL_VPTR IDLIGRF_SetDateTime(int argc,IDL_VPTR *argv) {

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

  s=IGRF_SetDateTime(yr,mo,dy,hr,mt,sc);

  return (IDL_GettmpLong(s));
}


static IDL_VPTR IDLIGRF_SetNow(int argc,IDL_VPTR *argv) {

  int s=0;
  s=IGRF_SetNow();

  return (IDL_GettmpLong(s));
}


static IDL_VPTR IDLIGRF_Tilt(int argc,IDL_VPTR *argv) {

    int n;

    if (argv[0]->flags & IDL_V_ARR) {
        int nval;
        IDL_VPTR vigrf;
        double *igrfptr=NULL;
        int yrtmp=0,motmp=0,dytmp=0,hrtmp=0,mttmp=0,sctmp=0;
        double igrf=0;

        union IDLIGRFPtr yr,mo,dy,hr,mt,sc;
        int yrtyp=0,motyp=0,dytyp=0,hrtyp=0,mttyp=0,sctyp=0;

        yr.vptr=NULL;

        nval=argv[0]->value.arr->n_elts;

        for (n=1;n<argc;n++) IDL_ENSURE_ARRAY(argv[n]);

        for (n=0;n<argc;n++) {
            if ((argv[n]->type !=IDL_TYP_INT) &&
                (argv[n]->type !=IDL_TYP_LONG) &&
                (argv[n]->type !=IDL_TYP_FLOAT) &&
                (argv[n]->type !=IDL_TYP_DOUBLE)) {

                /* bad type */
                char msg[256];
                sprintf(msg,"of argument %d in IGRF_Tilt()",n+1);
                IDL_MessageFromBlock(msg_block,IGRF_BADTYPE,IDL_MSG_LONGJMP,
                                        msg);
            }
        }

        for (n=0;n<argc;n++) {
            if (argv[n]->value.arr->n_elts !=nval)
                IDL_MessageFromBlock(msg_block,IGRF_MISMATCHELMS,IDL_MSG_LONGJMP,
                                        "in IGRF_Tilt()");
        }

        yr.vptr=(void *) argv[0]->value.arr->data;
        yrtyp=argv[0]->type;

        mo.vptr=(void *) argv[1]->value.arr->data;
        motyp=argv[1]->type;

        dy.vptr=(void *) argv[2]->value.arr->data;
        dytyp=argv[2]->type;

        hr.vptr=(void *) argv[3]->value.arr->data;
        hrtyp=argv[3]->type;

        mt.vptr=(void *) argv[4]->value.arr->data;
        mttyp=argv[4]->type;

        sc.vptr=(void *) argv[5]->value.arr->data;
        sctyp=argv[5]->type;

        igrfptr=(double *) IDL_MakeTempArray(IDL_TYP_DOUBLE,
                                            argv[0]->value.arr->n_dim,
                                            argv[0]->value.arr->dim,
                                            IDL_ARR_INI_ZERO,&vigrf);

        for (n=0;n<nval;n++) {
            switch (yrtyp) {
                case IDL_TYP_INT:
                    yrtmp=yr.iptr[n];
                    break;
                case IDL_TYP_LONG:
                    yrtmp=yr.lptr[n];
                    break;
                case IDL_TYP_FLOAT:
                    yrtmp=yr.fptr[n];
                    break;
                case IDL_TYP_DOUBLE:
                    yrtmp=yr.dptr[n];
                    break;
                default:
                    yrtmp=1970;
            }

            switch (motyp) {
                case IDL_TYP_INT:
                    motmp=mo.iptr[n];
                    break;
                case IDL_TYP_LONG:
                    motmp=mo.lptr[n];
                    break;
                case IDL_TYP_FLOAT:
                    motmp=mo.fptr[n];
                    break;
                case IDL_TYP_DOUBLE:
                    motmp=mo.dptr[n];
                    break;
                default:
                    motmp=1;
            }

            switch (dytyp) {
                case IDL_TYP_INT:
                    dytmp=dy.iptr[n];
                    break;
                case IDL_TYP_LONG:
                    dytmp=dy.lptr[n];
                    break;
                case IDL_TYP_FLOAT:
                    dytmp=dy.fptr[n];
                    break;
                case IDL_TYP_DOUBLE:
                    dytmp=dy.dptr[n];
                    break;
                default:
                    dytmp=1;
            }

            switch (hrtyp) {
                case IDL_TYP_INT:
                    hrtmp=hr.iptr[n];
                    break;
                case IDL_TYP_LONG:
                    hrtmp=hr.lptr[n];
                    break;
                case IDL_TYP_FLOAT:
                    hrtmp=hr.fptr[n];
                    break;
                case IDL_TYP_DOUBLE:
                    hrtmp=hr.dptr[n];
                    break;
                default:
                    hrtmp=0;
            }

            switch (mttyp) {
                case IDL_TYP_INT:
                    mttmp=mt.iptr[n];
                    break;
                case IDL_TYP_LONG:
                    mttmp=mt.lptr[n];
                    break;
                case IDL_TYP_FLOAT:
                    mttmp=mt.fptr[n];
                    break;
                case IDL_TYP_DOUBLE:
                    mttmp=mt.dptr[n];
                    break;
                default:
                    mttmp=0;
            }

            switch (sctyp) {
                case IDL_TYP_INT:
                    sctmp=sc.iptr[n];
                    break;
                case IDL_TYP_LONG:
                    sctmp=sc.lptr[n];
                    break;
                case IDL_TYP_FLOAT:
                    sctmp=sc.fptr[n];
                    break;
                case IDL_TYP_DOUBLE:
                    sctmp=sc.dptr[n];
                    break;
                default:
                    sctmp=0;
            }

            igrf=IGRF_Tilt(yrtmp,motmp,dytmp,hrtmp,mttmp,sctmp);
            igrfptr[n]=igrf;
        }
        //Is this the same address as igrfptr? Check out line 106. If vigrf is the same address as
        //igrfptr, why is it necessary to return the vigrf variable? Is it just for consistency?
        return (vigrf);

    } else {
        /* scalar */
        int yr,mo,dy,hr,mt,sc;
        double igrf;
        IDL_VPTR vigrf;

        for (n=0;n<argc;n++) IDL_ENSURE_SCALAR(argv[n]);

        yr=IDL_LongScalar(argv[0]);
        mo=IDL_LongScalar(argv[1]);
        dy=IDL_LongScalar(argv[2]);
        hr=IDL_LongScalar(argv[3]);
        mt=IDL_LongScalar(argv[4]);
        sc=IDL_LongScalar(argv[5]);

        igrf=IGRF_Tilt(yr,mo,dy,hr,mt,sc);

        vigrf=IDL_Gettmp();
        vigrf->type=IDL_TYP_DOUBLE;
        vigrf->value.d=igrf;

        return (vigrf);
    }

    return (IDL_GettmpLong(0));
}


static IDL_VPTR IDLECDIP_Convert(int argc,IDL_VPTR *argv,char *argk) {

    int s=0,n;

    IDL_VPTR outargv[6];
    IDL_VPTR volat,volon,vr;

    double inlat,inlon,height;
    double outlat,outlon,r;
    double *latptr,*lonptr,*rptr;

    double out[3];
    double Re=6371.0;

    static IDL_LONG inverse;

    static IDL_KW_PAR kw_pars[]={IDL_KW_FAST_SCAN,
        {"INVERSE",IDL_TYP_LONG,1,IDL_KW_ZERO,0,IDL_CHARA(inverse)},
        {NULL}};

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
            IDL_MessageFromBlock(msg_block,IGRF_MISMATCHELMS,IDL_MSG_LONGJMP,
                                    "in ECDIP_Convert()");
        }

        if ((outargv[0]->type !=IDL_TYP_FLOAT) &&
            (outargv[0]->type !=IDL_TYP_DOUBLE)) {

            /* bad type */
            IDL_MessageFromBlock(msg_block,IGRF_BADTYPE,IDL_MSG_LONGJMP,
                                    "of lat in ECDIP_Convert()");
        }

        if ((outargv[1]->type !=IDL_TYP_FLOAT) &&
            (outargv[1]->type !=IDL_TYP_DOUBLE)) {

            /* bad type */
            IDL_MessageFromBlock(msg_block,IGRF_BADTYPE,IDL_MSG_LONGJMP,
                                    "of lon in ECDIP_Convert()");
        }

        if ((outargv[2]->type !=IDL_TYP_FLOAT) &&
            (outargv[2]->type !=IDL_TYP_DOUBLE)) {

            /* bad type */
            IDL_MessageFromBlock(msg_block,IGRF_BADTYPE,IDL_MSG_LONGJMP,
                                    "of height in ECDIP_Convert()");
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

            if (inverse) s=ecdip2geod(inlat,inlon,height+Re,out);
            else         s=geod2ecdip(inlat,inlon,height,out);
            latptr[n]=out[0];
            lonptr[n]=out[1];
            rptr[n]=out[2];
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

        if (inverse) s=ecdip2geod(inlat,inlon,height+Re,out);
        else         s=geod2ecdip(inlat,inlon,height,out);
        outlat=out[0];
        outlon=out[1];
        r=out[2];

        IDL_StoreScalar(outargv[3],IDL_TYP_DOUBLE,(IDL_ALLTYPES *) &outlat);
        IDL_StoreScalar(outargv[4],IDL_TYP_DOUBLE,(IDL_ALLTYPES *) &outlon);
        IDL_StoreScalar(outargv[5],IDL_TYP_DOUBLE,(IDL_ALLTYPES *) &r);
    }

    IDL_KWCleanup(IDL_KW_CLEAN);
    return (IDL_GettmpLong(s));
}



static IDL_VPTR IDLECDIP_MLT(int argc,IDL_VPTR *argv,char *argk) {

    int n;

    static IDL_LONG inverse;

    static IDL_KW_PAR kw_pars[]={IDL_KW_FAST_SCAN,
        {"INVERSE",IDL_TYP_LONG,1,IDL_KW_ZERO,0,IDL_CHARA(inverse)},
        {NULL}};

    IDL_KWCleanup(IDL_KW_MARK);
    IDL_KWGetParams(argc,argv,argk,kw_pars,NULL,1);

    if (argv[0]->flags & IDL_V_ARR) {
        int nval;
        IDL_VPTR vigrf;
        double *igrfptr=NULL;
        int yrtmp=0,motmp=0,dytmp=0,hrtmp=0,mttmp=0,sctmp=0;
        double mlon=0,mlt=0;

        union IDLIGRFPtr lon;
        union IDLIGRFPtr yr,mo,dy,hr,mt,sc;
        int lontyp=0,yrtyp=0,motyp=0,dytyp=0,hrtyp=0,mttyp=0,sctyp=0;

        lon.vptr=NULL;
        yr.vptr=NULL;

        nval=argv[0]->value.arr->n_elts;

        for (n=1;n<argc;n++) IDL_ENSURE_ARRAY(argv[n]);

        for (n=0;n<argc;n++) {
            if ((argv[n]->type !=IDL_TYP_INT) &&
                (argv[n]->type !=IDL_TYP_LONG) &&
                (argv[n]->type !=IDL_TYP_FLOAT) &&
                (argv[n]->type !=IDL_TYP_DOUBLE)) {

                /* bad type */
                char msg[256];
                sprintf(msg,"of argument %d in ECDIP_MLT()",n+1);
                IDL_MessageFromBlock(msg_block,IGRF_BADTYPE,IDL_MSG_LONGJMP,
                                        msg);
            }
        }

        for (n=0;n<argc;n++) {
            if (argv[n]->value.arr->n_elts !=nval)
                IDL_MessageFromBlock(msg_block,IGRF_MISMATCHELMS,IDL_MSG_LONGJMP,
                                        "in ECDIP_MLT()");
        }

        yr.vptr=(void *) argv[0]->value.arr->data;
        yrtyp=argv[0]->type;

        mo.vptr=(void *) argv[1]->value.arr->data;
        motyp=argv[1]->type;

        dy.vptr=(void *) argv[2]->value.arr->data;
        dytyp=argv[2]->type;

        hr.vptr=(void *) argv[3]->value.arr->data;
        hrtyp=argv[3]->type;

        mt.vptr=(void *) argv[4]->value.arr->data;
        mttyp=argv[4]->type;

        sc.vptr=(void *) argv[5]->value.arr->data;
        sctyp=argv[5]->type;

        if (argv[6]->flags & IDL_V_ARR) {
            if (argv[6]->value.arr->n_elts !=nval)
                IDL_MessageFromBlock(msg_block,IGRF_MISMATCHELMS,IDL_MSG_LONGJMP,
                                        "in ECDIP_MLT()");

            lon.vptr=(void *) argv[6]->value.arr->data;
            lontyp=argv[6]->type;
        } else mlon=IDL_LongScalar(argv[6]);

        igrfptr=(double *) IDL_MakeTempArray(IDL_TYP_DOUBLE,
                                            argv[0]->value.arr->n_dim,
                                            argv[0]->value.arr->dim,
                                            IDL_ARR_INI_ZERO,&vigrf);

        for (n=0;n<nval;n++) {
            switch (yrtyp) {
                case IDL_TYP_INT:
                    yrtmp=yr.iptr[n];
                    break;
                case IDL_TYP_LONG:
                    yrtmp=yr.lptr[n];
                    break;
                case IDL_TYP_FLOAT:
                    yrtmp=yr.fptr[n];
                    break;
                case IDL_TYP_DOUBLE:
                    yrtmp=yr.dptr[n];
                    break;
                default:
                    yrtmp=1970;
            }

            switch (motyp) {
                case IDL_TYP_INT:
                    motmp=mo.iptr[n];
                    break;
                case IDL_TYP_LONG:
                    motmp=mo.lptr[n];
                    break;
                case IDL_TYP_FLOAT:
                    motmp=mo.fptr[n];
                    break;
                case IDL_TYP_DOUBLE:
                    motmp=mo.dptr[n];
                    break;
                default:
                    motmp=1;
            }

            switch (dytyp) {
                case IDL_TYP_INT:
                    dytmp=dy.iptr[n];
                    break;
                case IDL_TYP_LONG:
                    dytmp=dy.lptr[n];
                    break;
                case IDL_TYP_FLOAT:
                    dytmp=dy.fptr[n];
                    break;
                case IDL_TYP_DOUBLE:
                    dytmp=dy.dptr[n];
                    break;
                default:
                    dytmp=1;
            }

            switch (hrtyp) {
                case IDL_TYP_INT:
                    hrtmp=hr.iptr[n];
                    break;
                case IDL_TYP_LONG:
                    hrtmp=hr.lptr[n];
                    break;
                case IDL_TYP_FLOAT:
                    hrtmp=hr.fptr[n];
                    break;
                case IDL_TYP_DOUBLE:
                    hrtmp=hr.dptr[n];
                    break;
                default:
                    hrtmp=0;
            }

            switch (mttyp) {
                case IDL_TYP_INT:
                    mttmp=mt.iptr[n];
                    break;
                case IDL_TYP_LONG:
                    mttmp=mt.lptr[n];
                    break;
                case IDL_TYP_FLOAT:
                    mttmp=mt.fptr[n];
                    break;
                case IDL_TYP_DOUBLE:
                    mttmp=mt.dptr[n];
                    break;
                default:
                    mttmp=0;
            }

            switch (sctyp) {
                case IDL_TYP_INT:
                    sctmp=sc.iptr[n];
                    break;
                case IDL_TYP_LONG:
                    sctmp=sc.lptr[n];
                    break;
                case IDL_TYP_FLOAT:
                    sctmp=sc.fptr[n];
                    break;
                case IDL_TYP_DOUBLE:
                    sctmp=sc.dptr[n];
                    break;
                default:
                    sctmp=0;
            }

            if (lon.vptr !=NULL) {
                switch (lontyp) {
                    case IDL_TYP_FLOAT:
                        mlon=lon.fptr[n];
                        break;
                    case IDL_TYP_DOUBLE:
                        mlon=lon.dptr[n];
                        break;
                    default:
                        mlon=0;
                }
            }

            if (inverse) mlt = inv_ecdip_mlt(yrtmp,motmp,dytmp,hrtmp,mttmp,sctmp,mlon);
            else         mlt = ecdip_mlt(yrtmp,motmp,dytmp,hrtmp,mttmp,sctmp,mlon);
            igrfptr[n]=mlt;
        }

        IDL_KWCleanup(IDL_KW_CLEAN);
        return (vigrf);

    } else {
        /* scalar */
        int yr,mo,dy,hr,mt,sc;
        double mlon,mlt;
        IDL_VPTR vigrf;

        for (n=0;n<argc;n++) IDL_ENSURE_SCALAR(argv[n]);

        yr=IDL_LongScalar(argv[0]);
        mo=IDL_LongScalar(argv[1]);
        dy=IDL_LongScalar(argv[2]);
        hr=IDL_LongScalar(argv[3]);
        mt=IDL_LongScalar(argv[4]);
        sc=IDL_LongScalar(argv[5]);

        mlon=IDL_DoubleScalar(argv[6]);

        if (inverse) mlt=inv_ecdip_mlt(yr,mo,dy,hr,mt,sc,mlon);
        else         mlt=ecdip_mlt(yr,mo,dy,hr,mt,sc,mlon);

        vigrf=IDL_Gettmp();
        vigrf->type=IDL_TYP_DOUBLE;
        vigrf->value.d=mlt;

        IDL_KWCleanup(IDL_KW_CLEAN);
        return (vigrf);
    }

    IDL_KWCleanup(IDL_KW_CLEAN);
    return (IDL_GettmpLong(0));
}


static IDL_VPTR IDLIGRF_compute(int argc,IDL_VPTR *argv) {

    int s;
    IDL_VPTR vbrtp;
    double *brtpptr=NULL;
    double rtptmp[3],brtptmp[3];

    union IDLIGRFPtr rtp;
    int rtptyp=0;

    rtp.vptr=NULL;

    IDL_ENSURE_ARRAY(argv[0]);

    if (argv[0]->value.arr->n_elts !=3) {
      /* wrong input array dimensions */
      IDL_MessageFromBlock(msg_block,IGRF_BADRTP,IDL_MSG_LONGJMP,
                           "in IGRF_compute()");
    }

    brtpptr=(double *) IDL_MakeTempArray(IDL_TYP_DOUBLE,
                                         argv[0]->value.arr->n_dim,
                                         argv[0]->value.arr->dim,
                                         IDL_ARR_INI_ZERO,&vbrtp);

    if ((argv[0]->type !=IDL_TYP_FLOAT) &&
        (argv[0]->type !=IDL_TYP_DOUBLE)) {

        /* bad type */
         char msg[256];
         sprintf(msg,"of argument in IGRF_compute()");
         IDL_MessageFromBlock(msg_block,IGRF_BADTYPE,IDL_MSG_LONGJMP,
                              msg);
    }

    rtp.vptr=(void *) argv[0]->value.arr->data;
    rtptyp=argv[0]->type;

    switch (rtptyp) {
        case IDL_TYP_FLOAT:
            rtptmp[0]=rtp.fptr[0];
            rtptmp[1]=rtp.fptr[1];
            rtptmp[2]=rtp.fptr[2];
            break;
        case IDL_TYP_DOUBLE:
            rtptmp[0]=rtp.dptr[0];
            rtptmp[1]=rtp.dptr[1];
            rtptmp[2]=rtp.dptr[2];
            break;
        default:
            rtptmp[0]=0;
            rtptmp[1]=0;
            rtptmp[2]=0;
    }

    s=IGRF_compute(rtptmp,brtptmp);
    brtpptr[0]=brtptmp[0];
    brtpptr[1]=brtptmp[1];
    brtpptr[2]=brtptmp[2];

    return vbrtp;
}


static IDL_VPTR IDLIGRFModelCall(int argc,IDL_VPTR *argv) {
  
  int s=0,n;
  IDL_VPTR vox,voy,voz;

  double dtval=2000,flat,flon,elev=180;
  double x,y,z;
  double *xptr,*yptr,*zptr;
 
  IDL_EXCLUDE_EXPR(argv[4]);
  IDL_EXCLUDE_EXPR(argv[5]);
  IDL_EXCLUDE_EXPR(argv[6]);

  if (argv[1]->flags & IDL_V_ARR) {
    union IDLIGRFPtr lat,lon,dte,elv;
    int lattyp=0,lontyp=0,dtetyp=0,elvtyp=0;
    int nval;

    lat.vptr=NULL;
    lon.vptr=NULL;
    dte.vptr=NULL;
    elv.vptr=NULL;

    IDL_ENSURE_ARRAY(argv[2]);
 
    nval=argv[1]->value.arr->n_elts;

    lat.vptr=(void *) argv[1]->value.arr->data;
    lattyp=argv[1]->type;

    if ( (argv[2]->value.arr->n_elts !=nval)) {

      /* array size mismatch */

      IDL_MessageFromBlock(msg_block,IGRF_MISMATCHELMS,IDL_MSG_LONGJMP,
                           "in IGRFModelCall()");
    
    }

    lon.vptr=(void *) argv[2]->value.arr->data;
    lontyp=argv[2]->type;


    if (argv[0]->flags & IDL_V_ARR) {
      if (argv[0]->value.arr->n_elts !=nval)
        IDL_MessageFromBlock(msg_block,IGRF_MISMATCHELMS,IDL_MSG_LONGJMP,
                           "in IGRFModelCall()");

      dte.vptr=(void *) argv[0]->value.arr->data;
      dtetyp=argv[0]->type;

    } else dtval=IDL_DoubleScalar(argv[0]);
 
    if (argv[3]->flags & IDL_V_ARR) {
      if (argv[3]->value.arr->n_elts !=nval)
        IDL_MessageFromBlock(msg_block,IGRF_MISMATCHELMS,IDL_MSG_LONGJMP,
                           "in IGRFModelCall()");

      elv.vptr=(void *) argv[3]->value.arr->data;
      elvtyp=argv[3]->type;

    } else elev=IDL_DoubleScalar(argv[3]);
 
    if ((argv[1]->type !=IDL_TYP_FLOAT) && 
        (argv[1]->type !=IDL_TYP_DOUBLE)) {

      /* bad type */

      IDL_MessageFromBlock(msg_block,IGRF_BADTYPE,IDL_MSG_LONGJMP,
                           "of lat in IGRFModelCall()");

    }
   
    if ((argv[2]->type !=IDL_TYP_FLOAT) && 
        (argv[2]->type !=IDL_TYP_DOUBLE)) {

      /* bad type */

      IDL_MessageFromBlock(msg_block,IGRF_BADTYPE,IDL_MSG_LONGJMP,
                           "of lon in IGRFModelCall()");

    }
   
    if ((argv[3]->type !=IDL_TYP_FLOAT) && 
        (argv[3]->type !=IDL_TYP_DOUBLE)) {

      /* bad type */


      IDL_MessageFromBlock(msg_block,IGRF_BADTYPE,IDL_MSG_LONGJMP,
                           "of height in IGRFModelCall()");

    }
   

    xptr=(double *) IDL_MakeTempArray(IDL_TYP_DOUBLE,
                                        argv[1]->value.arr->n_dim,
                                        argv[1]->value.arr->dim,
					IDL_ARR_INI_ZERO,&vox);
    yptr=(double *) IDL_MakeTempArray(IDL_TYP_DOUBLE,
                                        argv[1]->value.arr->n_dim,
                                        argv[1]->value.arr->dim,
					IDL_ARR_INI_ZERO,&voy);

    zptr=(double *) IDL_MakeTempArray(IDL_TYP_DOUBLE,
                                      argv[1]->value.arr->n_dim,
                                      argv[1]->value.arr->dim,
				      IDL_ARR_INI_ZERO,&voz);

    for (n=0;n<nval;n++) {

     if (dte.vptr !=NULL) {
       switch (dtetyp) {
         case IDL_TYP_BYTE:
           dtval=dte.bptr[n];
           break;
         case IDL_TYP_INT:
           dtval=dte.iptr[n];
           break;
         case IDL_TYP_LONG:
           dtval=dte.lptr[n];
           break;
         case IDL_TYP_FLOAT:
           dtval=dte.fptr[n];
           break;
         case IDL_TYP_DOUBLE:
           dtval=dte.dptr[n];
           break;
         default:
          dtval=2000;
       }
     }



      switch (lattyp) {
        case IDL_TYP_FLOAT:
          flat=lat.fptr[n];
          break;
        case IDL_TYP_DOUBLE:
          flat=lat.dptr[n];
          break;
        default:
          flat=0;
      }

     switch (lontyp) {
        case IDL_TYP_FLOAT:
          flon=lon.fptr[n];
          break;
        case IDL_TYP_DOUBLE:
          flon=lon.dptr[n];
          break;
        default:
          flon=0;
      }

     if (elv.vptr !=NULL) {
       switch (elvtyp) {
         case IDL_TYP_BYTE:
           elev=elv.bptr[n];
           break;
         case IDL_TYP_INT:
           elev=elv.iptr[n];
           break;
         case IDL_TYP_LONG:
           elev=elv.lptr[n];
           break;
         case IDL_TYP_FLOAT:
           elev=elv.fptr[n];
           break;
         case IDL_TYP_DOUBLE:
           elev=elv.dptr[n];
           break;
         default:
          elev=180;
       }
     }

      s=IGRFCall(dtval,flat,flon,elev,&xptr[n],&yptr[n],&zptr[n]);
    }

    IDL_VarCopy(vox,argv[4]);
    IDL_VarCopy(voy,argv[5]);
    IDL_VarCopy(voz,argv[6]);

  } else {
    /* scalar */
    IDL_ENSURE_SCALAR(argv[0]);
    IDL_ENSURE_SCALAR(argv[2]);
    IDL_ENSURE_SCALAR(argv[3]);

    dtval=IDL_DoubleScalar(argv[0]);
    flat=IDL_DoubleScalar(argv[1]);
    flon=IDL_DoubleScalar(argv[2]);
    elev=IDL_DoubleScalar(argv[3]);

    s=IGRFCall(dtval,flat,flon,elev,&x,&y,&z);
    IDL_StoreScalar(argv[4],IDL_TYP_DOUBLE,(IDL_ALLTYPES *) &x);
    IDL_StoreScalar(argv[5],IDL_TYP_DOUBLE,(IDL_ALLTYPES *) &y);
    IDL_StoreScalar(argv[6],IDL_TYP_DOUBLE,(IDL_ALLTYPES *) &z);
  }
 
  return (IDL_GettmpLong(s));

}


int IDL_Load(void) {

  static IDL_SYSFUN_DEF2 fnaddr[]={
    { {IDLIGRFModelCall},"IGRFMODELCALL",7,7,0,0},
    { {IDLIGRF_GetDateTime},"IGRF_GETDATETIME",1,1,IDL_SYSFUN_DEF_F_KEYWORDS,0},
    { {IDLIGRF_SetDateTime},"IGRF_SETDATETIME",1,6,0,0},
    { {IDLIGRF_SetNow},"IGRF_SETNOW",0,0,0,0},
    { {IDLIGRF_Tilt},"IGRF_TILT",6,6,0,0},
    { {IDLIGRF_compute},"IGRF_COMPUTE",1,1,0,0},
    { {IDLECDIP_Convert},"ECDIP_CONVERT",6,6,IDL_SYSFUN_DEF_F_KEYWORDS,0},
    { {IDLECDIP_MLT},"ECDIP_MLT",7,7,IDL_SYSFUN_DEF_F_KEYWORDS,0},
  };


  if (!(msg_block = IDL_MessageDefineBlock("igrf",
                    IDL_CARRAY_ELTS(msg_arr), msg_arr)))
    return IDL_FALSE;

  return IDL_SysRtnAdd(fnaddr,TRUE,IDL_CARRAY_ELTS(fnaddr));

}
