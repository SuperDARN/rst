/* fitblk.h
   =========
   Author: R.J.Barnes
*/




#ifndef _FITBLK_H
#define _FITBLK_H

struct FitPrm {
  int channel; /* zero=mono 1 or 2 is stereo */
  int offset; /* used for stereo badlags */
  int cp;
  int xcf;
  int tfreq;
  int noise;
  int nrang;
  int smsep;
  int nave;
  int mplgs;
  int mpinc;
  int txpl;
  int lagfr;
  int mppul;
  int bmnum;
  int old;
  int *lag[2];
  int *pulse;
  int *pwr0;
  int maxbeam;
  double interfer[3];
  double bmsep;
  double phidiff;
  double tdiff;
  double vdir;
};

struct FitBlock {
  struct FitPrm prm;
  struct complex *acfd;
  struct complex *xcfd;
};

struct FitElv { /* elevation angle derived from the cross correlation */
  double normal;
  double low;
  double high;
};

struct FitNoise { /* noise statistics */
  double vel;
  double skynoise;
  double lag0;
};

struct FitRange {  /* fitted parameters for a single range */
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
      
#endif
