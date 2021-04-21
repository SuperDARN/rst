/* rprmidl.h
   ========= 
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

#ifndef _RPRMIDL_H
#define _RPRMIDL_H

#define PULSE_SIZE 64
#define LAG_SIZE 150 
#define MAX_RANGE 800

#define ORIGIN_TIME_SIZE 26
#define ORIGIN_COMMAND_SIZE 128
#define COMBF_SIZE 80       



struct RadarIDLParm {
  struct {
    UCHAR major;
    UCHAR minor;
  } revision;

  struct {
    UCHAR code;
    IDL_STRING time;
    IDL_STRING command;
  } origin;


  short cp;
  short stid;

  struct {
    short yr;
    short mo;
    short dy;
    short hr;
    short mt;
    short sc;
    IDL_LONG us;
  } time;

  short txpow;
  short nave;
  short atten;
  short lagfr;
  short smsep;
  short  ercod;

  struct {
    short agc;
    short lopwr;
  } stat;

  struct {
    float search;
    float mean;
  } noise;

  short channel;
  short bmnum;
  float bmazm;
  short scan;

  short rxrise;
  struct {
    short sc;
    IDL_LONG us;
  } intt;
  short txpl;
  short mpinc;
  short mppul;
  short mplgs;
  short mplgexs;
  short nrang;
  short frang;
  short rsep;
  short xcf;
  short tfreq;
  short offset;
  short ifmode;

  IDL_LONG mxpwr;
  IDL_LONG lvmax;

  short pulse[PULSE_SIZE];
  short lag[LAG_SIZE*2];
  IDL_STRING combf;
};


struct RadarIDLParm *IDLMakeRadarParm(IDL_VPTR *vptr);

void IDLCopyRadarParmToIDL(struct RadarParm *prm,
                                      struct RadarIDLParm *iprm);

void IDLCopyRadarParmFromIDL(struct RadarIDLParm *iprm,struct RadarParm *prm);


#endif
