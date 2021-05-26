/* do_phase_fit.h
   ==============
   Author: K.Baker

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



int do_phase_fit(double omega_guess, 
		  char xflag, 
		  int mplgs,
		  double complex *acf,
		  double *tau,
		  double *w,
		  double *sum_wk2_arr,
		  double *phi_res,
		  int *badlag,
		  double t0,
		  double sum_w,
		  double sum_wk,
		  double sum_wk2,
		  
		  double *omega,
		  double *phi0,
		  double *sdev,
		  double *phi0_err,
		  double *omega_err);

