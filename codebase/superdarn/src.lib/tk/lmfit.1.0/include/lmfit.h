/* lmfit.h
   ==========

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
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:

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

*/

#ifndef _lmfit_H
#define _lmfit_H

#ifndef PULSE_SIZE
#define PULSE_SIZE 256
#endif

struct datapoints
{
  double *x;
  double *y;
  double *ey;
  double mag;
};
struct exdatapoints
{
  int lagnum;
  float phase;
  float lagpwr;
};

void lmfit(struct RadarParm *prm,struct RawData *ptr,
           struct FitData *fit,struct FitBlock *fblk,
           struct RadarSite *hd,struct RadarTdiff *tdiff,
           double tdiff_fix,int print);
double getguessex(struct RadarParm *prm,struct RawData *raw,
              struct FitData *fit, struct FitBlock *fblk, int rang, double skynoise);
int singlefit(int m, int n, double *p, double *deviates,
                        double **derivs, void *private);
void lm_noise_stat(struct RadarParm *prm, struct RawData * raw,
                double * skynoise);
void setup_fblk(struct RadarParm *prm, struct RawData *raw,
                struct FitBlock *input, struct RadarSite *hd);
double calc_phi0(float *x,float *y, float m, int n);
void ls_fit(float *x,float *y, int n, float *a, float *b);
double calc_err(double w_guess, struct RawData *raw, float *good_lags, int goodcnt,
              int R, float *lagpwr,double pwr, struct RadarParm *prm);
int lm_dbl_cmp(const void *x,const void *y);

#endif
