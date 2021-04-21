/* rprm.h
   ====== 
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



#ifndef _RPRM_H
#define _RPRM_H

struct RadarParm {
  struct {
    char major;
    char minor;
  } revision;

  struct {
    char code;
    char *time;
    char *command;
  } origin;

  int16 cp;
  int16 stid;

  struct {
    int16 yr;
    int16 mo;
    int16 dy;
    int16 hr;
    int16 mt;
    int16 sc;
    int32 us;
  } time;

  int16 txpow;
  int16 nave;
  int16 atten;
  int16 lagfr;
  int16 smsep;
  int16  ercod;
  
  struct {
    int16 agc;
    int16 lopwr;
  } stat;

  struct {
    float search;
    float mean;
  } noise;

  int16 channel;
  int16 bmnum;
  float bmazm;
  int16 scan;
 
  int16 rxrise;
  struct {
    int16 sc;
    int32 us;
  } intt;
  int16 txpl;
  int16 mpinc;
  int16 mppul;
  int16 mplgs;
  int16 mplgexs;
  int16 nrang;
  int16 frang;
  int16 rsep;
  int16 xcf;
  int16 tfreq;
  int16 offset; 
  int16 ifmode;

  int32 mxpwr;
  int32 lvmax;

  int16 *pulse;
  int16 *lag[2]; 
  char *combf;
};


struct RadarParm *RadarParmMake();
void RadarParmFree(struct RadarParm *ptr);

int RadarParmSetOriginTime(struct RadarParm *ptr,char *str);

int RadarParmSetOriginCommand(struct RadarParm *ptr,char *str);

int RadarParmSetCombf(struct RadarParm *ptr,char *str);

int RadarParmSetPulse(struct RadarParm *ptr,int mppul,int16 *pulse);

int RadarParmSetLag(struct RadarParm *ptr,int mplgs,int16 *);

int RadarParmEncode(struct DataMap *ptr,struct RadarParm *prm);

int RadarParmDecode(struct DataMap *ptr,struct RadarParm *prm);

void *RadarParmFlatten(struct RadarParm *ptr,size_t *size);

int RadarParmExpand(struct RadarParm *ptr,void *buffer);



#endif
