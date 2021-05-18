/* acf_preproc.h
   =============
   Author: R.J.Barnes & K.Baker
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

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






/*	This header file defines the values which an ACF preprocessor should
	return to fitacf */

#define ACF_UNMODIFIED	0
#define ACF_ION_SCAT	1
#define ACF_GROUND_SCAT 2
#define ACF_MIXED_SCAT	3

int acf_preproc(double complex *acf, double complex *orig_acf, 
        double *noise_lev, int range, int *badlag, int mplgs);

