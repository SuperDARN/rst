/* iqidl.h
   ======== 
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

#ifndef _IQIDL_H
#define _IQIDL_H


#define MAXNAVE 300

struct IQIDL {
  struct {
    IDL_LONG major;
    IDL_LONG minor;
  } revision;
  IDL_LONG chnnum;
  IDL_LONG smpnum;
  IDL_LONG skpnum;
  IDL_LONG seqnum;
  IDL_LONG tbadtr;
  struct {
    IDL_LONG sec;
    IDL_LONG nsec;
  } tval[MAXNAVE]; 
  short atten[MAXNAVE];
  float noise[MAXNAVE];
  IDL_LONG offset[MAXNAVE];
  IDL_LONG size[MAXNAVE];
  IDL_LONG badtr[MAXNAVE];
};

struct IQIDLInx {
  double time;
  IDL_LONG offset;
};

void IDLCopyIQToIDL(struct IQ *iq,struct IQIDL *iiq);

void IDLCopyIQFromIDL(struct IQIDL *iiq,struct IQ *iq);

struct IQIDL *IDLMakeIQ(IDL_VPTR *vptr);

struct IQIDLInx *IDLMakeIQInx(int num,IDL_VPTR *vptr);


#endif
