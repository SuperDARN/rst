/* rawidl.h
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

#ifndef _RAWIDL_H
#define _RAWIDL_H


struct RawIDLData {
  struct {
    IDL_LONG major;
    IDL_LONG minor;
  } revision;
  
  float thr;
  float pwr0[MAX_RANGE];
  float acfd[MAX_RANGE*LAG_SIZE*2];
  float xcfd[MAX_RANGE*LAG_SIZE*2];

};

struct RawIDLInx {
  double time;
  IDL_LONG offset;
};

void IDLCopyRawDataToIDL(int nrang,int mplgs,int xcf,struct RawData *raw,
			 struct RawIDLData *iraw);

void IDLCopyRawDataFromIDL(int nrang,int mplgs,int xcf,struct RawIDLData *iraw,
                           struct RawData *raw);

struct RawIDLData *IDLMakeRawData(IDL_VPTR *vptr);

struct RawIDLInx *IDLMakeRawInx(int num,IDL_VPTR *vptr);


#endif
