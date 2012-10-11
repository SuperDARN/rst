/* rawdata.h
   ========== 

  Author: R.J.Barnes
*/

/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
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
