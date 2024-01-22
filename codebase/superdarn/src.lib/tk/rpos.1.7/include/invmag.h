/* inv_mag.h
   =========
   Author: R.J.Barnes
Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

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
#ifndef _INVMAG_H
#define _INVMAG_H

int RPosRngBmAzmElv(int bm,int rn,int year,
                    struct RadarSite *hdw,double frang,
                    double rsep,double rx,double height,
                    double *azm,double *elv,int chisham);

int RPosInvMag(int bm,int rn,int year,struct RadarSite *hdw,double frang,
               double rsep,double rx,double height,
               double *mlat,double *mlon,double *azm,
               int chisham,int magflg);

#endif
