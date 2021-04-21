/* rcdf.h
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

