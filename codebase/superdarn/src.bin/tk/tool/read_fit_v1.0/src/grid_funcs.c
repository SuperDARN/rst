#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <zlib.h>
#include "rtypes.h"
#include "dmap.h"
#include "option.h"
#include "rtime.h"
#include "radar.h"
#include "rprm.h"
#include "fitdata.h"
#include "cfitdata.h"
#include "scandata.h"
#include "fitread.h"
#include "fitscan.h"
#include "fitindex.h"
#include "fitseek.h"
#include "oldfitread.h"
#include "oldfitscan.h"
#include "cfitread.h"
#include "cfitindex.h"
#include "cfitseek.h"
#include "cfitscan.h"
#include "fitscan.h"
#include "filter.h"
#include "bound.h"
#include "checkops.h"
#include "rpos.h"

/**
 * THESE ROUTINES ARE COPIED FROM MAKE_GRID, BECAUSE I COULDN'T FIGURE OUT
 * HOW TO INCLUDE THIS FILE IN THE MAKEFILE AS A SRC FILE
 **/

/**
 * Exclude beams that are not part of a scan
 **/
int exclude_outofscan(struct RadarScan *ptr)
{
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
  for (n=0;n<ptr->num;n++)
    {
      
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

/**
 * Outputs an error statement for an unrecognized input option
 **/

int rst_opterr(char *txt)
{
  fprintf(stderr,"Option not recognized: %s\n",txt);
  fprintf(stderr,"Please try: make_grid --help\n");
  return(-1);
}

/**
* END COPIED ROUTINES
**/
