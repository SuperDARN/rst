/* rawdata.h
   ========== 

  Author: R.J.Barnes
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

#ifndef _RAWDATA_H
#define _RAWDATA_H

struct RawData {
  struct {
    int major;
    int minor;
  } revision;
  float thr;
  float *pwr0;
  float *acfd[2];
  float *xcfd[2];
};


struct RawData *RawMake();
void RawFree(struct RawData *ptr);
int RawSetPwr(struct RawData *ptr,int nrang,float *pwr0,int snum,int *slist);
int RawSetACF(struct RawData *ptr,int nrang,int mplgs,float *acfd,int snum,int *slist);
int RawSetXCF(struct RawData *ptr,int nrang,int mplgs,float *xcfd,int snum,int *slist);


void *RawFlatten(struct RawData *ptr,int nrang,int mplgs,size_t *size);
int RawExpand(struct RawData *ptr,int nrang,int mplgs,void *buffer);




#endif
