/* noise_stat.h
   ============
   Author: R.J.Barnes & K.Baker & P.Ponomarenko
*/
/*
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

double lag_power(double complex *a);
double noise_stat(double mnpwr,struct FitPrm *ptr,
                  struct FitACFBadSample *badsmp, double complex *acf);
