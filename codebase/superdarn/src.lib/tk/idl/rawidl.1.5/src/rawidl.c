/* rawidl.c
   ======== 
   Author R.J.Barnes
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
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
#include "rawdata.h"
#include "rprmidl.h"
#include "rawidl.h"

void IDLCopyRawDataFromIDL(int nrang,int mplgs,int xcf,struct RawIDLData *iraw,
                           struct RawData *raw) {

  int n,x;

  float acftmp[MAX_RANGE*LAG_SIZE*2];
  float xcftmp[MAX_RANGE*LAG_SIZE*2];

  raw->revision.major=iraw->revision.major;
  raw->revision.minor=iraw->revision.minor;
  raw->thr=iraw->thr;

  for (n=0;n<nrang;n++) {
    for (x=0;x<mplgs;x++) {
      acftmp[2*(n*mplgs+x)]=iraw->acfd[x*MAX_RANGE+n];
      acftmp[2*(n*mplgs+x)+1]=iraw->acfd[LAG_SIZE*MAX_RANGE+MAX_RANGE*x+n];
      if (xcf !=0) {
        xcftmp[2*(n*mplgs+x)]=iraw->xcfd[x*MAX_RANGE+n];
        xcftmp[2*(n*mplgs+x)+1]=iraw->xcfd[LAG_SIZE*MAX_RANGE+MAX_RANGE*x+n];
      }
    }
  }

  RawSetPwr(raw,nrang,iraw->pwr0,0,NULL);
  RawSetACF(raw,nrang,mplgs,acftmp,0,NULL);
  if (xcf !=0) RawSetXCF(raw,nrang,mplgs,xcftmp,0,NULL);
 
}



void IDLCopyRawDataToIDL(int nrang,int mplgs, int xcf,struct RawData *raw,
                           struct RawIDLData *iraw) {

  int n,x;

  memset(iraw,0,sizeof(struct RawIDLData));

  iraw->revision.major=raw->revision.major;
  iraw->revision.minor=raw->revision.minor;
  iraw->thr=raw->thr;

  for (n=0;n<nrang;n++) {
    iraw->pwr0[n]=raw->pwr0[n];
    if (raw->acfd[0] !=NULL) {
      for (x=0;x<mplgs;x++) {
        iraw->acfd[x*MAX_RANGE+n]=raw->acfd[0][n*mplgs+x];
        iraw->acfd[LAG_SIZE*MAX_RANGE+MAX_RANGE*x+n]=raw->acfd[1][n*mplgs+x];
        if ((xcf !=0) && (raw->xcfd !=NULL)) {
          iraw->xcfd[x*MAX_RANGE+n]=raw->xcfd[0][n*mplgs+x];
          iraw->xcfd[LAG_SIZE*MAX_RANGE+MAX_RANGE*x+n]=raw->xcfd[1][n*mplgs+x];
        }
      }
    }
  }
}

struct RawIDLData *IDLMakeRawData(IDL_VPTR *vptr) {
  
  void *s=NULL;
  
  static IDL_MEMINT rdim[]={1,MAX_RANGE};
  static IDL_MEMINT ldim[]={3,MAX_RANGE,LAG_SIZE,2};
 
  static IDL_STRUCT_TAG_DEF revision[]={
    {"MAJOR",0,(void *) IDL_TYP_LONG},
    {"MINOR",0,(void *) IDL_TYP_LONG},
    {0}};
  
  static IDL_STRUCT_TAG_DEF rawdata[]={    
    {"REVISION",0,NULL},   /* 0 */
    {"THR",0,(void *) IDL_TYP_FLOAT},   /* 1 */ 
    {"PWR0",rdim,(void *) IDL_TYP_FLOAT}, /* 2 */
    {"ACFD",ldim,(void *) IDL_TYP_FLOAT}, /* 3 */
    {"XCFD",ldim,(void *) IDL_TYP_FLOAT}, /* 4 */
  };

  static IDL_MEMINT ilDims[IDL_MAX_ARRAY_DIM];
 
  rawdata[0].type=IDL_MakeStruct("RLSTR",revision);

  s=IDL_MakeStruct("RAWDATA",rawdata);
           
  ilDims[0]=1;
  
  return (struct RawIDLData *) IDL_MakeTempStruct(s,1,ilDims,vptr,TRUE);
  
}


struct RawIDLInx *IDLMakeRawInx(int num,IDL_VPTR *vptr) {
  
  void *s=NULL;

  
  static IDL_MEMINT idim[1]={0};
 
  static IDL_STRUCT_TAG_DEF rawinx[]={
    {"TIME",0,(void *) IDL_TYP_DOUBLE},
    {"OFFSET",0,(void *) IDL_TYP_LONG},
    {0}};

   s=IDL_MakeStruct("RAWINX",rawinx);  
   idim[0]=num;

   return (struct RawIDLInx *) IDL_MakeTempStruct(s,1,idim,vptr,TRUE);
}
