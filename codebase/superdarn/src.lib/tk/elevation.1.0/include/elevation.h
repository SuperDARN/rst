/* elevation.h
   ===========
   Author: R.J.Barnes, K.Baker & S.G.Shepherd
*/


/*
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
2021-09-27 Angeline G. Burrell: Added a version of the elevation angle routine
                                that accounts for lobe direction.
*/

#ifndef _RMATH_H
#include "rmath.h"
#endif

#ifndef _RADAR_H
#include "radar.h"
#endif

#ifndef _FITBLK_H
#include "fitblk.h"
#endif

#ifndef _ELEVATION_H
#define _ELEVATION_H

double elevation_v2_lobe(int lobe, int bmnum, int tfreq, int channel,
			 struct RadarSite *site, double psi_obs);
double elevation(struct FitPrm * prm, double phi0);
double elevation_v2(struct FitPrm * prm, double psi_obs);
double elev_goose(struct FitPrm * prm, double phi0);

#endif
