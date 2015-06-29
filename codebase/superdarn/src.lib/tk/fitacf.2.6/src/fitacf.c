/* fitacf.c
     ========
     Author: R.J.Barnes & K.Baker
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
#include <math.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <zlib.h>
#include "rmath.h"
#include "rtypes.h"
#include "dmap.h"
#include "rprm.h"
#include "radar.h"
#include "rawdata.h"
#include "fitdata.h"
#include "fitblk.h"
#include "dofit.h"
#include "fitacfversion.h"

#define GOOSEBAY 1

/**
Free a fit block
*/
void FitACFFree(struct FitBlock *fit_blk) {
    if (fit_blk->prm.pulse !=NULL) free(fit_blk->prm.pulse);
    if (fit_blk->prm.lag[0] !=NULL) free(fit_blk->prm.lag[0]);
    if (fit_blk->prm.lag[1] !=NULL) free(fit_blk->prm.lag[1]);
    if (fit_blk->acfd !=NULL) free(fit_blk->acfd);
    if (fit_blk->xcfd !=NULL) free(fit_blk->xcfd);
}

 
/**
Creates and initializes a new fit block
*/
struct FitBlock *FitACFMake(struct RadarSite *radar_site,
                 int year) {
    int i;
    struct FitBlock *fit_blk;
    fit_blk=malloc(sizeof(struct FitBlock));
    if (fit_blk==NULL) return NULL;

    if (year < 1993) fit_blk->prm.old=1;
    for (i=0;i<3;i++) fit_blk->prm.interfer[i]=radar_site->interfer[i];
    fit_blk->prm.bmsep=radar_site->bmsep;
    fit_blk->prm.phidiff=radar_site->phidiff;
    fit_blk->prm.tdiff=radar_site->tdiff;
    fit_blk->prm.vdir=radar_site->vdir;
    fit_blk->prm.maxbeam=radar_site->maxbeam;
    fit_blk->prm.pulse=NULL;
    fit_blk->prm.lag[0]=NULL;
    fit_blk->prm.lag[1]=NULL;
    fit_blk->prm.pwr0=NULL;
    fit_blk->acfd=NULL;
    fit_blk->xcfd=NULL;
    return fit_blk;
}

/**
Fills the fit block with the radar parameters and raw data to fit
*/
int fill_fit_block(struct RadarParm *radar_prms, struct RawData *raw_data,
                    struct FitBlock *fit_blk, struct FitData *fit_data){

    int i, j, n;
    void *tmp=NULL;

    fit_blk->prm.xcf=radar_prms->xcf;
    fit_blk->prm.tfreq=radar_prms->tfreq;
    fit_blk->prm.noise=radar_prms->noise.search;
    fit_blk->prm.nrang=radar_prms->nrang;
    fit_blk->prm.smsep=radar_prms->smsep;
    fit_blk->prm.nave=radar_prms->nave;
    fit_blk->prm.mplgs=radar_prms->mplgs;
    fit_blk->prm.mpinc=radar_prms->mpinc;
    fit_blk->prm.txpl=radar_prms->txpl;
    fit_blk->prm.lagfr=radar_prms->lagfr;
    fit_blk->prm.mppul=radar_prms->mppul;
    fit_blk->prm.bmnum=radar_prms->bmnum;
    fit_blk->prm.cp=radar_prms->cp;
    fit_blk->prm.channel=radar_prms->channel;
    fit_blk->prm.offset=radar_prms->offset;  /* stereo offset */

    /* need to incorporate Sessai's code for setting the offset
         for legacy data here.
    */
    if (fit_blk->prm.pulse==NULL){
        tmp=malloc(sizeof(int)*fit_blk->prm.mppul);
    }
    else{ 
        tmp=realloc(fit_blk->prm.pulse,sizeof(int)*fit_blk->prm.mppul);
    }

    if (tmp==NULL) return -1;
    fit_blk->prm.pulse=tmp;

    for (i=0;i<fit_blk->prm.mppul;i++){
        fit_blk->prm.pulse[i]=radar_prms->pulse[i];
    }

    for (n=0;n<2;n++) {
        if (fit_blk->prm.lag[n]==NULL){
            tmp=malloc(sizeof(int)*(fit_blk->prm.mplgs+1));
        }
        else{
            tmp=realloc(fit_blk->prm.lag[n],sizeof(int)*(fit_blk->prm.mplgs+1));
        }
        if (tmp==NULL) return -1;
        fit_blk->prm.lag[n]=tmp;

        for (i=0;i<=fit_blk->prm.mplgs;i++){
            fit_blk->prm.lag[n][i]=radar_prms->lag[n][i];
        }
    }



    if (fit_blk->prm.pwr0==NULL){
        tmp=malloc(sizeof(int)*fit_blk->prm.nrang);
    }
    else{
        tmp=realloc(fit_blk->prm.pwr0,sizeof(int)*fit_blk->prm.nrang); 
    }

    if (tmp==NULL) return -1;
    fit_blk->prm.pwr0=tmp;

    if (fit_blk->acfd==NULL){
        tmp=malloc(sizeof(struct complex)*fit_blk->prm.nrang * fit_blk->prm.mplgs);
    }
    else{
        tmp=realloc(fit_blk->acfd,sizeof(struct complex)*fit_blk->prm.nrang * fit_blk->prm.mplgs);
    }

    if (tmp==NULL) return -1;
    fit_blk->acfd=tmp;

    if (fit_blk->xcfd==NULL){
        tmp=malloc(sizeof(struct complex)*fit_blk->prm.nrang*fit_blk->prm.mplgs);
    }
    else{
        tmp=realloc(fit_blk->xcfd,sizeof(struct complex)*fit_blk->prm.nrang*fit_blk->prm.mplgs);
    }

    if (tmp==NULL) return -1;
    fit_blk->xcfd=tmp;

    memset(fit_blk->acfd,0,sizeof(struct complex)*fit_blk->prm.nrang*fit_blk->prm.mplgs);   
    memset(fit_blk->xcfd,0,sizeof(struct complex)*fit_blk->prm.nrang*fit_blk->prm.mplgs);   

    for (i=0;i<fit_blk->prm.nrang;i++) {
        fit_blk->prm.pwr0[i]=raw_data->pwr0[i];
        
        if (raw_data->acfd[0] !=NULL) {
            for (j=0;j<fit_blk->prm.mplgs;j++) {
                fit_blk->acfd[i*fit_blk->prm.mplgs+j].x=raw_data->acfd[0][i*fit_blk->prm.mplgs+j];
                fit_blk->acfd[i*fit_blk->prm.mplgs+j].y=raw_data->acfd[1][i*fit_blk->prm.mplgs+j];
            }
        }
        if (raw_data->xcfd[0] !=NULL) {
            for (j=0;j<fit_blk->prm.mplgs;j++) {
                fit_blk->xcfd[i*fit_blk->prm.mplgs+j].x=raw_data->xcfd[0][i*fit_blk->prm.mplgs+j];
                fit_blk->xcfd[i*fit_blk->prm.mplgs+j].y=raw_data->xcfd[1][i*fit_blk->prm.mplgs+j];
            }
        } 
    } 
 
    return 0;
}

/**
Initializes a block for fitted data and then runs the ACF fitting 
procedure*/
int FitACF(struct RadarParm *radar_prms, struct RawData *raw_data,struct FitBlock *fit_blk, struct FitData *fit_data) {

    int fnum, goose, s;

    if (radar_prms->time.yr < 1993) {
        fit_blk->prm.old=1;
    }

    fit_data->revision.major=FITACF_MAJOR_REVISION;
    fit_data->revision.minor=FITACF_MINOR_REVISION;

    /*initialize the fitblock with prm*/
    s = fill_fit_block(radar_prms, raw_data, fit_blk, fit_data);
    if (s == -1){
        return -1;
    }

    FitSetRng(fit_data,fit_blk->prm.nrang);
    if (fit_blk->prm.xcf) {
     FitSetXrng(fit_data,fit_blk->prm.nrang);
     FitSetElv(fit_data,fit_blk->prm.nrang);
    }
    
    
    goose = (radar_prms->stid == GOOSEBAY);

    fnum = do_fit(fit_blk, 5, goose, fit_data->rng, fit_data->xrng, fit_data->elv, &fit_data->noise);

    return 0;
}
