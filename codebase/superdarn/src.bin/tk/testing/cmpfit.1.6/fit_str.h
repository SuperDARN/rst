/* fit_str.h
   =========
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




#ifndef _INT_TYPES
  #include "rtypes.h"
#endif

#define FIT_RECL 1024
#define INX_RECL 16

struct fit_rec1 {
  int32 rrn,r_time;
  int16 plist[sizeof(struct radar_parms)/2];
  int16 ppat[PULSE_PAT_LEN];
  int16 lags[2][LAG_TAB_LEN];
  char comment[ORIG_COMBF_SIZE];
  int32 r_noise_lev;
  int32 r_noise_lag0;  
  int32 r_noise_vel;
  int16 r_pwr0[ORIG_MAX_RANGE];
  int16 r_slist[ORIG_MAX_RANGE];
  char r_numlags[ORIG_MAX_RANGE];
};

struct fit_rec2 {
  int32 rrn, r_time, r_xflag;
  unsigned char range[25];
  unsigned char r_qflag[25];
  int16 r_pwr_l[25], r_pwr_l_err[25], r_pwr_s[25], r_pwr_s_err[25];
  int16 r_vel[25], r_vel_err[25], r_w_l[25], r_w_l_err[25];
  int16 r_w_s[25], r_w_s_err[25], r_phi0[25], r_phi0_err[25];
  int16 r_elev[25], r_elev_low[25], r_elev_high[25];
  int16 r_sdev_l[25], r_sdev_s[25], r_sdev_phi[25], r_gscat[25];
};

union fit_out {
  int16 rec0[512];
  struct fit_rec1 r1;
  struct fit_rec2 r2;
};



