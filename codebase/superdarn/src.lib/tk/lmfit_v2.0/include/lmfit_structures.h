/*
 Structures used in ACF fitting

 Copyright (c) 2016 University of Saskatchewan

 Adapted by: Ashton Reimer
 From code by: Keith Kotyk


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






#ifndef _LMFITSTRUCT_H
#define _LMFITSTRUCT_H

#include "lmfit_leastsquares.h"

typedef struct rangenode{
	int range;
	double* SC_pow;
  double refrc_idx;
  llist acf;
  llist xcf;
  llist scpwr;
	llist phases;
	llist pwrs;
  llist elev;
  LMFITDATA* l_acf_fit;
  LMFITDATA* q_acf_fit;
  LMFITDATA* l_xcf_fit;
  LMFITDATA* q_xcf_fit;
  double prev_pow;
  double prev_phase;
  double prev_width;
}RANGENODE;

typedef struct scnode{
  double clutter;
  double t;
  double sigma;
  int lag_num;
}SCNODE;

typedef struct phasenode{
	double phi;
	double t;
	double sigma;
  int lag_num;
}PHASENODE;

typedef struct pwrnode{
	double pwr;
	double t;
	double sigma;
  int lag_num;
}PWRNODE;

typedef struct acfnode{
  double re;
  double im;
  double t;
  double sigma_re;
  double sigma_im;
  int lag_num;
}ACFNODE;

typedef struct lag_node{
	int pulse_diff;
	int pulses[2];
	int lag_num;
	int sample_base1;
	int sample_base2;
}LAGNODE;

typedef enum {
  FORWARD,
  BEHIND
} INTERFER_POS;

typedef struct fit_prms {
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
  double *pwr0;
  double **acfd;
  double **xcfd;
  int maxbeam;
  double bmoff;
  double bmsep;
  double interfer_x;
  double interfer_y;
  double interfer_z;
  double phidiff;
  double tdiff;
  double vdir;
}FITPRMS;


#endif
