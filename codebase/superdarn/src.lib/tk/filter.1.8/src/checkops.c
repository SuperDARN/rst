/* checkops.c
   ========== 
   Author: R.J.Barnes
   Comments: E.G.Thomas (2016)
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include "rtypes.h"
#include "rtime.h"
#include "rawdata.h"
#include "fitdata.h"
#include "scandata.h"
#include "filter.h"



/**
 * Checks to make sure that the radar operating parameters do not change
 * between scans. If the frequency, first range distance, or range separation
 * change between scans the location of scatter will also change making it
 * impossible to sensibly median filter the data. An error is returned if any
 * of these parameters changes between scans.
 **/
int FilterCheckOps(int depth, struct RadarScan **ptr, int fmax) {

    int bm,bt,num,t;

    /* Loop over number of scans in boxcar filter */
    for (num=0;num<depth;num++) {

        /* If center scan then continue */
        if (num==depth/2) continue;

        /* Loop over number of beams on center scan */
        for (bm=0;bm<ptr[depth/2]->num;bm++) {

            /* Current beam number on center scan */
            bt=ptr[depth/2]->bm[bm].bm;

            /* Loop over number of beams on scan num */
            for (t=0;t<ptr[num]->num;t++) {

                /* Check if current beam number matches center scan beam */
                if (ptr[num]->bm[t].bm !=bt) continue;

                /* Return error if the distance to first range doesn't match */
                if (ptr[num]->bm[t].frang !=ptr[depth/2]->bm[bm].frang) return -1;

                /* Return error if the range separation doesn't match */
                if (ptr[num]->bm[t].rsep !=ptr[depth/2]->bm[bm].rsep) return -1;

                /* Return error if frequency difference is larger than fmax */
                if ((fmax !=0) &&
                    (fabs(ptr[num]->bm[t].freq-ptr[depth/2]->bm[bm].freq)>fmax))
                    return -1;

            }

        }

    }

    /* Return zero if successful */
    return 0;

}
