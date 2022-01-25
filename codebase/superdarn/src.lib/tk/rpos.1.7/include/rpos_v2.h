/* rpos_v2.h
   =========
   Author: E.G.Thomas

   Copyright (C) 2022  Evan G. Thomas

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


#ifndef _RPOS_V2_H
#define _RPOS_V2_H

void RPosGeo_v2(int center, int bcrd, int rcrd, struct RadarSite *pos,
                int frang, int rsep, int rxrise, double height,
                double *rho, double *lat, double *lng,
                double hop, int model, int gs, int rear);

double calc_standard_vhm(double r, double fh);
double calc_chisham_vhm(double r, double *hop);
double calc_cv_vhm(double r, int gs, double *hop);

double calc_virtual_height(double r, double elv, double hop);
double calc_elevation_angle(double r, double xh, double hop, int pseudo);
double calc_coning_angle(struct RadarSite *pos, int bmnum, double elv,
                         int center, int rear);
double calc_ground_range(double r, double xh, double hop);

void gc_destination(double lat1, double lon1, double d, double azi,
                    double *lat2, double *lon2);
#endif
