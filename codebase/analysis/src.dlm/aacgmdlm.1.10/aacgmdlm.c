/* aacgmdlm.c
   =========== 
   Author R.J.Barnes
*/

/*
 $Licence$
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

#define AACGM_ERROR 0
#define AACGM_MISMATCHDIMS -1
#define AACGM_MISMATCHELMS -2
#define AACGM_BADTYPE -3

static IDL_MSG_DEF msg_arr[] =
  {
    {  "AACGM_ERROR",   "%NError: %s." },
    {  "AACGM_MISMATCHDIMS","%NMismatched array dimensions %s"},
    {  "AACGM_MISMATCHELMS","%NNumber of array elements do not match %s"},
    {  "AACGM_BADTYPE","%NArrays of floating point type are allowed %s"},
  };

static IDL_MSG_BLOCK msg_block;


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
  int outargc;
  IDL_VPTR outargv[6];
  IDL_VPTR volat,volon,vr;

  double inlat,inlon,height;
  double outlat,outlon,r;
  double *latptr,*lonptr,*rptr;

  static IDL_LONG geo;

  static IDL_KW_PAR kw_pars[]={IDL_KW_FAST_SCAN,
			       {"GEO",IDL_TYP_LONG,1,
                                IDL_KW_ZERO,0,
                                IDL_CHARA(geo)},
				 {NULL}};

  IDL_KWCleanup(IDL_KW_MARK);
  outargc=IDL_KWGetParams(argc,argv,argk,kw_pars,outargv,1);
 
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
      if (outargv[0]->type==IDL_TYP_FLOAT) inlat=
         ((float *) outargv[0]->value.arr->data)[n];
      else inlat=((double *) outargv[0]->value.arr->data)[n];
      if (outargv[1]->type==IDL_TYP_FLOAT) inlon=
         ((float *) outargv[1]->value.arr->data)[n];
      else inlon=((double *) outargv[1]->value.arr->data)[n];
      if (outargv[2]->type==IDL_TYP_FLOAT) height=
         ((float *) outargv[2]->value.arr->data)[n];
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



int IDL_Load(void) {

  static IDL_SYSFUN_DEF2 fnaddr[]={
    { IDLAACGMLoadCoef,"AACGMLOADCOEF",1,1,0,0},
    { IDLAACGMConvert,"AACGMCONVERT",6,6,IDL_SYSFUN_DEF_F_KEYWORDS,0},
   
  };


  if (!(msg_block = IDL_MessageDefineBlock("aacgm",
                    IDL_CARRAY_ELTS(msg_arr), msg_arr)))
    return IDL_FALSE;

  return IDL_SysRtnAdd(fnaddr,TRUE,IDL_CARRAY_ELTS(fnaddr));

}
