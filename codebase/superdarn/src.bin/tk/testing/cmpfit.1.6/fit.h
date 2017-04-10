/* fitdata.h
   =========
   Author: R.J.Barnes
*/

/*
   See license.txt
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
      
