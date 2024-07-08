/* grdidl.h
   ======== 
   Author R.J.Barnes
*/

/*
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

#ifndef _GRDIDL_H
#define _GRDIDL_H

struct GridIDLPrm {
  struct {
    int16 yr;
    int16 mo;
    int16 dy;
    int16 hr;
    int16 mt;
    double sc;
  } start;
  struct {
    int16 yr;
    int16 mo;
    int16 dy;
    int16 hr;
    int16 mt;
    double sc;
  } end;
  int32 stnum;
  int32 vcnum;
  int16 xtd;
};


struct GridIDLStVec {
  int16 stid;
  int16 chn;
  int16 npnt;
  float freq;
  int16 major_revision;
  int16 minor_revision;
  int16 prog_id;
  int16 gsct;
  struct {
    float mean;
    float sd;
  } noise;
  struct {
    float min;
    float max;
  } vel;
  struct {
    float min;
    float max;
  } pwr;
  struct {
    float min;
    float max;
  } wdt;
  struct {
    float min;
    float max;
  } verr;
};

struct GridIDLGVec {
  float mlat;
  float mlon;
  float azm;
  float srng;
  struct {
    float median;
    float sd;
  } vel;
 struct {
    float median;
    float sd;
  } pwr;      
  struct {
    float median;
    float sd;
  } wdt;
  int16 stid;
  int16 chn;
  int32 index;
};




struct GridIDLInx {
  double time;
  IDL_LONG offset;
};

void IDLCopyGridPrmFromIDL(struct GridIDLPrm *iprm,
                           struct GridData *grd);


void IDLCopyGridStVecFromIDL(struct GridIDLStVec *iprm,
			    int nvec,int size,struct GridData *grd);

void IDLCopyGridGVecFromIDL(struct GridIDLGVec *iprm,
			    int nvec,int size,struct GridData *grd);


void IDLCopyGridPrmToIDL(struct GridData *grd,
                         struct GridIDLPrm *iprm);

void IDLCopyGridStVecToIDL(struct GridData *grd,int nvec,int size,
			   struct GridIDLStVec *iprm);

void IDLCopyGridGVecToIDL(struct GridData *grd,int nvec,int size,
			  struct GridIDLGVec *iprm);


struct GridIDLPrm *IDLMakeGridPrm(IDL_VPTR *vptr);
struct GridIDLStVec *IDLMakeGridStVec(int nvec,IDL_VPTR *vptr);
struct GridIDLGVec *IDLMakeGridGVec(int nvec,IDL_VPTR *vptr);

struct GridIDLInx *IDLMakeGridInx(int num,IDL_VPTR *vptr);


#endif
