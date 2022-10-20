/* fitblk.h
   =========
   Author: R.J.Barnes

   Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory


       RST is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    Modifications
    =============
    2020-11-12 Marina Schmidt Converted RST complex -> C library complex
    E.G.Thomas 2021-08: added support for bmoff parameter
    2022-02-04 Emma Bland (UNIS): Added "error" and "fitted" fields to FitElv structure to support FitACF v3
*/




#ifndef _FITBLK_H
#define _FITBLK_H
#include <complex.h>

struct FitPrm {
  int channel; 
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
  int old_elev; /* set to use the old elevation angle routines SGS: 20170811 */
  int *lag[2];
  int *pulse;
  int *pwr0;
  int maxbeam;
  double interfer[3];
  double bmoff;
  double bmsep;
  double phidiff;
  double tdiff;
  double vdir;
};

struct FitBlock {
  struct FitPrm prm;
  double complex *acfd;
  double complex *xcfd;
};

struct FitElv { /* elevation angle derived from the cross correlation */
  double normal;
  double low;
  double high;
  double fitted;
  double error;
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
