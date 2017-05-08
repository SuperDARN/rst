/*Copyright (C) 2016  SuperDARN Canada

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

/*
Structures used in ACF fitting

Keith Kotyk
ISAS
July 2015

*/






#ifndef _FITSTRUCT_H
#define _FITSTRUCT_H

#include "leastsquares.h"

typedef struct rangenode{
	int range;
	double* CRI;
  double refrc_idx;
	llist alpha_2;
	llist phases;
	llist pwrs;
  llist elev;
  FITDATA* l_pwr_fit;
  FITDATA* q_pwr_fit;
  FITDATA* l_pwr_fit_err;
  FITDATA* q_pwr_fit_err;
  FITDATA* phase_fit;
  FITDATA* elev_fit;
}RANGENODE;

typedef struct phasenode{
	double phi;
	double t;
	double sigma;
  int lag_idx;
}PHASENODE;

typedef struct pwrnode{
	double ln_pwr;
	double t;
	double sigma;
  int lag_idx;
}PWRNODE;

typedef struct lag_node{
	int lag_num;
	int pulses[2];
	int lag_idx;
	int sample_base1;
	int sample_base2;
}LAGNODE;

typedef struct alpha{
  int lag_idx;
  double alpha_2;
}ALPHANODE;

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
  double bmsep;
  double interfer_x;
  double interfer_y;
  double interfer_z;
  double phidiff;
  double tdiff;
  double vdir;
  struct {
    short yr;
    short mo;
    short dy;
    short hr;
    short mt;
    short sc;
    int us;
  } time;

}FITPRMS;


#endif
