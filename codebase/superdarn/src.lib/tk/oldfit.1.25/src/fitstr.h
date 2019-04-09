/* fitstr.h
   ========
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




#define FIT_RECL 1024
#define INX_RECL 16

#define ORIG_COMBF_SIZE 80
#define ORIG_PULSE_PAT_LEN   16             
#define ORIG_LAG_TAB_LEN     48              
#define ORIG_MAX_RANGE  75              

#ifdef SEC
#undef SEC
#endif




struct radar_parms {

  struct {
    char MAJOR,MINOR;
  } REV;/* REV.MAJOR is the major revision #, REV.MINOR is the minor # */

  int16  NPARM,	/* the total number of 16-bit words in the parameter block */
	 ST_ID,	/* station ID code: 1=Goose Bay, 2=Schefferville,
					4=Halley Station, 8=Syowa */
	 YEAR,	/* date and start time of the record.  Year=19xx */
	 MONTH,
	 DAY,
	 HOUR,
	 MINUT,
	 SEC,
	 TXPOW,	/* transmitted power (kW) */
	 NAVE,	/* number of times the pulse sequence was transmitted */
	 ATTEN,	/* attenuation setting of the receiver (0-3) */
	 LAGFR,	/* the lag to the first range (microsecs) (see note 2 below) */
	 SMSEP,	/* the sample separation (microsecs) (see note 2) */
	 ERCOD,	/* error flag (see error definitions) */
	 AGC_STAT,	/* AGC status word */
	 LOPWR_STAT,	/* Low power status word */
	 NBAUD; 		/* number of elements in pulse code */
  int32 NOISE,	/* the noise level determined during the clear freq. search */
        NOISE_MEAN;  /* average noise across frequency band */
  int16 CHANNEL;
  int16 RXRISE;	/* receiver rise time */
  /* The second set of parameters are set by the user */	
  /* These parameters can either be set manually or by a RADLANG program */
  /* There are a total of 24 words in this parameter list */
  int16 INTT,	/* the integration period */
	TXPL,	/* the pulse length (in micro seconds) */
	MPINC,	/* the basic lag separation (in microseconds) */
	MPPUL,	/* the number of pulses in the pulse pattern */
	MPLGS,	/* the number of lags in the pulse pattern (note 1 below) */
	NRANG,	/* the number of range gates */
	FRANG,	/* the distance to the first range (see note 2 below) */
	RSEP,	/* the range separation (in km) */
	BMNUM,	/* beam number */				
	XCF,	/* flag indicating if cross-correlations were being done */
	TFREQ,	/* the transmitted frequency (in KHz) */
	SCAN;	/* flag indicating the scan mode  */
	int32 MXPWR,	/*	Maximum power allowed (see note 3) */
	      LVMAX;	/*  Maximum noise level allowed (see note 3) */
	/* user defined variables */
	int32 usr_resL1,
              usr_resL2;
	int16 CP,
	      usr_resS1,
	      usr_resS2,
	      usr_resS3;
};


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
  int16 pulse[ORIG_PULSE_PAT_LEN];
  int16 lag[2][ORIG_LAG_TAB_LEN];
  char combf[ORIG_COMBF_SIZE];
  struct noise noise;
  struct range_data rng[ORIG_MAX_RANGE];
  struct range_data xrng[ORIG_MAX_RANGE];
  struct elv elev[ORIG_MAX_RANGE];
};
      
struct fit_rec1 {
  int32 rrn,r_time;
  int16 plist[sizeof(struct radar_parms)/2];
  int16 ppat[ORIG_PULSE_PAT_LEN];
  int16 lags[2][ORIG_LAG_TAB_LEN];
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



