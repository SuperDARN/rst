/* gtable.c
   ========
   Author: R.J.Barnes & W.Bristow
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
#include <string.h>
#include <math.h>
#include "radar.h"
#include "rpos.h" 
#include "invmag.h"

#include "rmath.h"
#include "rtime.h"
#include "scandata.h"
#include "gtable.h"

/* Hardcoded minimum velocity, power, and spectral width error values
 * used in GridTableMap */
double v_e_min=100;
double p_l_e_min=1;
double w_l_e_min=1;



/**
 * Creates a pointer to a structure to store the gridded radar velocity data for a
 * single time interval.
 **/
struct GridTable *GridTableMake() {

    struct GridTable *ptr;
    ptr=malloc(sizeof(struct GridTable));
    if (ptr==NULL) return NULL;
    memset(ptr,0,sizeof(struct GridTable));
    ptr->bm=NULL;
    ptr->pnt=NULL;

    /* Initialize start time to -1 to indicate this is the first grid record */
    ptr->st_time=-1;

    /* Return pointer to the GridTable structure */
    return ptr;

}



/**
 * Frees memory allocated to GridTable, GridBm, and GridPnt structures.
 **/
void GridTableFree(struct GridTable *ptr) {

    int n;

    if (ptr==NULL) return;

    if (ptr->bm !=NULL) {
        for (n=0;n<ptr->bnum;n++) {
            if (ptr->bm[n].azm !=NULL) free(ptr->bm[n].azm);
            if (ptr->bm[n].ival !=NULL) free(ptr->bm[n].ival);
            if (ptr->bm[n].inx !=NULL) free(ptr->bm[n].inx);
        }
        free(ptr->bm);
    }
    if (ptr->pnt !=NULL) free(ptr->pnt);
    free(ptr);

}



/**
 * Resets all of the values at each grid cell in the GridPnt structure to zero.
 **/
int GridTableZero(int pnum, struct GridPnt *ptr) {

    int i;

    /* Zero out all of the values at each grid cell in the GridPnt structure */
    for (i=0;i<pnum;i++) {
        ptr[i].azm=0;
        ptr[i].vel.median_n=0;
        ptr[i].vel.median_e=0;
        ptr[i].vel.sd=0;
        ptr[i].pwr.median=0;
        ptr[i].pwr.sd=0;
        ptr[i].wdt.median=0;
        ptr[i].wdt.sd=0;
        ptr[i].cnt=0;
    }

    /* Return zero on success */
    return 0;

}



/**
 * Tests whether gridded data should be written to a file. Returns a non-zero
 * value if the data should be written.
 **/
int GridTableTest(struct GridTable *ptr, struct RadarScan *scan) {

    double tm;
    int i;

    /* Calculate time at center of radar scan */
    tm=(scan->st_time+scan->ed_time)/2.0;

    /* If this is the first iteration in make_grid then return */
    if (ptr->st_time==-1) return 0;

    /* If the currently loaded RadarScan structure occurred after the end of
     * the GridTable structure then begin with the function */
    if (tm>ptr->ed_time) {
        /* average the grid */

        /* Initialize the number of grid points in the GridTable structure
         * to zero */ 
        ptr->npnt=0;

        /* Calculate average frequency, noise mean, and noise standard deviation
         * across all scans included in grid record */
        ptr->freq=ptr->freq/ptr->nscan;
        ptr->noise.mean=ptr->noise.mean/ptr->nscan;
        ptr->noise.sd=ptr->noise.sd/ptr->nscan;

        /* Loop over number of points in GridTable structure */
        for (i=0;i<ptr->pnum;i++) {

            /* If no velocity measurements in GridPnt then continue */
            if (ptr->pnt[i].cnt==0) continue;

            /* If at least 25% of the possible GridPnt cells don't have velocity
             * measurements then continue */
            if (ptr->pnt[i].cnt<=(0.25*ptr->nscan*ptr->pnt[i].max)) {
                ptr->pnt[i].cnt=0;
                continue;
            }

            /* Update the total number of grid points in the GridTable structure */
            ptr->npnt++;

            /* Calculate weighted mean of north/east velocity components */
            ptr->pnt[i].vel.median_n=ptr->pnt[i].vel.median_n/ptr->pnt[i].vel.sd;
            ptr->pnt[i].vel.median_e=ptr->pnt[i].vel.median_e/ptr->pnt[i].vel.sd;

            /* Calculate magnitude of weighted mean velocity vector */
            ptr->pnt[i].vel.median=sqrt(ptr->pnt[i].vel.median_n*ptr->pnt[i].vel.median_n +
            ptr->pnt[i].vel.median_e*ptr->pnt[i].vel.median_e);

            /* Calculate azimuth of weighted mean velocity vector */
            ptr->pnt[i].azm=atan2(ptr->pnt[i].vel.median_e,ptr->pnt[i].vel.median_n)*180./acos(-1.);

            /* Calculate weighted mean of spectral width and power */
            ptr->pnt[i].wdt.median=ptr->pnt[i].wdt.median/ptr->pnt[i].wdt.sd;
            ptr->pnt[i].pwr.median=ptr->pnt[i].pwr.median/ptr->pnt[i].pwr.sd;

            /* Calculate standard deviation of velocity, power, and spectral width */
            ptr->pnt[i].vel.sd=1/sqrt(ptr->pnt[i].vel.sd);
            ptr->pnt[i].wdt.sd=1/sqrt(ptr->pnt[i].wdt.sd);
            ptr->pnt[i].pwr.sd=1/sqrt(ptr->pnt[i].pwr.sd);

        }

        /* Reset status of GridTable structure to zero */
        ptr->status=0;

        /* GridTable structure ready to write to file */
        return 1;

    }

    /* GridTable structure not ready to write to file */
    return 0;

}



/**
 * Returns the index of the pointer to a newly added grid cell in the structure
 * storing gridded radar data.
 **/
int GridTableAddPoint(struct GridTable *ptr) {

    void *tmp=NULL;  

    /* Make sure that pointer to the GridTable structure exists */
    if (ptr==NULL) return -1;

    /* If the GridPnt structure doesn't exist then create one,
     * otherwise increase size of GridPnt by one */
    if (ptr->pnt==NULL) tmp=malloc(sizeof(struct GridPnt));
    else tmp=realloc(ptr->pnt,sizeof(struct GridPnt)*(ptr->pnum+1));
    if (tmp==NULL) return -1;
    ptr->pnt=tmp;
    memset(&ptr->pnt[ptr->pnum],0,sizeof(struct GridPnt));

    /* Update the total number of points in the GridTable structure */
    ptr->pnum++;

    /* Return the index of the current point */
    return ptr->pnum-1;

}



/**
 * Returns the index of the pointer to the GridPnt structure whose reference
 * number matches the input ref value. If a matching grid cells does not exist
 * in the GridTable structure, then a value of -1 is returned.
 **/
int GridTableFindPoint(struct GridTable *ptr, int ref) {

    int n;

    /* Loop over all points in GridTable structure. If point ref
     * matches input ref then break */
    for (n=0;n<ptr->pnum;n++) if (ref==ptr->pnt[n].ref) break;

    /* Return error flag if point not found */
    if (n==ptr->pnum) return -1;

    /* Return index of matching point */
    return n;

}



/**
 * Returns the index of the pointer to a newly added grid beam in the structure
 * storing gridded radar data. The magnetic latitude, longitude, and azimuth
 * of the velocity measurements are stored in the GridPnt and GridBm structures
 * respectively.
 **/
int GridTableAddBeam(struct GridTable *ptr,
                     struct RadarSite *pos, double alt,
                     double tval, struct RadarBeam *bm,
                     int chisham, int magflg) {

    int yr,mo,dy,hr,mt;
    double sc;
    double lat,lon,azm,geoazm,elv,lspc,velco;
    float grdlat,grdlon;
    int ref;
    int r=0,inx,s;
    void *tmp=NULL;
    struct GridBm *b=NULL;
    struct GridPnt *p=NULL;

    /* Make sure that pointer to GridTable structure exists */
    if (ptr==NULL) return -1;

    /* Make sure that pointer to RadarSite structure exists */
    if (pos==NULL) return -1;

    /* Make sure that pointer to RadarBeam structure exists */
    if (bm==NULL) return -1;

    /* Inertial velocity correction as a function of radar geodetic latitude [m/s] */
    velco=(2*PI/86400.0)*RE*1000*cos(PI*pos->geolat/180.0);

    /* If the GridBm structure doesn't exist in GridTable then create one,
     * otherwise add another beam to the array and update the beam number */
    if (ptr->bm==NULL) tmp=malloc(sizeof(struct GridBm));
    else tmp=realloc(ptr->bm,sizeof(struct GridBm)*(ptr->bnum+1));

    if (tmp==NULL) return -1;
    ptr->bm=tmp;
    b=&ptr->bm[ptr->bnum];

    /* Update the total number of beams in the GridTable structure */
    ptr->bnum++;

    /* Populate GridBm structure with info from RadarBeam structure 
     * (except rxrise - that we get from the hdw.dat file because
     * nearly all radars write a default value of 100 microseconds
     * hardcoded in default.h and set by global.c in QNX4 systems) */
    b->bm=bm->bm;
    b->frang=bm->frang;
    b->rsep=bm->rsep;
    b->rxrise=pos->recrise;
    b->nrang=bm->nrang;

    b->azm=malloc(sizeof(double)*b->nrang);
    if (b->azm==NULL) return -1;
    b->ival=malloc(sizeof(double)*b->nrang);
    if (b->ival==NULL) return -1;
    b->inx=malloc(sizeof(int)*b->nrang);
    if (b->inx==NULL) return -1;

    /* Convert input tval to year, month, day, hour, minute, seconds */
    TimeEpochToYMDHMS(tval,&yr,&mo,&dy,&hr,&mt,&sc);

    /* Loop through range gates along beam */
    for (r=0;r<b->nrang;r++) {

        /* Calculate geographic azimuth and elevation to range/beam position */
        s=RPosRngBmAzmElv(b->bm,r,yr,pos,
                    b->frang,b->rsep,b->rxrise,
                    alt,&geoazm,&elv,chisham);

        /* If geographic azimuth/elevation calculation failed then
         * break out of loop */
        if (s==-1) break;

        /* Calculate magnetic latitude, longitude, and azimuth of range/beam
         * position */
        s=RPosInvMag(b->bm,r,yr,pos,
               b->frang,b->rsep,b->rxrise,
               alt,&lat,&lon,&azm,chisham,magflg);

        /* If magnetic latitude/longitude/azimuth calculation failed then 
         * break out of loop */
        if (s==-1) break;

        /* Make sure magnetic azimuth varies between 0-360 degrees */
        if (azm<0.) azm+=360.;

        /* Make sure magnetic longitude varies between 0-360 degrees */
        if (lon<0) lon+=360;

        /* Calculate magnetic grid cell latitude
         * (eg, 72.1->72.5, 57.8->57.5, etc) */
        if (lat>0) grdlat=(int) (lat)+0.5;
        else grdlat=(int) (lat)-0.5;

        /* Calculate magnetic grid longitude spacing at grid latitude */
        lspc=((int) (360*cos(fabs(grdlat)*PI/180)+0.5))/(360.0);

        /* Calculate magnetic grid cell longitude */
        grdlon=((int) (lon*lspc)+0.5)/lspc;

        /* Calculate reference number to grid latitude/longitude cell */
        if (lat>0)
            ref=1000*( (int) lat ) + ( (int) (lon*lspc) );
        else ref=-1000*( (int) -lat ) - ( (int) (lon*lspc) );

        /* Find index of GridPnt structure corresponding to reference number */
        inx=GridTableFindPoint(ptr,ref);

        /* If matching GridPnt structure not found then create one */
        if (inx==-1) inx=GridTableAddPoint(ptr);

        /* Get pointer to the GridPnt structure */
        p=&ptr->pnt[inx];  

        /* Set the reference number of the GridPnt structure */
        p->ref=ref;

        /* Update the total number of range gates that map to the GridPnt structure */
        p->max++;

        /* Set the magnetic latitude and longitude of GridPnt structure */
        p->mlat=grdlat;
        p->mlon=grdlon;

        /* Set the index, magnetic azimuth, and inertial velocity correction factor of
         * the GridBm structure */
        b->inx[r]=inx;    
        b->azm[r]=azm;
        b->ival[r]=velco*cos(PI*(90+geoazm)/180.0);

    }

    /* Return error if didn't finish looping through all range gates */
    if (r !=b->nrang) {
        return -1;
    }

    /* Return the index of the beam number added to the GridTable structure */
    return ptr->bnum-1;

}



/**
 * Returns the index of the pointer to the GridBm structure whose beam number
 * and operating parameters match those of the input Radaream structure. If a
 * matching grid beam does not exist in the GridTable structure, then a value
 * of -1 is returned.
 **/
int GridTableFindBeam(struct GridTable *ptr, struct RadarBeam *bm) {

    int n;

    /* Loop over number of beams in GridTable structure */
    for (n=0;n<ptr->bnum;n++) {

        if (ptr->bm[n].bm !=bm->bm) continue;
        if (ptr->bm[n].frang !=bm->frang) continue;
        if (ptr->bm[n].rsep !=bm->rsep) continue;
        if (ptr->bm[n].nrang !=bm->nrang) continue;

        /* Break out of loop if GridBm parameters match RadarBeam parameters
         * including beam number, distance to first range, range separation,
         * receiver rise time, and number of range gates */
        break;

    }

    /* If beam not found then return error */
    if (n==ptr->bnum) return -1;

    /* Return index of GridBm structure that matches RadarBeam parameters */
    return n;

}



/**
 * Maps radar scan data to an equi-area grid in magnetic coordinates.
 **/
int GridTableMap(struct GridTable *ptr, struct RadarScan *scan,
                 struct RadarSite *pos, int tlen, int iflg, double alt,
                 int chisham, int magflg) {

    double freq=0,noise=0;
    double variance=0;
    double tm;
    int inx,cnt=0;
    int n,r,b;

    double v_e,p_l_e,w_l_e;

    struct GridBm *bm=NULL;

    /* Calculate center time of radar scan */
    tm=(scan->st_time+scan->ed_time)/2.0;

    /* If starting a new GridTable structure then initialize some values */
    if (ptr->status==0) {
        ptr->status=1;
        ptr->noise.mean=0;
        ptr->noise.sd=0;
        ptr->freq=0;
        ptr->nscan=0;
        GridTableZero(ptr->pnum,ptr->pnt);
        /*ptr->st_time=tlen*( (int) (tm/tlen));*/
        ptr->st_time=scan->st_time;
        ptr->ed_time=ptr->st_time+tlen;
        ptr->st_id=scan->stid;
    }

    /* Loop over number of beams in RadarScan structure */
    for (n=0;n<scan->num;n++) {

        /* Make sure beam record is valid, else skip */
        if (scan->bm[n].bm==-1) continue;

        /* Look for beam index in current GridTable structure */
        b=GridTableFindBeam(ptr,&scan->bm[n]);

        /* If beam not found, add a new beam to GridTable structure */
        if (b==-1) {
            /* map a new beam */
            b=GridTableAddBeam(ptr,pos,alt,tm,&scan->bm[n],chisham,magflg);
            if (b==-1) break;
        }

        /* Get pointer to beam in GridTable structure */
        bm=&ptr->bm[b];

        /* Loop over range gates along radar beam */
        for (r=0;r<scan->bm[n].nrang;r++) {

            /* If no scatter in beam/range gate cell then continue
             * (filters in make_grid can also manually change sct value to 0) */
            if (scan->bm[n].sct[r]==0) continue;

            /* Get velocity, power, and spectral width errors */
            v_e=scan->bm[n].rng[r].v_e;
            p_l_e=scan->bm[n].rng[r].p_l_e;
            w_l_e=scan->bm[n].rng[r].w_l_e;

            /* If velocity error is less than 100 m/s then set it to 100 m/s */
            if (v_e<v_e_min) v_e=v_e_min;

            /* If power error is less than 1 dB then set it to 1 dB */
            if (p_l_e<p_l_e_min) p_l_e=p_l_e_min;

            /* If spectral width error is less than 1 m/s then set it to 1 m/s */
            if (w_l_e<w_l_e_min) w_l_e=w_l_e_min;

            /* Get grid cell index of radar beam / gate measurement */
            inx=bm->inx[r];

            /*Add magnetic azimuth of radar beam to GridPnt structure */
            ptr->pnt[inx].azm+=bm->azm[r];

            if (iflg !=0) {
                /* If gridding in inertial frame then add north/east velocities to
                 * GridPnt structure including ival correction */
                ptr->pnt[inx].vel.median_n+=
                  -(scan->bm[n].rng[r].v+bm->ival[r])*
                  1/(v_e*v_e)*cosd(bm->azm[r]);
                ptr->pnt[inx].vel.median_e+=
                  -(scan->bm[n].rng[r].v+bm->ival[r])*
                  1/(v_e*v_e)*sind(bm->azm[r]);
            } else {
                /* Otherwise add north/east veocity components to GridPnt
                 * structure normally */
                ptr->pnt[inx].vel.median_n+=-scan->bm[n].rng[r].v*cosd(bm->azm[r])/(v_e*v_e);
                ptr->pnt[inx].vel.median_e+=-scan->bm[n].rng[r].v*sind(bm->azm[r])/(v_e*v_e);
            }

            /* Add power and spectral width values to GridPnt structure */
            ptr->pnt[inx].pwr.median+=scan->bm[n].rng[r].p_l*1/(p_l_e*p_l_e);
            ptr->pnt[inx].wdt.median+=scan->bm[n].rng[r].w_l*1/(w_l_e*w_l_e);

            /* Add velocity, power, and spectral width standard deviations to
             * GridPnt structure */
            ptr->pnt[inx].vel.sd+=1/(v_e*v_e);
            ptr->pnt[inx].pwr.sd+=1/(p_l_e*p_l_e);
            ptr->pnt[inx].wdt.sd+=1/(w_l_e*w_l_e);

            /* Update the total number of measurements contained in GridPnt
             * structure */
            ptr->pnt[inx].cnt++;

        }

    }

    /* Return an error if all beams in RadarScan structure were not considered */
    if (n !=scan->num) return -1;

    /* Loop over number of beams in RadarScan structure */
    for (n=0;n<scan->num;n++) {

        /* Make sure beam record is valid, else skip */
        if (scan->bm[n].bm==-1) continue;
        ptr->prog_id=scan->bm[n].cpid;

        /* Sum the frequency and noise values */
        freq+=scan->bm[n].freq;
        noise+=scan->bm[n].noise;

        /* Update the number of beams used for averaging frequency and noise */
        cnt++;

    }

    /* Calculate average frequency and noise of all beams in RadarScan structure */
    freq=freq/cnt;
    noise=noise/cnt;

    /* Loop over number of beams in RadarScan structure */
    for (n=0;n<scan->num;n++) {

        /* Make sure beam record is valid, else skip */
        if (scan->bm[n].bm==-1) continue;

        /* Calculate variance of noise values */
        variance+=(scan->bm[n].noise-noise)*(scan->bm[n].noise-noise);

    }

    /* Add noise mean, noise standard deviation, and frequency values to
     * GridTable structure for later averaging in GridTableTest */
    ptr->noise.mean+=noise;
    ptr->noise.sd+=sqrt(variance/cnt);
    ptr->freq+=freq;
    ptr->nscan++;

    /* Return zero if successful */
    return 0;

}
