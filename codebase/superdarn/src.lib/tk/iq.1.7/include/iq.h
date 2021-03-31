/* iq.h
   ============= 
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

#ifndef _IQ_H
#define _IQ_H

struct IQ {
  struct { 
    int major;
    int minor;
  } revision;
  int chnnum;
  int smpnum;
  int skpnum;
  int seqnum;
  int tbadtr;
  struct timespec *tval;
  int *atten;
  float *noise;
  int *offset;
  int *size;
  int *badtr;
};


struct IQ *IQMake();
void IQFree(struct IQ *ptr);

int IQSetTime(struct IQ *ptr,int nave,struct timespec *tval);
int IQSetAtten(struct IQ *ptr,int nave,int *atten);
int IQSetNoise(struct IQ *ptr,int nave,float *noise);
int IQSetOffset(struct IQ *ptr,int nave,int *offset);
int IQSetSize(struct IQ *ptr,int nave,int *size);
int IQSetBadTR(struct IQ *ptr,int nave,int *badtr);

void *IQFlatten(struct IQ *ptr,int nave,size_t *size);
int IQExpand(struct IQ *ptr,int nave,void *buffer);

#endif
