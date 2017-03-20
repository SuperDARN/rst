/* rcdf.h
   ======
   Author: R.J.Barnes
*/


/*
   See license.txt
*/




#ifndef _RCDF_H
#define _RCDF_H

struct RCDFData {
  char name[CDF_VAR_NAME_LEN+1];
  CDFstatus status;
  long num;
  long type;
  long numdim;
  long var[CDF_MAX_DIMS];
  long dim[CDF_MAX_DIMS];
  long size;
  void *data;
};

long RCDFSize(long type);
long RCDFMajority(CDFid id);
long RCDFVarNumZ(CDFid id);
long RCDFVarNumR(CDFid id);
long RCDFMaxRecZ(CDFid id,char *varname);
long RCDFMaxRecR(CDFid id,char *varname);
int RCDFReadZ(CDFid id,long recno,char **varname,struct RCDFData *ptr);
int RCDFReadR(CDFid id,long recno,char **varname,struct RCDFData *ptr);

struct RCDFData *RCDFMake(int num);
void RCDFFree(struct RCDFData *ptr,int num);

#endif

