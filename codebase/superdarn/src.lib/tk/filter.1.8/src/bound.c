/* bound.c
   =======
   Author: R.J.Barnes
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
   Comments: E.G.Thomas (2016)
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include "rtypes.h"
#include "rtime.h"
#include "scandata.h"
#include "filter.h"



/**
 * Excludes points from radar scan data depending on their type (eg ground
 * or ionospheric scatter).
 **/
int FilterBoundType(struct RadarScan *ptr, int type) {

    int bm,rng;

    /* Loop over all beams in scan */
    for (bm=0;bm<ptr->num;bm++) {

        /* Loop over all range gates in beam */
        for (rng=0;rng<ptr->bm[bm].nrang;rng++) {

            /* If no scatter in range gate then continue */
            if (ptr->bm[bm].sct[rng]==0) continue;

            /* If gsct flag (1=gs,0=iono) matches input type flag
             * (0=gs,1=iono,2=both) then mark cell as having no scatter */
            if (ptr->bm[bm].rng[rng].gsct==type) ptr->bm[bm].sct[rng]=0;

        }

    }

    /* Return zero if successful */
    return 0;

}



/**
 * Examines each point in the radar scan data and tests whether its parameters
 * (velocity, power, spectral width, velocity error) lie within certain values.
 * If they do not the data point is rejected and removed.
 **/
int FilterBound(int prm, struct RadarScan *ptr, double *min, double *max) {

    int bm,rng;

    /* Loop over all beams in scan */
    for (bm=0;bm<ptr->num;bm++) {

        /* Loop over all range gates in beam */
        for (rng=0;rng<ptr->bm[bm].nrang;rng++) {

            /* If no scatter in range gate then continue */
            if (ptr->bm[bm].sct[rng]==0) continue;

            /* If measured velocity magnitude less than vmin then mark gate
             * as empty */
            if (fabs(ptr->bm[bm].rng[rng].v)<min[0]) ptr->bm[bm].sct[rng]=0;

            /* If measured velocity magnitude greater than vmax then mark gate
             * as empty */
            if (fabs(ptr->bm[bm].rng[rng].v)>max[0]) ptr->bm[bm].sct[rng]=0;

            /* If measured lambda power less than pmin then mark gate
             * as empty */
            if (ptr->bm[bm].rng[rng].p_l<min[1]) ptr->bm[bm].sct[rng]=0;

            /* If measured lambda power greater than pmax then mark gate
             * as empty */
            if (ptr->bm[bm].rng[rng].p_l>max[1]) ptr->bm[bm].sct[rng]=0;

            /* If measured spectral width less than wmin then mark gate
             * as empty */
            if (ptr->bm[bm].rng[rng].w_l<min[2]) ptr->bm[bm].sct[rng]=0;

            /* If measured spectral width greater than wmax then mark gate
             * as empty */
            if (ptr->bm[bm].rng[rng].w_l>max[2]) ptr->bm[bm].sct[rng]=0;

            /* If measured velocity error less than vemin then mark gate
             * as empty */
            if (ptr->bm[bm].rng[rng].v_e<min[3]) ptr->bm[bm].sct[rng]=0;

            /* If measured velocity error greater than vemax then mark gate
             * as empty */
            if (ptr->bm[bm].rng[rng].v_e>max[3]) ptr->bm[bm].sct[rng]=0;

        }

    }

    /* Return zero if successful */
    return 0;

}
