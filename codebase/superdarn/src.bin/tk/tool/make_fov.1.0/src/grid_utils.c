/* make_grid.c
   ===========
*/

/*
 LICENSE AND DISCLAIMER

 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

 This file is part of the Radar Software Toolkit (RST).

 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.

 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <zlib.h>

#include "rtypes.h"
#include "scandata.h"


/**
 * Exclude beams that are not part of a scan
 **/
int exclude_outofscan(struct RadarScan *ptr) {

    int n,num=0;
    struct RadarBeam *tmp;

    /* Exit if there is no data in this radar scan */
    if (ptr==NULL) return -1;
    if (ptr->num==0) return -1;

    /* Initialize a temporary structure, exiting if it ends up being null */
    /* AGB: should this be able to happen, given the previous two checks? */
    tmp=malloc(sizeof(struct RadarBeam)*ptr->num);
    if (tmp==NULL) return -1;

    /* Cycle through each beam in this scan */
    for (n=0;n<ptr->num;n++) {

      /* Exit if this beam is not part of a scan */
      if (ptr->bm[n].scan<0) continue;

      /* Save the beam to the temporary structure if it is part of a scan */
      memcpy(&tmp[num],&ptr->bm[n],sizeof(struct RadarBeam));
      num++;

    }

    free(ptr->bm);

    /* If there are beams in a scan, assign the temporary data to the radar
       structure.  Otherwise, merely free the temporary structure */
    if (num>0) {
        ptr->bm=realloc(tmp,sizeof(struct RadarBeam)*num);
        if (ptr->bm==NULL) {
            free(tmp);
            ptr->num=0;
            return -1;
        }
    } else {
        free(tmp);
        ptr->bm=NULL;
    }

    /* Update the number of beams in the radar structure */
    ptr->num=num;

    /* Return success */
    return 0;
}


/**
 * Calculates the slant range to a range gate.
 **/
double slant_range(int frang, int rsep,
                   double rxris, double range_edge,
                   int range_gate) {

    int lagfr,smsep;

    /* Calculate the lag to first range gate in microseconds */
    lagfr=frang*20/3;

    /* Calculate the sample separation in microseconds */
    smsep=rsep*20/3;

    /* Return the calculated slant range [km] */
    return (lagfr-rxris+(range_gate-1)*smsep+range_edge)*0.15;

}
