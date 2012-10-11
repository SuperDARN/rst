/* sim_data.h
   ==========
*/

#ifndef _sim_data_H
#define _sim_data_H

#define PI 3.14159

struct irreg
{
  double space;         /*location in space*/
  double time;          /*generation time*/
  double width;         /*lifetime distribution*/
  double start_time;    /*sample when irreg appears*/
  double velo;          /*random velocity component (Gaussian)*/
};

void acf_27(complex double * aa, complex double * rr, int cpid);
void sim_data(double *t_d, double *t_g, double *t_c, double *v_dop, int * qflg,
              double *velo, double *amp0, double freq, double noise_lev,
              int noise_flg, int nave, int nrang, int lagfr,
              double smsep, int cpid, int life_dist,
              int n_pul, int cri_flg, int n_lags, int * pulse_t, int * tau,
              double dt, complex double * out_samples, complex double ** out_acfs, int decayflg);
float ran1(long *idum);
float gasdev(long *idum);

#endif
