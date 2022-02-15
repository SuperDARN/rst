/* mltdlm.c
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
#include "mlt.h"
#include "mlt_v2.h"
#include "aacgmlib_v2.h"

#define MLT_ERROR 0
#define MLT_BADTYPE -1
#define MLT_MISMATCHDIMS -2
#define MLT_MISMATCHELMS -3


static IDL_MSG_DEF msg_arr[] =
    {
        {  "MLT_ERROR",   "%NError: %s." },
        {  "MLT_BADTYPE","%NBad argument types %s"},
        {  "MLT_MISMATCHDIMS","%NMismatched array dimensions %s"},
        {  "MLT_MISMATCHELMS","%NNumber of array elements do not match %s"},
    };

static IDL_MSG_BLOCK msg_block;


union IDLMLTPtr {
    unsigned char *bptr;
    short *iptr;
    int *lptr;
    float *fptr;
    double *dptr;
    void *vptr;
};



static IDL_VPTR IDLMLTConvertYMDHMS(int argc,IDL_VPTR *argv) {

    int n;

    if (argv[0]->flags & IDL_V_ARR) {
        int nval;
        IDL_VPTR vmlt;
        double *mltptr=NULL;
        int yrtmp=0,motmp=0,dytmp=0,hrtmp=0,mttmp=0,sctmp=0;
        double mlon=0,mlt=0;

        union IDLMLTPtr lon;
        union IDLMLTPtr yr,mo,dy,hr,mt,sc;
        int lontyp=0,yrtyp=0,motyp=0,dytyp=0,hrtyp=0,mttyp=0,sctyp=0;

        lon.vptr=NULL;
        yr.vptr=NULL;

        nval=argv[0]->value.arr->n_elts;

        for (n=1;n<6;n++) IDL_ENSURE_ARRAY(argv[n]);

        for (n=0;n<6;n++) {
            if ((argv[n]->type !=IDL_TYP_INT) &&
                (argv[n]->type !=IDL_TYP_LONG) &&
                (argv[n]->type !=IDL_TYP_FLOAT) &&
                (argv[n]->type !=IDL_TYP_DOUBLE)) {

                /* bad type */
                char msg[256];
                sprintf(msg,"of argument %d in MLTConvertYMDHMS()",n+1);
                IDL_MessageFromBlock(msg_block,MLT_BADTYPE,IDL_MSG_LONGJMP,
                                        msg);
            }
        }

        if ((argv[6]->type !=IDL_TYP_FLOAT) &&
            (argv[6]->type !=IDL_TYP_DOUBLE)) {

            /* bad type */
            IDL_MessageFromBlock(msg_block,MLT_BADTYPE,IDL_MSG_LONGJMP,
                                    "of lon in MLTConvertYMDHMS()");
        }

        for (n=0;n<6;n++) {
            if (argv[n]->value.arr->n_elts !=nval) 
                IDL_MessageFromBlock(msg_block,MLT_MISMATCHELMS,IDL_MSG_LONGJMP,
                                        "in MLTConvertYMDHMS()");
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
                IDL_MessageFromBlock(msg_block,MLT_MISMATCHELMS,IDL_MSG_LONGJMP,
                                        "in MLTConvertYMDHMS()");

            lon.vptr=(void *) argv[6]->value.arr->data;
            lontyp=argv[6]->type;
        } else mlon=IDL_LongScalar(argv[6]);

        mltptr=(double *) IDL_MakeTempArray(IDL_TYP_DOUBLE,
                                            argv[0]->value.arr->n_dim,
                                            argv[0]->value.arr->dim,
                                            IDL_ARR_INI_ZERO,&vmlt);

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

            mlt=MLTConvertYMDHMS(yrtmp,motmp,dytmp,hrtmp,mttmp,sctmp,mlon);
            mltptr[n]=mlt;
        }

        return (vmlt);

    } else {
        /* scalar */
        int yr,mo,dy,hr,mt,sc;
        double mlon,mlt;
        IDL_VPTR vmlt;

        IDL_ENSURE_SCALAR(argv[0]);
        IDL_ENSURE_SCALAR(argv[1]);
        IDL_ENSURE_SCALAR(argv[2]);
        IDL_ENSURE_SCALAR(argv[3]);
        IDL_ENSURE_SCALAR(argv[4]);
        IDL_ENSURE_SCALAR(argv[5]);

        yr=IDL_LongScalar(argv[0]);
        mo=IDL_LongScalar(argv[1]);
        dy=IDL_LongScalar(argv[2]);
        hr=IDL_LongScalar(argv[3]);
        mt=IDL_LongScalar(argv[4]);
        sc=IDL_LongScalar(argv[5]);

        mlon=IDL_DoubleScalar(argv[6]);

        mlt=MLTConvertYMDHMS(yr,mo,dy,hr,mt,sc,mlon);

        vmlt=IDL_Gettmp();
        vmlt->type=IDL_TYP_DOUBLE;
        vmlt->value.d=mlt;

        return (vmlt);
    }

    return (IDL_GettmpLong(0));
}



static IDL_VPTR IDLMLTConvertYrsec(int argc,IDL_VPTR *argv) {

    int n;

    if (argv[1]->flags & IDL_V_ARR) {
        int nval;
        IDL_VPTR vmlt;
        double *mltptr=NULL;
        int yrtmp=0,yrsec=0;
        double mlon=0,mlt=0;

        union IDLMLTPtr lon;
        union IDLMLTPtr yr,yrs;
        int lontyp=0,yrtyp=0,yrstyp;

        lon.vptr=NULL;
        yr.vptr=NULL;

        nval=argv[1]->value.arr->n_elts;

        yrs.vptr=(void *) argv[1]->value.arr->data;
        yrstyp=argv[1]->type;

        if ((argv[1]->type !=IDL_TYP_LONG) &&
            (argv[1]->type !=IDL_TYP_FLOAT) &&
            (argv[1]->type !=IDL_TYP_DOUBLE)) {

            /* bad type */
            IDL_MessageFromBlock(msg_block,MLT_BADTYPE,IDL_MSG_LONGJMP,
                                    "of yrsec in MLTConvertYrsec()");
        }

        if ((argv[2]->type !=IDL_TYP_FLOAT) &&
            (argv[2]->type !=IDL_TYP_DOUBLE)) {

            /* bad type */
            IDL_MessageFromBlock(msg_block,MLT_BADTYPE,IDL_MSG_LONGJMP,
                                    "of lon in MLTConvertYrsec()");
        }

        if (argv[0]->flags & IDL_V_ARR) {
            if (argv[0]->value.arr->n_elts !=nval) 
                IDL_MessageFromBlock(msg_block,MLT_MISMATCHELMS,IDL_MSG_LONGJMP,
                                        "in MLTConvertYrsec()");

            yr.vptr=(void *) argv[0]->value.arr->data;
            yrtyp=argv[0]->type;
        } else yrtmp=IDL_LongScalar(argv[0]);

        if (argv[2]->flags & IDL_V_ARR) {
            if (argv[2]->value.arr->n_elts !=nval) 
                IDL_MessageFromBlock(msg_block,MLT_MISMATCHELMS,IDL_MSG_LONGJMP,
                                        "in MLTConvertYrsec()");

            lon.vptr=(void *) argv[2]->value.arr->data;
            lontyp=argv[2]->type;
        } else mlon=IDL_LongScalar(argv[2]);

        mltptr=(double *) IDL_MakeTempArray(IDL_TYP_DOUBLE,
                                            argv[1]->value.arr->n_dim,
                                            argv[1]->value.arr->dim,
                                            IDL_ARR_INI_ZERO,&vmlt);

        for (n=0;n<nval;n++) {
            switch (yrstyp) {
                case IDL_TYP_INT:
                    yrsec=yrs.iptr[n];
                    break;
                case IDL_TYP_LONG:
                    yrsec=yrs.lptr[n];
                    break;
                case IDL_TYP_FLOAT:
                    yrsec=yrs.fptr[n];
                    break;
                case IDL_TYP_DOUBLE:
                    yrsec=yrs.dptr[n];
                    break;
                default:
                    yrsec=0;
            }

            if (yr.vptr !=NULL) {
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

            mlt=MLTConvertYrsec(yrtmp,yrsec,mlon);
            mltptr[n]=mlt;
        }

        return (vmlt);

    } else {
        /* scalar */
        int yr,yrsec;
        double mlon,mlt;
        IDL_VPTR vmlt;

        IDL_ENSURE_SCALAR(argv[0]);
        IDL_ENSURE_SCALAR(argv[1]);
        IDL_ENSURE_SCALAR(argv[2]);

        yr=IDL_LongScalar(argv[0]);
        yrsec=IDL_LongScalar(argv[1]);
        mlon=IDL_DoubleScalar(argv[2]);

        mlt=MLTConvertYrsec(yr,yrsec,mlon);

        vmlt=IDL_Gettmp();
        vmlt->type=IDL_TYP_DOUBLE;
        vmlt->value.d=mlt;

        return (vmlt);
    }

    return (IDL_GettmpLong(0));
}



static IDL_VPTR IDLMLTConvertEpoch(int argc,IDL_VPTR *argv) {

    int n;

    if (argv[0]->flags & IDL_V_ARR) {
        int nval;
        IDL_VPTR vmlt;
        double *mltptr=NULL;
        double epoch=0,mlon=0,mlt=0;

        union IDLMLTPtr lon;
        int lontyp=0;

        lon.vptr=NULL;

        nval=argv[0]->value.arr->n_elts;

        if (argv[0]->type !=IDL_TYP_DOUBLE) {

            /* bad type */
            IDL_MessageFromBlock(msg_block,MLT_BADTYPE,IDL_MSG_LONGJMP,
                                    "of epoch in MLTConvertEpoch()");
        }

        if ((argv[1]->type !=IDL_TYP_FLOAT) &&
            (argv[1]->type !=IDL_TYP_DOUBLE)) {

            /* bad type */
            IDL_MessageFromBlock(msg_block,MLT_BADTYPE,IDL_MSG_LONGJMP,
                                    "of lon in MLTConvertEpoch()");
        }

        if (argv[1]->flags & IDL_V_ARR) {
            if (argv[1]->value.arr->n_elts !=nval) 
                IDL_MessageFromBlock(msg_block,MLT_MISMATCHELMS,IDL_MSG_LONGJMP,
                                        "in MLTConvertEpoch()");

            lon.vptr=(void *) argv[1]->value.arr->data;
            lontyp=argv[1]->type;
        } else mlon=IDL_LongScalar(argv[1]);

        mltptr=(double *) IDL_MakeTempArray(IDL_TYP_DOUBLE,
                                            argv[0]->value.arr->n_dim,
                                            argv[0]->value.arr->dim,
                                            IDL_ARR_INI_ZERO,&vmlt);

        for (n=0;n<nval;n++) {
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

            epoch=((double *) argv[0]->value.arr->data)[n];
            mlt=MLTConvertEpoch(epoch,mlon);
            mltptr[n]=mlt;
        }

        return (vmlt);

    } else {
        /* scalar */
        double epoch,mlon,mlt;
        IDL_VPTR vmlt;

        IDL_ENSURE_SCALAR(argv[0]);
        IDL_ENSURE_SCALAR(argv[1]);

        epoch=IDL_DoubleScalar(argv[0]);
        mlon=IDL_DoubleScalar(argv[1]);

        mlt=MLTConvertEpoch(epoch,mlon);

        vmlt=IDL_Gettmp();
        vmlt->type=IDL_TYP_DOUBLE;
        vmlt->value.d=mlt;

        return (vmlt);
    }

    return (IDL_GettmpLong(0));
}



static IDL_VPTR IDLMLTConvertYMDHMS_v2(int argc,IDL_VPTR *argv,char *argk) {

    int n;

    static IDL_LONG inverse;

    static IDL_KW_PAR kw_pars[]={IDL_KW_FAST_SCAN,
        {"MLT2MLON",IDL_TYP_LONG,1,IDL_KW_ZERO,0,IDL_CHARA(inverse)},
        {NULL}};

    IDL_KWCleanup(IDL_KW_MARK);
    IDL_KWGetParams(argc,argv,argk,kw_pars,NULL,1);

    if (argv[0]->flags & IDL_V_ARR) {
        int nval;
        IDL_VPTR vmlt;
        double *mltptr=NULL;
        int yrtmp=0,motmp=0,dytmp=0,hrtmp=0,mttmp=0,sctmp=0;
        double mlon=0,mlt=0;

        union IDLMLTPtr lon;
        union IDLMLTPtr yr,mo,dy,hr,mt,sc;
        int lontyp=0,yrtyp=0,motyp=0,dytyp=0,hrtyp=0,mttyp=0,sctyp=0;

        lon.vptr=NULL;
        yr.vptr=NULL;

        nval=argv[0]->value.arr->n_elts;

        for (n=1;n<6;n++) IDL_ENSURE_ARRAY(argv[n]);

        for (n=0;n<6;n++) {
            if ((argv[n]->type !=IDL_TYP_INT) &&
                (argv[n]->type !=IDL_TYP_LONG) &&
                (argv[n]->type !=IDL_TYP_FLOAT) &&
                (argv[n]->type !=IDL_TYP_DOUBLE)) {

                /* bad type */
                char msg[256];
                sprintf(msg,"of argument %d in MLTConvertYMDHMS_v2()",n+1);
                IDL_MessageFromBlock(msg_block,MLT_BADTYPE,IDL_MSG_LONGJMP,
                                        msg);
            }
        }

        if ((argv[6]->type !=IDL_TYP_FLOAT) &&
            (argv[6]->type !=IDL_TYP_DOUBLE)) {

            /* bad type */
            IDL_MessageFromBlock(msg_block,MLT_BADTYPE,IDL_MSG_LONGJMP,
                                    "of lon in MLTConvertYMDHMS_v2()");
        }

        for (n=0;n<6;n++) {
            if (argv[n]->value.arr->n_elts !=nval) 
                IDL_MessageFromBlock(msg_block,MLT_MISMATCHELMS,IDL_MSG_LONGJMP,
                                        "in MLTConvertYMDHMS_v2()");
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
                IDL_MessageFromBlock(msg_block,MLT_MISMATCHELMS,IDL_MSG_LONGJMP,
                                        "in MLTConvertYMDHMS_v2()");

            lon.vptr=(void *) argv[6]->value.arr->data;
            lontyp=argv[6]->type;
        } else mlon=IDL_LongScalar(argv[6]);

        mltptr=(double *) IDL_MakeTempArray(IDL_TYP_DOUBLE,
                                            argv[0]->value.arr->n_dim,
                                            argv[0]->value.arr->dim,
                                            IDL_ARR_INI_ZERO,&vmlt);

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

            if (inverse) mlt=inv_MLTConvertYMDHMS_v2(yrtmp,motmp,dytmp,hrtmp,mttmp,sctmp,mlon);
            else         mlt=MLTConvertYMDHMS_v2(yrtmp,motmp,dytmp,hrtmp,mttmp,sctmp,mlon);
            mltptr[n]=mlt;
        }

        IDL_KWCleanup(IDL_KW_CLEAN);
        return (vmlt);

    } else {
        /* scalar */
        int yr,mo,dy,hr,mt,sc;
        double mlon,mlt;
        IDL_VPTR vmlt;

        IDL_ENSURE_SCALAR(argv[0]);
        IDL_ENSURE_SCALAR(argv[1]);
        IDL_ENSURE_SCALAR(argv[2]);
        IDL_ENSURE_SCALAR(argv[3]);
        IDL_ENSURE_SCALAR(argv[4]);
        IDL_ENSURE_SCALAR(argv[5]);

        yr=IDL_LongScalar(argv[0]);
        mo=IDL_LongScalar(argv[1]);
        dy=IDL_LongScalar(argv[2]);
        hr=IDL_LongScalar(argv[3]);
        mt=IDL_LongScalar(argv[4]);
        sc=IDL_LongScalar(argv[5]);

        mlon=IDL_DoubleScalar(argv[6]);

        if (inverse) mlt=inv_MLTConvertYMDHMS_v2(yr,mo,dy,hr,mt,sc,mlon);
        else         mlt=MLTConvertYMDHMS_v2(yr,mo,dy,hr,mt,sc,mlon);

        vmlt=IDL_Gettmp();
        vmlt->type=IDL_TYP_DOUBLE;
        vmlt->value.d=mlt;

        IDL_KWCleanup(IDL_KW_CLEAN);
        return (vmlt);
    }

    IDL_KWCleanup(IDL_KW_CLEAN);
    return (IDL_GettmpLong(0));
}


static IDL_VPTR IDLMLTConvertYrsec_v2(int argc,IDL_VPTR *argv,char *argk) {

    int n;

    static IDL_LONG inverse;

    static IDL_KW_PAR kw_pars[]={IDL_KW_FAST_SCAN,
        {"MLT2MLON",IDL_TYP_LONG,1,IDL_KW_ZERO,0,IDL_CHARA(inverse)},
        {NULL}};

    IDL_KWCleanup(IDL_KW_MARK);
    IDL_KWGetParams(argc,argv,argk,kw_pars,NULL,1);

    if (argv[1]->flags & IDL_V_ARR) {
        int nval;
        IDL_VPTR vmlt;
        double *mltptr=NULL;
        int yrtmp=0,yrsec=0;
        double mlon=0,mlt=0;

        union IDLMLTPtr lon;
        union IDLMLTPtr yr,yrs;
        int lontyp=0,yrtyp=0,yrstyp;

        lon.vptr=NULL;
        yr.vptr=NULL;

        nval=argv[1]->value.arr->n_elts;

        yrs.vptr=(void *) argv[1]->value.arr->data;
        yrstyp=argv[1]->type;

        if ((argv[1]->type !=IDL_TYP_LONG) &&
            (argv[1]->type !=IDL_TYP_FLOAT) &&
            (argv[1]->type !=IDL_TYP_DOUBLE)) {

            /* bad type */
            IDL_MessageFromBlock(msg_block,MLT_BADTYPE,IDL_MSG_LONGJMP,
                                    "of yrsec in MLTConvertYrsec_v2()");
        }

        if ((argv[2]->type !=IDL_TYP_FLOAT) &&
            (argv[2]->type !=IDL_TYP_DOUBLE)) {

            /* bad type */
            IDL_MessageFromBlock(msg_block,MLT_BADTYPE,IDL_MSG_LONGJMP,
                                    "of lon in MLTConvertYrsec_v2()");
        }

        if (argv[0]->flags & IDL_V_ARR) {
            if (argv[0]->value.arr->n_elts !=nval) 
                IDL_MessageFromBlock(msg_block,MLT_MISMATCHELMS,IDL_MSG_LONGJMP,
                                        "in MLTConvertYrsec_v2()");

            yr.vptr=(void *) argv[0]->value.arr->data;
            yrtyp=argv[0]->type;
        } else yrtmp=IDL_LongScalar(argv[0]);

        if (argv[2]->flags & IDL_V_ARR) {
            if (argv[2]->value.arr->n_elts !=nval) 
                IDL_MessageFromBlock(msg_block,MLT_MISMATCHELMS,IDL_MSG_LONGJMP,
                                        "in MLTConvertYrsec_v2()");

            lon.vptr=(void *) argv[2]->value.arr->data;
            lontyp=argv[2]->type;
        } else mlon=IDL_LongScalar(argv[2]);

        mltptr=(double *) IDL_MakeTempArray(IDL_TYP_DOUBLE,
                                            argv[1]->value.arr->n_dim,
                                            argv[1]->value.arr->dim,
                                            IDL_ARR_INI_ZERO,&vmlt);

        for (n=0;n<nval;n++) {
            switch (yrstyp) {
                case IDL_TYP_INT:
                    yrsec=yrs.iptr[n];
                    break;
                case IDL_TYP_LONG:
                    yrsec=yrs.lptr[n];
                    break;
                case IDL_TYP_FLOAT:
                    yrsec=yrs.fptr[n];
                    break;
                case IDL_TYP_DOUBLE:
                    yrsec=yrs.dptr[n];
                    break;
                default:
                    yrsec=0;
            }

            if (yr.vptr !=NULL) {
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

            if (inverse) mlt=inv_MLTConvertYrsec_v2(yrtmp,yrsec,mlon);
            else         mlt=MLTConvertYrsec_v2(yrtmp,yrsec,mlon);
            mltptr[n]=mlt;
        }

        IDL_KWCleanup(IDL_KW_CLEAN);
        return (vmlt);

    } else {
        /* scalar */
        int yr,yrsec;
        double mlon,mlt;
        IDL_VPTR vmlt;

        IDL_ENSURE_SCALAR(argv[0]);
        IDL_ENSURE_SCALAR(argv[1]);
        IDL_ENSURE_SCALAR(argv[2]);

        yr=IDL_LongScalar(argv[0]);
        yrsec=IDL_LongScalar(argv[1]);
        mlon=IDL_DoubleScalar(argv[2]);

        if (inverse) mlt=inv_MLTConvertYrsec_v2(yr,yrsec,mlon);
        else         mlt=MLTConvertYrsec_v2(yr,yrsec,mlon);

        vmlt=IDL_Gettmp();
        vmlt->type=IDL_TYP_DOUBLE;
        vmlt->value.d=mlt;

        IDL_KWCleanup(IDL_KW_CLEAN);
        return (vmlt);
    }

    IDL_KWCleanup(IDL_KW_CLEAN);
    return (IDL_GettmpLong(0));
}


static IDL_VPTR IDLMLTConvertEpoch_v2(int argc,IDL_VPTR *argv,char *argk) {

    int n;

    static IDL_LONG inverse;

    static IDL_KW_PAR kw_pars[]={IDL_KW_FAST_SCAN,
        {"MLT2MLON",IDL_TYP_LONG,1,IDL_KW_ZERO,0,IDL_CHARA(inverse)},
        {NULL}};

    IDL_KWCleanup(IDL_KW_MARK);
    IDL_KWGetParams(argc,argv,argk,kw_pars,NULL,1);

    if (argv[0]->flags & IDL_V_ARR) {
        int nval;
        IDL_VPTR vmlt;
        double *mltptr=NULL;
        double epoch=0,mlon=0,mlt=0;

        union IDLMLTPtr lon;
        int lontyp=0;

        lon.vptr=NULL;

        nval=argv[0]->value.arr->n_elts;

        if (argv[0]->type !=IDL_TYP_DOUBLE) {

            /* bad type */
            IDL_MessageFromBlock(msg_block,MLT_BADTYPE,IDL_MSG_LONGJMP,
                                    "of epoch in MLTConvertEpoch_v2()");
        }

        if ((argv[1]->type !=IDL_TYP_FLOAT) && 
            (argv[1]->type !=IDL_TYP_DOUBLE)) {

            /* bad type */
            IDL_MessageFromBlock(msg_block,MLT_BADTYPE,IDL_MSG_LONGJMP,
                                    "of lon in MLTConvertEpoch_v2()");
        }

        if (argv[1]->flags & IDL_V_ARR) {
            if (argv[1]->value.arr->n_elts !=nval) 
                IDL_MessageFromBlock(msg_block,MLT_MISMATCHELMS,IDL_MSG_LONGJMP,
                                        "in MLTConvertEpoch_v2()");

            lon.vptr=(void *) argv[1]->value.arr->data;
            lontyp=argv[1]->type;
        } else mlon=IDL_LongScalar(argv[1]);

        mltptr=(double *) IDL_MakeTempArray(IDL_TYP_DOUBLE,
                                            argv[0]->value.arr->n_dim,
                                            argv[0]->value.arr->dim,
                                            IDL_ARR_INI_ZERO,&vmlt);

        for (n=0;n<nval;n++) {
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

            epoch=((double *) argv[0]->value.arr->data)[n];
            if (inverse) mlt=inv_MLTConvertEpoch_v2(epoch,mlon);
            else         mlt=MLTConvertEpoch_v2(epoch,mlon);
            mltptr[n]=mlt;
        }

        IDL_KWCleanup(IDL_KW_CLEAN);
        return (vmlt);

    } else {
        /* scalar */
        double epoch,mlon,mlt;
        IDL_VPTR vmlt;

        IDL_ENSURE_SCALAR(argv[0]);
        IDL_ENSURE_SCALAR(argv[1]);

        epoch=IDL_DoubleScalar(argv[0]);
        mlon=IDL_DoubleScalar(argv[1]);

        if (inverse) mlt=inv_MLTConvertEpoch_v2(epoch,mlon);
        else         mlt=MLTConvertEpoch_v2(epoch,mlon);

        vmlt=IDL_Gettmp();
        vmlt->type=IDL_TYP_DOUBLE;
        vmlt->value.d=mlt;

        IDL_KWCleanup(IDL_KW_CLEAN);
        return (vmlt);
    }

    IDL_KWCleanup(IDL_KW_CLEAN);
    return (IDL_GettmpLong(0));
}



int IDL_Load(void) {

    static IDL_SYSFUN_DEF2 fnaddr[]={
        { {IDLMLTConvertEpoch},"MLTCONVERTEPOCH",2,2,0,0},
        { {IDLMLTConvertYrsec},"MLTCONVERTYRSEC",3,3,0,0},
        { {IDLMLTConvertYMDHMS},"MLTCONVERTYMDHMS",7,7,0,0},
        { {IDLMLTConvertEpoch_v2},"MLTCONVERTEPOCH_V2",2,2,IDL_SYSFUN_DEF_F_KEYWORDS,0},
        { {IDLMLTConvertYrsec_v2},"MLTCONVERTYRSEC_V2",3,3,IDL_SYSFUN_DEF_F_KEYWORDS,0},
        { {IDLMLTConvertYMDHMS_v2},"MLTCONVERTYMDHMS_V2",7,7,IDL_SYSFUN_DEF_F_KEYWORDS,0},
    };

    if (!(msg_block = IDL_MessageDefineBlock("mlt",
                      IDL_CARRAY_ELTS(msg_arr), msg_arr)))
        return IDL_FALSE;

    return IDL_SysRtnAdd(fnaddr,TRUE,IDL_CARRAY_ELTS(fnaddr));
}
