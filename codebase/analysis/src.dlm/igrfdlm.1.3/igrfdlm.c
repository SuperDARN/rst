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
#include "igrfcall.h"

#define IGRF_ERROR 0
#define IGRF_MISMATCHDIMS -1
#define IGRF_MISMATCHELMS -2
#define IGRF_BADTYPE -3

static IDL_MSG_DEF msg_arr[] =
  {
    {  "IGRF_ERROR",   "%NError: %s." },
    {  "IGRF_MISMATCHDIMS","%NMismatched array dimensions %s"},
    {  "IGRF_MISMATCHELMS","%NNumber of array elements do not match %s"},
    {  "IGRF_BADTYPE","%NArrays of floating point type are allowed %s"},
  };

static IDL_MSG_BLOCK msg_block;

union IGRFIDLPtr {
  unsigned char *bptr;
  short *iptr;
  int *lptr;
  float *fptr;
  double *dptr;
  void *vptr;
};



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
    union IGRFIDLPtr lat,lon,dte,elv;
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
   
  };


  if (!(msg_block = IDL_MessageDefineBlock("igrf",
                    IDL_CARRAY_ELTS(msg_arr), msg_arr)))
    return IDL_FALSE;

  return IDL_SysRtnAdd(fnaddr,TRUE,IDL_CARRAY_ELTS(fnaddr));

}
