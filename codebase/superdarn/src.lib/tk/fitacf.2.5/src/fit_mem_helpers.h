/* fit_mem_helpers.h

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

#ifndef _FIT_MEM_HELPERS_H
#define _FIT_MEM_HELPERS_H



void free_arrays(double **sum_wk2_arr, double **phi_res, double **tau, 
                    double **tau2, double **phi_k, double **w, double **pwr, 
                    double **wt, double **wt2, double **wp, int **bad_pwr);
void zero_fitrange(struct FitRange *ptr);
int allocate_ls_arrays(struct FitPrm *prm, double **sum_wk2_arr, double **phi_res, double **tau, 
                    double **tau2, double **phi_k, double **w, double **pwr, 
                    double **wt, double **wt2, double **wp, int **bad_pwr);
#endif
