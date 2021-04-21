/* cfitdata.h
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



#ifndef _CFITDATA_H
#define _CFITDATA_H


#define CFIT_MAJOR_REVISION 2
#define CFIT_MINOR_REVISION 1

struct CFitCell {
  int gsct;
  double p_0;
  double p_0_e;
  double v;
  double v_e;
  double w_l;
  double w_l_e;
  double p_l;
  double p_l_e;
};

struct CFitdata {
  struct {
    int major;
    int minor;
  } version;
  int16 stid; 
  double time;
  int16 scan;
  int16 cp;
  int16 bmnum;
  float bmazm;
  int16 channel;
  struct {
    int16 sc;
    int32 us;
  } intt;
  int16 frang;
  int16 rsep;
  int16 rxrise;
  int16 tfreq;
  float noise;
  int16 atten;
  int16 nave;
  int16 nrang;

  int16 num;
  int16 *rng;
  struct CFitCell *data;
};

struct CFitdata *CFitMake();
void CFitFree(struct CFitdata *ptr);
int CFitSetRng(struct CFitdata *ptr,int num);

#endif

 





