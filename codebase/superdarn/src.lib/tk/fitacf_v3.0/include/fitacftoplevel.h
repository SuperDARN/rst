/*Copyright (C) 2016  SuperDARN Canada

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

/*
FITACF main routine

Keith Kotyk
ISAS
July 2015

*/

#ifndef _FITACFTOPLEVEL_H
#define _FITACFTOPLEVEL_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <zlib.h>
#include "fit_structures.h"
#include "rtypes.h"
#include "dmap.h"
#include "rprm.h"
#include "radar.h"
#include "rawdata.h"
#include "fitdata.h"
#include "fitblk.h"

#define MAJOR 3
#define MINOR 0

void FitacfFree(FITPRMS *fit_prms);
void Copy_Fitting_Prms(struct RadarSite *radar_site,
		       struct RadarParm *radar_prms, struct RawData *raw_data,
		       FITPRMS *fit_prms);
int Allocate_Fit_Prm(struct RadarParm *radar_prms, FITPRMS *fit_prms);
int Fitacf(FITPRMS *fit_prms,struct FitData *fit_data);

#endif
