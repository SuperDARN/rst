/* fitdata.h
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





struct elv { /* elevation angle derived from the cross correlation */
  double normal;
  double low;
  double high;
};

struct noise { /* noise statistics */
  double vel;
  double skynoise;
  double lag0;
};

struct range_data {  /* fitted parameters for a single range */
  double v;
  double v_err;
  double p_0;
  double p_l;
  double p_l_err; 
  double p_s;
  double p_s_err;
  double w_l;
  double w_l_err;
  double w_s;
  double w_s_err;
  double phi0;
  double phi0_err;
  double sdev_l;
  double sdev_s;
  double sdev_phi;
  int qflg,gsct;
  char nump;
};


struct fitdata { /* the data structure */
  struct radar_parms prms;
  int16 pulse[PULSE_PAT_LEN];
  int16 lag[2][LAG_TAB_LEN];
  char combf[ORIG_COMBF_SIZE];
  struct noise noise;
  struct range_data rng[ORIG_MAX_RANGE];
  struct range_data xrng[ORIG_MAX_RANGE];
  struct elv elev[ORIG_MAX_RANGE];
};
      
