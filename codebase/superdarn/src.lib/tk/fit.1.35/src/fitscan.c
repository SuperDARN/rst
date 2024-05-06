/* fitscan.c
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
   Comments: E.G.Thomas

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
#include "rprm.h"
#include "fitdata.h"
#include "scandata.h"
#include "fitread.h"
#include "fitwrite.h"



/**
 * Reads one full scan of data from the open fit file and stores it in the
 * RadarScan structure. Returns zero on success, 1 if the end of file was
 * reached, or -1 if an error occurred.
 **/
int FitReadRadarScan(int fid, int *state,
                     struct RadarScan *ptr,
                     struct RadarParm *prm, struct FitData *fit,
                     int tlen,
                     int lock, int chn) {

    int fstatus=0;
    int flg=0;
    int r;

    struct RadarBeam *bm;
    if (ptr==NULL) return -1;
    if (fit==NULL) return -1;
    if (state==NULL) return -1;

    /* If the end of the fit file has not already been reached then read the
     * next record into the prm and fit structures */
    if (*state!=1) {

        /* If no channel is specified then read the next available record */
        if (chn==0) fstatus=FitRead(fid,prm,fit);

        else {

            /* If a channel is specified then continue reading fit records until
             * either a record for the correct channel is found or the end of
             * the file is reached */
            do {

                fstatus=FitRead(fid,prm,fit); /* read first fit record */

                if (fstatus==-1) break;

                /* The logic below will handle mono/stereo mixed days.
                 * If the channel is set to zero then the file is mono
                 * and we should treat the data as channel A.
                */

            } while ( ((chn==2) && (prm->channel !=2)) || 
                      ((chn==1) && (prm->channel ==2)));

        }

        if (fstatus==-1) return -1;

    }

    /* If no errors occurred when reading the fit record then begin populating
     * the RadarScan structure with parameters from the prm structure */
    if (*state !=2) {

        ptr->stid=prm->stid;
        ptr->version.major=prm->revision.major;
        ptr->version.minor=prm->revision.minor;
        ptr->st_time=TimeYMDHMSToEpoch(prm->time.yr,prm->time.mo,
                      prm->time.dy,
                      prm->time.hr,prm->time.mt,
                      prm->time.sc+prm->time.us/1.0e6);

        /* If scan flag is being ignored and assuming scan boundaries are fixed
         * relative to start of day, then recalculate scan start time */
        if ((tlen !=0) && (lock !=0)) ptr->st_time=tlen*(int) (ptr->st_time/tlen);

        /* Reset number of beams in RadarScan structure to zero, etc. */
        RadarScanReset(ptr);

    }

    /* Indicate that fit data was successfully read and the next record is pending */
    *state=1;

    /* Add fit and prm records to RadarScan structure until either a new scan
     * flag is found or the data duration exceeds tlen, depending on input options */
    do {

        /* Add a new beam to the RadarScan structure */
        bm=RadarScanAddBeam(ptr,prm->nrang);
        if (bm==NULL) {
            flg=-1;
            break;
        }

        /* Calculate time of radar beam sounding */
        bm->time=TimeYMDHMSToEpoch(prm->time.yr,prm->time.mo,
                  prm->time.dy,
                  prm->time.hr,prm->time.mt,
                  prm->time.sc+prm->time.us/1.0e6);

        /* Load radar operating parameters into RadarBeam structure */
        bm->scan=prm->scan;
        bm->bm=prm->bmnum;
        bm->bmazm=prm->bmazm;
        bm->cpid=prm->cp;
        bm->intt.sc=prm->intt.sc;
        bm->intt.us=prm->intt.us;
        bm->nave=prm->nave;
        bm->frang=prm->frang;
        bm->rsep=prm->rsep;
        bm->rxrise=prm->rxrise;
        bm->freq=prm->tfreq;
        bm->noise=fit->noise.skynoise;
        bm->atten=prm->atten;
        bm->channel=prm->channel;
        bm->nrang=prm->nrang;

        /* Set flags indicating scatter in each range gate to zero */
        for (r=0;r<bm->nrang;r++) bm->sct[r]=0;

        /* Loop over number of range gates along radar beam, populating
         * RadarBeam structure with fit data */
        for (r=0;r<bm->nrang;r++) {

            bm->sct[r]=(fit->rng[r].qflg==1);
            bm->rng[r].gsct=fit->rng[r].gsct;
            bm->rng[r].p_0=fit->rng[r].p_0;
            bm->rng[r].p_0_e=0;
            bm->rng[r].v=fit->rng[r].v;
            bm->rng[r].p_l=fit->rng[r].p_l;
            bm->rng[r].w_l=fit->rng[r].w_l;
            bm->rng[r].v_e=fit->rng[r].v_err;
            if (fit->xrng !=NULL) bm->rng[r].phi0=fit->xrng[r].phi0;
            else bm->rng[r].phi0=0;
            if (fit->elv !=NULL) bm->rng[r].elv=fit->elv[r].normal;
            else bm->rng[r].elv=0;

        }

        /* Calculate end time of radar scan */
        ptr->ed_time=TimeYMDHMSToEpoch(prm->time.yr,prm->time.mo,
                      prm->time.dy,
                      prm->time.hr,prm->time.mt,
                      prm->time.sc+prm->time.us/1.0e6);

        /* Error check if too many beams were included in RadarScan structure */
        if (ptr->num>1000) {
            flg=-1;
            break;
        }

        /* Read the next record into the prm and fit structures */
        if (chn==0) fstatus=FitRead(fid,prm,fit);
        else {

            /* If a channel is specified then continue reading fit records until
             * either a record for the correct channel is found or the end of
             * the file is reached */
            do {

                fstatus=FitRead(fid,prm,fit);

                if (fstatus==-1) break;


                /* The logic below will handle mono/stereo mixed days.
                 * If the channel is set to zero then the file is mono
                 * and we should treat the data as channel A.
                */

            } while ( ((chn==2) && (prm->channel !=2)) || 
                      ((chn==1) && (prm->channel ==2)));

        }

        /* If end of file was reached by FitRead then set flg equal to 2 */
        if (fstatus==-1) flg=2;
        else {

            if (tlen==0) {

                /* Set flg equal to 1 if scan data according to scan flag
                 * was successfully stored in RadarScan structure */
                if (abs(prm->scan)==1) flg=1;

                /* Set flg equal to 1 if scan data of length tlen was
                 * successfully stored in RadarScan structure */
            } else if (ptr->ed_time-ptr->st_time>=tlen) flg=1;

        }

    } while (flg==0);

    /* Decrement value of flg, such that 2->1 indicates the end of the file was
     * reached or 1->0 indicates attempt to store scan data was a success */
    if (flg>0) flg--;

    /* If the end of fit file was reached then store that information in state
     * variable */
    if (flg==1) *state=2;

    /* Return zero on success, 1 if the end of file was reached, or -1 if an error
     * occurred */
    return flg;

}



int FitFreadRadarScan(FILE *fp,int *state,
                      struct RadarScan *ptr,
                      struct RadarParm *prm,struct FitData *fit,
                      int tlen,
                      int lock,int chn) {
  return FitReadRadarScan(fileno(fp),state,ptr,prm,fit,tlen,lock,chn);
}



int FitToRadarScan(struct RadarScan *ptr,
                   struct RadarParm *prm,
                   struct FitData *fit) {


  int r;
  struct RadarBeam *bm;
  if (ptr==NULL) return -1;
  if (fit==NULL) return -1;
  
  ptr->stid=prm->stid;

  bm=RadarScanAddBeam(ptr,prm->nrang);
  if (bm==NULL) return -1;  
  bm->time=TimeYMDHMSToEpoch(prm->time.yr,prm->time.mo,
			      prm->time.dy,
			      prm->time.hr,prm->time.mt,
			      prm->time.sc+prm->time.us/1.0e6); 
 
  bm->scan=prm->scan;  
  bm->bm=prm->bmnum;
  bm->bmazm=prm->bmazm;
  bm->cpid=prm->cp;
  bm->intt.sc=prm->intt.sc;
  bm->intt.us=prm->intt.us;
  bm->nave=prm->nave;
  bm->frang=prm->frang;
  bm->rsep=prm->rsep;
  bm->rxrise=prm->rxrise;
  bm->freq=prm->tfreq;
  bm->noise=prm->noise.search;
  bm->atten=prm->atten;
  bm->channel=prm->channel;
  bm->nrang=prm->nrang;

  for (r=0;r<bm->nrang;r++) bm->sct[r]=0;
  if (fit->rng==NULL) {
    memset(bm->rng,0,sizeof(struct RadarBeam)*bm->nrang);
    return 0;
  }
  for (r=0;r<bm->nrang;r++) {
    bm->sct[r]=(fit->rng[r].qflg==1);
    bm->rng[r].gsct=fit->rng[r].gsct;
    bm->rng[r].p_0=fit->rng[r].p_0;
    bm->rng[r].p_0_e=0;
    bm->rng[r].v=fit->rng[r].v;
    bm->rng[r].p_l=fit->rng[r].p_l;
    bm->rng[r].w_l=fit->rng[r].w_l;
    bm->rng[r].v_e=fit->rng[r].v_err;    
  }

  return 0;
}
