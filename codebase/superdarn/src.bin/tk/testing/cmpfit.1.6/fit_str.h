/* fit_str.h
   =========
   Author: R.J.Barnes
*/

/*
   See license.txt
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



