/*
LMFIT2 main routine

Adapted by: Ashton Reimer
From code by: Keith Kotyk

ISAS
August 2016

*/

#ifndef _FITACF_H
#define _FITACF_H

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

#define MAJOR 2
#define MINOR 0

void LMFIT2Free(FITPRMS *fit_prms);
void Copy_Fitting_Prms(struct RadarSite *radar_site,struct RadarParm *radar_prms,
				   struct RawData *raw_data,FITPRMS *fit_prms);
FITPRMS* Allocate_Fit_Prm(struct RadarParm *radar_prms);
int LMFIT2(FITPRMS *fit_prms,struct FitData *fit_data);

#endif
