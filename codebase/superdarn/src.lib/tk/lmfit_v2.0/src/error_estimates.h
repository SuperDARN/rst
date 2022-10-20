/* error_estimates.h
   ==========

 Copyright (c) 2016 University of Saskatchewan
 Author: Ashton Reimer

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


*/

#ifndef _error_estimates_H
#define _error_estimates_H

#define PI 3.14159

int lag0_error(int nrang, float *pwr0, float noise_power, float K, float Kn, float *error);
int norm_acf_error1(int nrang, float *pwr0, float noise_power, float *selfclutter, float *acf, float K, float Kn, float *error);
int norm_acf_error2(int nrang, float *pwr0, float noise_power, float *selfclutter, float *acf, float K, float Kn, float *error);
int acf_error(int mplgs, float pwr0, float noise_power, float *selfclutter, float K, float *error);

#endif
