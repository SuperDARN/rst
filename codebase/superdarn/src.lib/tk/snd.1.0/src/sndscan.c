/* sndscan.c
   =========
   Author: E.G.Thomas

   
   Copyright (C) <year>  <name of author>

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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <zlib.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "dmap.h"
#include "snddata.h"
#include "scandata.h"
#include "sndread.h"


/**
 * Reads one full scan of data from the open sndt file and stores it in the
 * RadarScan structure. Returns zero on success, 1 if the end of file was
 * reached, or -1 if an error occurred.
 **/
int SndReadRadarScan(int fid, int *state,
                     struct RadarScan *ptr, struct SndData *snd,
                     int tlen, int lock) {

    int fstatus=0;
    int flg=0;
    int r;

    struct RadarBeam *bm;
    if (ptr==NULL) return -1;
    if (snd==NULL) return -1;
    if (state==NULL) return -1;

    /* If the end of the snd file has not already been reached then read the
     * next record into the snd structure */
    if (*state!=1) {

        /* Read the next available record */
        fstatus=SndRead(fid,snd);

        if (fstatus==-1) return -1;

    }

    /* If no errors occurred when reading the snd record then begin populating
     * the RadarScan structure with parameters from the snd structure */
    if (*state !=2) {

        ptr->stid=snd->stid;
        ptr->version.major=snd->snd_revision.major;
        ptr->version.minor=snd->snd_revision.minor;
        ptr->st_time=TimeYMDHMSToEpoch(snd->time.yr,snd->time.mo,
                      snd->time.dy,
                      snd->time.hr,snd->time.mt,
                      snd->time.sc+snd->time.us/1.0e6);

        /* If scan flag is being ignored and assuming scan boundaries are fixed
         * relative to start of day, then recalculate scan start time */
        if ((tlen !=0) && (lock !=0)) ptr->st_time=tlen*(int) (ptr->st_time/tlen);

        /* Reset number of beams in RadarScan structure to zero, etc. */
        RadarScanReset(ptr);

    }

    /* Indicate that snd data was successfully read and the next record is pending */
    *state=1;

    /* Add snd records to RadarScan structure until either a new scan
     * flag is found or the data duration exceeds tlen, depending on input options */
    do {

        /* Add a new beam to the RadarScan structure */
        bm=RadarScanAddBeam(ptr,snd->nrang);
        if (bm==NULL) {
            flg=-1;
            break;
        }

        /* Calculate time of radar beam sounding */
        bm->time=TimeYMDHMSToEpoch(snd->time.yr,snd->time.mo,
                  snd->time.dy,
                  snd->time.hr,snd->time.mt,
                  snd->time.sc+snd->time.us/1.0e6);

        /* Load radar operating parameters into RadarBeam structure */
        bm->scan=snd->scan;
        bm->bm=snd->bmnum;
        bm->bmazm=snd->bmazm;
        bm->cpid=snd->cp;
        bm->intt.sc=snd->intt.sc;
        bm->intt.us=snd->intt.us;
        bm->nave=snd->nave;
        bm->frang=snd->frang;
        bm->rsep=snd->rsep;
        bm->rxrise=snd->rxrise;
        bm->freq=snd->tfreq;
        bm->noise=snd->noise.search;
        bm->atten=0;
        bm->channel=snd->channel;
        bm->nrang=snd->nrang;

        /* Set flags indicating scatter in each range gate to zero */
        for (r=0;r<bm->nrang;r++) bm->sct[r]=0;

        /* Loop over number of range gates along radar beam, populating
         * RadarBeam structure with snd data */
        for (r=0;r<bm->nrang;r++) {

            bm->sct[r]=(snd->rng[r].qflg==1);
            bm->rng[r].gsct=snd->rng[r].gsct;
            bm->rng[r].p_0=0;
            bm->rng[r].p_0_e=0;
            bm->rng[r].v=snd->rng[r].v;
            bm->rng[r].p_l=snd->rng[r].p_l;
            bm->rng[r].w_l=snd->rng[r].w_l;
            bm->rng[r].v_e=snd->rng[r].v_err;
            if (snd->xcf) bm->rng[r].phi0=snd->rng[r].phi0;
            else bm->rng[r].phi0=0;
            bm->rng[r].elv=0;

        }

        /* Calculate end time of radar scan */
        ptr->ed_time=TimeYMDHMSToEpoch(snd->time.yr,snd->time.mo,
                      snd->time.dy,
                      snd->time.hr,snd->time.mt,
                      snd->time.sc+snd->time.us/1.0e6);

        /* Error check if too many beams were included in RadarScan structure */
        if (ptr->num>1000) {
            flg=-1;
            break;
        }

        /* Read the next record into the snd structure */
        fstatus=SndRead(fid,snd);

        /* If end of file was reached by SndRead then set flg equal to 2 */
        if (fstatus==-1) flg=2;
        else {

            if (tlen==0) {

                /* Set flg equal to 1 if scan data according to scan flag
                 * was successfully stored in RadarScan structure */
                if (abs(snd->scan)==1) flg=1;

                /* Set flg equal to 1 if scan data of length tlen was
                 * successfully stored in RadarScan structure */
            } else if (ptr->ed_time-ptr->st_time>=tlen) flg=1;

        }

    } while (flg==0);

    /* Decrement value of flg, such that 2->1 indicates the end of the file was
     * reached or 1->0 indicates attempt to store scan data was a success */
    if (flg>0) flg--;

    /* If the end of snd file was reached then store that information in state
     * variable */
    if (flg==1) *state=2;

    /* Return zero on success, 1 if the end of file was reached, or -1 if an error
     * occurred */
    return flg;

}



int SndFreadRadarScan(FILE *fp,int *state,
                      struct RadarScan *ptr,struct SndData *snd,
                      int tlen,int lock) {
  return SndReadRadarScan(fileno(fp),state,ptr,snd,tlen,lock);
}



int SndToRadarScan(struct RadarScan *ptr,
                   struct SndData *snd) {


  int r;
  struct RadarBeam *bm;
  if (ptr==NULL) return -1;
  if (snd==NULL) return -1;
  
  ptr->stid=snd->stid;

  bm=RadarScanAddBeam(ptr,snd->nrang);
  if (bm==NULL) return -1;  
  bm->time=TimeYMDHMSToEpoch(snd->time.yr,snd->time.mo,
			      snd->time.dy,
			      snd->time.hr,snd->time.mt,
			      snd->time.sc+snd->time.us/1.0e6); 
 
  bm->scan=snd->scan;  
  bm->bm=snd->bmnum;
  bm->bmazm=snd->bmazm;
  bm->cpid=snd->cp;
  bm->intt.sc=snd->intt.sc;
  bm->intt.us=snd->intt.us;
  bm->nave=snd->nave;
  bm->frang=snd->frang;
  bm->rsep=snd->rsep;
  bm->rxrise=snd->rxrise;
  bm->freq=snd->tfreq;
  bm->noise=snd->noise.search;
  bm->atten=0;
  bm->channel=snd->channel;
  bm->nrang=snd->nrang;

  for (r=0;r<bm->nrang;r++) bm->sct[r]=0;
  if (snd->rng==NULL) {
    memset(bm->rng,0,sizeof(struct RadarBeam)*bm->nrang);
    return 0;
  }
  for (r=0;r<bm->nrang;r++) {
    bm->sct[r]=(snd->rng[r].qflg==1);
    bm->rng[r].gsct=snd->rng[r].gsct;
    bm->rng[r].p_0=0;
    bm->rng[r].p_0_e=0;
    bm->rng[r].v=snd->rng[r].v;
    bm->rng[r].p_l=snd->rng[r].p_l;
    bm->rng[r].w_l=snd->rng[r].w_l;
    bm->rng[r].v_e=snd->rng[r].v_err;    
    if (snd->xcf) bm->rng[r].phi0=snd->rng[r].phi0;
    else bm->rng[r].phi0=0;
    bm->rng[r].elv=0;
  }

  return 0;
}
