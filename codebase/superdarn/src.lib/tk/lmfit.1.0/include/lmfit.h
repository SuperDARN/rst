/* lmfit.h
   ==========
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
                struct FitData *fit,struct FitBlock *fblk, int print);
double getguessex(struct RadarParm *prm,struct RawData *raw,
              struct FitData *fit, struct FitBlock *fblk, int rang, double skynoise);
int singlefit(int m, int n, double *p, double *deviates,
                        double **derivs, void *private);
void lm_noise_stat(struct RadarParm *prm, struct RawData * raw,
                double * skynoise);
void setup_fblk(struct RadarParm *prm, struct RawData *raw,struct FitBlock *input);
double calc_phi0(float *x,float *y, float m, int n);
void ls_fit(float *x,float *y, int n, float *a, float *b);
double calc_err(double w_guess, struct RawData *raw, float *good_lags, int goodcnt,
              int R, float *lagpwr,double pwr, struct RadarParm *prm);
int lm_dbl_cmp(const void *x,const void *y);

#endif
