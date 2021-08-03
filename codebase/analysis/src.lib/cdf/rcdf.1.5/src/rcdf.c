/* rcdf.c
   ======
   Author: R.J.Barnes
*/

/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cdf.h"
#include "rcdf.h"

long RCDFSize(long type) {
  switch (type) {
  case CDF_BYTE:
  case CDF_CHAR:
  case CDF_INT1:
  case CDF_UCHAR:
  case CDF_UINT1:
    return 1;
  case CDF_INT2:
  case CDF_UINT2:
    return 2;
  case CDF_INT4:
  case CDF_UINT4:
  case CDF_REAL4:
  case CDF_FLOAT:
    return 4;
  default:
    return 8;
  }
} 


struct RCDFData *RCDFMake(int num) {
  int n;
  struct RCDFData *ptr;
  ptr=malloc(sizeof(struct RCDFData)*num);
  memset(ptr,0,sizeof(struct RCDFData)*num);
  for (n=0;n<num;n++) ptr[n].data=NULL;
  return ptr;
}


void RCDFFree(struct RCDFData *ptr,int num) {
  int n;
  if (ptr==NULL) return;
  for (n=0;n<num;n++) if (ptr[n].data !=NULL) free(ptr[n].data);
  free(ptr);
}


long RCDFMajority(CDFid id) {
  CDFstatus status;
  long majority;
  status=CDFlib(SELECT_,CDF_,id,GET_,CDF_MAJORITY_,&majority,NULL_);
  if (status !=CDF_OK) return -1;
  return majority;
}


long RCDFVarNumZ(CDFid id) {
  CDFstatus status;
  long varnum;
  status=CDFlib(SELECT_,CDF_,id,GET_,CDF_NUMzVARS_,&varnum,NULL_);
  if (status !=CDF_OK) return -1;
  return varnum;
}

long RCDFVarNumR(CDFid id) {
  CDFstatus status;
  long varnum;
  status=CDFlib(SELECT_,CDF_,id,GET_,CDF_NUMrVARS_,&varnum,NULL_);
  if (status !=CDF_OK) return -1;
  return varnum;
}

long RCDFMaxRecZ(CDFid id,char *varname) {
  CDFstatus status;
  long maxrec;
  status=CDFlib(SELECT_,CDF_,id,SELECT_,zVAR_NAME_,varname,NULL_);
  if (status !=CDF_OK) return -1;
  status=CDFlib(GET_,zVAR_MAXREC_,&maxrec,NULL_);
  if (status !=CDF_OK) return -1;
  return maxrec;
}

long RCDFMaxRecR(CDFid id,char *varname) {
  CDFstatus status;
  long maxrec;
  status=CDFlib(SELECT_,CDF_,id,SELECT_,rVAR_NAME_,varname,NULL_);
  if (status !=CDF_OK) return -1;
  status=CDFlib(GET_,rVAR_MAXREC_,&maxrec,NULL_);
  if (status !=CDF_OK) return -1;
  return maxrec;
}



int RCDFReadZ(CDFid id,long recno,char **varname,struct RCDFData *ptr) {

  CDFstatus status;  
  int i,j,cnt=0;

  long *var;
  long *offset;
  long *size;
  void **buffer;

  long total_size=0;
  void *data;

  status=CDFlib(SELECT_,CDF_,id,NULL_);
  if (status !=CDF_OK) return -1;

  for (i=0;varname[i] !=NULL;i++) cnt++;
  var=malloc(sizeof(long)*cnt);
  offset=malloc(sizeof(long)*cnt);
  size=malloc(sizeof(long)*cnt);
  buffer=malloc(sizeof(void *)*cnt);

  cnt=0; 
  for (i=0;varname[i] !=NULL;i++) {     
    
    /* get variable number */

    ptr[i].status=CDFlib(GET_,zVAR_NUMBER_,varname[i],&ptr[i].num,NULL_);
    if (ptr[i].status !=CDF_OK) continue;

  
    /* select it */

    ptr[i].status=CDFlib(SELECT_,zVAR_,ptr[i].num,NULL_);
    if (ptr[i].status !=CDF_OK) continue;

    /* find its proper name */

    ptr[i].status=CDFlib(GET_,zVAR_NAME_,ptr[i].name,NULL_);
    if (ptr[i].status !=CDF_OK) continue;


    /* find its type */

    ptr[i].status=CDFlib(GET_,zVAR_DATATYPE_,&ptr[i].type,NULL_);
    if (ptr[i].status !=CDF_OK) continue;

  

    /* find number of dimensions */

    ptr[i].status=CDFlib(GET_,zVAR_NUMDIMS_,&ptr[i].numdim,NULL_);
    if (ptr[i].status !=CDF_OK) continue;
   
    /* find extent */

    ptr[i].status=CDFlib(GET_,zVAR_DIMSIZES_,&ptr[i].dim,NULL_);
    if (ptr[i].status !=CDF_OK) continue;
 
     ptr[i].status=CDFlib(GET_,zVAR_DIMVARYS_,&ptr[i].var,NULL_);
    if (ptr[i].status !=CDF_OK) continue;
 
 
    ptr[i].size=RCDFSize(ptr[i].type);

    if (ptr[i].numdim>0) for (j=0;j<ptr[i].numdim;j++) 
      if (ptr[i].var[j]==VARY) ptr[i].size=ptr[i].size*ptr[i].dim[j];

    
    if (ptr[i].data==NULL) ptr[i].data=malloc(ptr[i].size);
    else ptr[i].data=realloc(ptr[i].data,ptr[i].size);
    
    if (ptr[i].data==NULL) continue;
    
    var[cnt]=ptr[i].num;
    buffer[cnt]=ptr[i].data;
    size[cnt]=ptr[i].size;
    offset[cnt]=total_size;
    total_size+=ptr[i].size;
    cnt++;
  }  

  
  data=malloc(total_size);
  if (data==NULL) return -1;

  status=CDFlib(SELECT_,zVARs_RECNUMBER_,recno,NULL_);
  if (status !=CDF_OK) return -1;
    
  status=CDFlib(GET_,zVARs_RECDATA_,cnt,var,data,NULL_);

  if (status==CDF_OK) 
  {
    for (int i=0;i<cnt;i++) 
    {
        memcpy(buffer[i],(void*)data+offset[i],size[i]);  
    }
  }
  free(data);   
  free(var);
  free(buffer);
  free(offset);
  free(size); 
  if (status !=CDF_OK) return -1;
  return 0;
}

int RCDFReadR(CDFid id,long recno,char **varname,struct RCDFData *ptr) {

  CDFstatus status;  
  int i,j,cnt=0;

  long *var=NULL;
  long *offset=NULL;
  long *size=NULL;
  void **buffer=NULL;

  long total_size=0;
  void *data=NULL;

  status=CDFlib(SELECT_,CDF_,id,NULL_);
  if (status !=CDF_OK) return -1;

  for (i=0;varname[i] !=NULL;i++) cnt++;
  var=malloc(sizeof(long)*cnt);
  offset=malloc(sizeof(long)*cnt);
  size=malloc(sizeof(long)*cnt);
  buffer=malloc(sizeof(void *)*cnt);

  cnt=0; 


  for (i=0;varname[i] !=NULL;i++) {     
    
    /* get variable number */

    ptr[i].status=CDFlib(GET_,rVAR_NUMBER_,varname[i],&ptr[i].num,NULL_);
    if (ptr[i].status !=CDF_OK) continue;

  
    /* select it */

    ptr[i].status=CDFlib(SELECT_,rVAR_,ptr[i].num,NULL_);
    if (ptr[i].status !=CDF_OK) continue;

    /* find its proper name */

    ptr[i].status=CDFlib(GET_,rVAR_NAME_,ptr[i].name,NULL_);
    if (ptr[i].status !=CDF_OK) continue;


    /* find its type */

    ptr[i].status=CDFlib(GET_,rVAR_DATATYPE_,&ptr[i].type,NULL_);
    if (ptr[i].status !=CDF_OK) continue;

  

    /* find number of dimensions */

    ptr[i].status=CDFlib(GET_,rVARs_NUMDIMS_,&ptr[i].numdim,NULL_);
    if (ptr[i].status !=CDF_OK) continue;
   
    /* find extent */

    ptr[i].status=CDFlib(GET_,rVARs_DIMSIZES_,&ptr[i].dim,NULL_);
    if (ptr[i].status !=CDF_OK) continue;
  
 
    ptr[i].status=CDFlib(GET_,rVAR_DIMVARYS_,&ptr[i].var,NULL_);
    if (ptr[i].status !=CDF_OK) continue;
 

    ptr[i].size=RCDFSize(ptr[i].type);

    if (ptr[i].numdim>0) for (j=0;j<ptr[i].numdim;j++) 
      if (ptr[i].var[j]==VARY) ptr[i].size=ptr[i].size*ptr[i].dim[j];


    
    if (ptr[i].data==NULL) ptr[i].data=malloc(ptr[i].size);
    else ptr[i].data=realloc(ptr[i].data,ptr[i].size);
    
    if (ptr[i].data==NULL) continue;    
    var[cnt]=ptr[i].num;
    buffer[cnt]=ptr[i].data;
    size[cnt]=ptr[i].size;
    offset[cnt]=total_size;
    total_size+=ptr[i].size;
    cnt++;
  }  

  
  data=malloc(total_size);
  if (data==NULL) return -1;

  status=CDFlib(SELECT_,rVARs_RECNUMBER_,recno,NULL_);
  if (status !=CDF_OK) return -1;
    
  status=CDFlib(GET_,rVARs_RECDATA_,cnt,var,data,NULL_);

 
  if (status==CDF_OK)
  {
    for (i=0;i<cnt;i++)
    {
        memcpy(buffer[i],(void*)data+offset[i],size[i]);  
    }
  }
  free(data);   
  free(var);
  free(buffer);
  free(offset);
  free(size); 

  if (status !=CDF_OK) return -1;

  return 0;
}







