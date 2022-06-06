/* error_estimates.h
   ==========
*/

#ifndef _error_estimates_H
#define _error_estimates_H

#define PI 3.14159

int lag0_error(int nrang, float *pwr0, float noise_power, float K, float Kn, float *error);
int norm_acf_error1(int nrang, float *pwr0, float noise_power, float *selfclutter, float *acf, float K, float Kn, float *error);
int norm_acf_error2(int nrang, float *pwr0, float noise_power, float *selfclutter, float *acf, float K, float Kn, float *error);
int acf_error(int mplgs, float pwr0, float noise_power, float *selfclutter, float K, float *error);

#endif
