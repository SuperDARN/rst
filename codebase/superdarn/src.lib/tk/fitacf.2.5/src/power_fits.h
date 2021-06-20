/* power_fits.h
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
along with this program.  If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/

#ifndef _POWER_FITS_H
#define _POWER_FITS_H


void set_sigma_error_huge(struct FitRange *ptr);
void set_lambda_error_huge(struct FitRange *ptr);
void do_sigma_fit(struct FitPrm *prm, struct FitRange *ptr, int *badlag, 
                    int *bad_pwr, double *w, double *tau, double *tau2, double *pwr, double sum_np, 
                    double sum_w, double t0, double sum_wk, double sum_wk2, double c_log_err,
                    double sum_p, double t2, double sum_pk, double sum_wk4, double t4, 
                    double sum_pk2);
void do_lambda_fit(struct FitPrm *prm, struct FitRange *ptr, int *badlag, 
                    int *bad_pwr, double *w, double * tau, double *pwr, double sum_np, 
                    double sum_w, double t0, double sum_wk, double sum_wk2, double c_log_err,
                    double sum_p, double t2, double sum_pk);
#endif
