/* sndidl.h
   ========
   Author E.G.Thomas
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
along with this program.  If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/


#ifndef _SNDIDL_H
#define _SNDIDL_H


#define MAX_RANGE 300

#define ORIGIN_TIME_SIZE 26
#define ORIGIN_COMMAND_SIZE 128
#define COMBF_SIZE 80


struct SndIDLData {
  struct {
    UCHAR major;
    UCHAR minor;
  } radar_revision;

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

  short nave;
  short lagfr;
  short smsep;

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

  short nrang;
  short frang;
  short rsep;
  short xcf;
  short tfreq;

  float sky_noise;

  IDL_STRING combf;

  struct {
    IDL_LONG major;
    IDL_LONG minor;
  } fit_revision;

  struct {
    short major;
    short minor;
  } snd_revision;

  UCHAR qflg[MAX_RANGE]; 
  UCHAR gflg[MAX_RANGE]; 
  float v[MAX_RANGE];
  float v_e[MAX_RANGE];
  float p_l[MAX_RANGE]; 
  float w_l[MAX_RANGE];
  UCHAR x_qflg[MAX_RANGE];
  float phi0[MAX_RANGE];
  float phi0_e[MAX_RANGE];
};


void IDLCopySndDataToIDL(int nrang, struct SndData *snd,
                         struct SndIDLData *isnd);

void IDLCopySndDataFromIDL(int nrang, struct SndIDLData *isnd,
                           struct SndData *snd);

struct SndIDLData *IDLMakeSndData(IDL_VPTR *vptr);

#endif
