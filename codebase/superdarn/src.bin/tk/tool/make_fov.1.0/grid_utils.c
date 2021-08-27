/* grid_utils.c
   ============
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
#include "rtime.h"
#include "scandata.h"


/**
 * Exclude beams that arxe not part of a scan
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

int set_stereo_channel(char *chnstr)
{
  int channel = 0;

  if(chnstr != NULL)
    {
      if(tolower(chnstr[0]) == 'a') channel = 1;
      else if(tolower(chnstr[0]) == 'b') channel = 2;
      else
	fprintf(stderr, "Unknown Stereo channel %s, leaving unset\n", chnstr);
    }

  return(channel);
}

int set_fix_channel(char *chnstr_fix)
{
  int channel_fix=-1;

  if(chnstr_fix != NULL)
    {
      if(tolower(chnstr_fix[0]) == 'a') channel_fix = 1;
      else if(tolower(chnstr_fix[0]) == 'b') channel_fix = 2;
      else if(tolower(chnstr_fix[0]) == 'c') channel_fix = 3;
      else if(tolower(chnstr_fix[0]) == 'd') channel_fix = 4;
      else
	fprintf(stderr, "Unknown fix channel %s, leaving unset\n", chnstr_fix);
    }

  return(channel_fix);
}

/**
 * Converts an input date from YYYYMMDD format to an epoch time in number of
 * seconds since 00:00 UT on January 1, 1970.
 **/
double strdate(char *text) {

    double tme;
    int val;
    int yr,mo,dy;

    /* Calculate day, month, and year from YYYYMMDD format date */
    val=atoi(text);
    dy=val % 100;
    mo=(val / 100) % 100;
    yr=(val / 10000);

    /* If only 2-digit year provided then assume it was pre-2000 */
    if (yr<1970) yr+=1900;

    /* Calculate epoch time of input year, month, and day */
    tme=TimeYMDHMSToEpoch(yr,mo,dy,0,0,0);

    /* Return epoch time in number of seconds since 00:00UT on January 1, 1970 */
    return tme;

}


/**
 * Converts an input time from HHMM format to number of seconds.
 **/
double strtime(char *text) {

    int hr,mn;
    int i;

    for (i=0;(text[i] !=':') && (text[i] !=0);i++);
    if (text[i]==0) return atoi(text)*3600L;
    text[i]=0;
    hr=atoi(text);
    mn=atoi(text+i+1);
    return hr*3600L+mn*60L;

}
