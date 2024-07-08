/* rpos.h
   ======
   Author: R.J.Barnes

Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/

#ifndef _RPOS_H
#define _RPOS_H

struct RPos {
    double gdlat;
    double gdlon;
    double boresite;
    double bmwidth;
    double rxris;
};

double slant_range(int frang,int rsep,
                   double rxris,double range_edge,
                   int rang_gate);

void RPosGeo(int center,int bcrd,int rcrd,
             struct RadarSite *pos,
             int lagfr,int smsep,int rxrise,double height,
             double *rho,double *lat,double *lng,double *srng,int chisham);

void RPosMag(int center,int bcrd,int rcrd,
             struct RadarSite *pos,
             int lagfr,int smsep,int rxrise,double height,
             double *rho,double *lat,double *lng,double *srng,int chisham,
             int old_aacgm);

void RPosCubic(int center,int bcrd,int rcrd,
               struct RadarSite *pos,
               int lagfr,int smsep,int rxrise,double height,
               double *x,double *y,double *z);

void RPosGeoGS(int center,int bcrd,int rcrd,
               struct RadarSite *pos,
               int lagfr,int smsep,int rxrise,double height,
               double *rho,double *lat,double *lng);

void RPosMagGS(int center,int bcrd,int rcrd,
               struct RadarSite *pos,
               int lagfr,int smsep,int rxrise,double height,
               double *rho,double *lat,double *lng);

void RPosCubicGS(int center,int bcrd,int rcrd,
                 struct RadarSite *pos,
                 int lagfr,int smsep,int rxrise,double height,
                 double *x,double *y,double *z);

#endif
