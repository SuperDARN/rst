/*Copyright (C) 2016  SuperDARN Canada

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.*/
/*
FITACF main routine

Keith Kotyk
ISAS
July 2015

*/

#include "preprocessing.h"
#include "fitting.h"
#include "fitacftoplevel.h"
#include "leastsquares.h"
#include "determinations.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>

/**
Free a fit parameter structure
*/
void FitACFFree(FITPRMS *fit_prms) {
    if (fit_prms->pulse !=NULL) free(fit_prms->pulse);
    if (fit_prms->lag[0] !=NULL) free(fit_prms->lag[0]);
    if (fit_prms->lag[1] !=NULL) free(fit_prms->lag[1]);
    if (fit_prms->pwr0 !=NULL) free(fit_prms->pwr0);
    if (fit_prms->acfd !=NULL){
		free(fit_prms->acfd);
	}
	if (fit_prms->xcfd !=NULL){
		free(fit_prms->xcfd);
	}
    free(fit_prms);

}

/**
Allocates a new data structure for fit parameters.
*/
FITPRMS* Allocate_Fit_Prm(struct RadarParm *radar_prms){

    FITPRMS *fit_prms;
    int columns,n;
    size_t is, rows;


    fit_prms=malloc(sizeof(FITPRMS));

    if (fit_prms==NULL){
        fprintf(stderr, "COULD NOT ALLOCATE fit_prms\n");
        return NULL;
    }

    fit_prms->pulse=malloc(sizeof(*fit_prms->pulse)*radar_prms->mppul);

    if (fit_prms->pulse==NULL){
        fprintf(stderr, "COULD NOT ALLOCATE fit_prms->pulse\n");
        return NULL;
    }

    for (n=0;n<2;n++) {

        fit_prms->lag[n]=malloc(sizeof(*fit_prms->lag[n])*(radar_prms->mplgs+1));

        if (fit_prms->lag[n]==NULL){
            fprintf(stderr, "COULD NOT ALLOCATE fit_prms->lag[%d]\n",n);
            return NULL;
        }
    }


    fit_prms->pwr0=malloc(sizeof(*fit_prms->pwr0)*radar_prms->nrang);

    if (fit_prms->pwr0==NULL){
        fprintf(stderr, "COULD NOT ALLOCATE fit_prms->pwr0\n");
        return NULL;
    }

    rows = radar_prms->nrang * radar_prms->mplgs;
    columns = 2;

    fit_prms->acfd = malloc(rows * sizeof(*fit_prms->acfd) + (rows * (columns * sizeof(**fit_prms->acfd))));

    if (fit_prms->acfd==NULL){
        fprintf(stderr, "COULD NOT ALLOCATE fit_prms->acfd\n");
        return NULL;
    }

    fit_prms->acfd[0] = (double*)(fit_prms->acfd + rows);
    for(is=0;is<rows;is++){
        fit_prms->acfd[is] = (double*)(fit_prms->acfd[0] + (is * columns));
    }


    fit_prms->xcfd = malloc(rows * sizeof(*fit_prms->xcfd) + (rows * (columns * sizeof(**fit_prms->xcfd))));

    if (fit_prms->xcfd==NULL){
        fprintf(stderr, "COULD NOT ALLOCATE fit_prms->xcfd\n");
        return NULL;
    }


    fit_prms->xcfd[0] = (double*)(fit_prms->xcfd + rows);
    for(is=0;is<rows;is++){
        fit_prms->xcfd[is] = (double*)(fit_prms->xcfd[0] + (is * columns));
    }

    return fit_prms;

}

/**
Copies fitting parameters from raw data and radar sites into the data structure
used in the ACF fitting procedure.
*/
void Copy_Fitting_Prms(struct RadarSite *radar_site, struct RadarParm *radar_prms,
				   struct RawData *raw_data,FITPRMS *fit_prms){

    int i, j, n;


    if (radar_prms->time.yr < 1993){
        fit_prms->old=1;
    }
    else
    {
        fit_prms->old=0;
    }

    fit_prms->interfer_x=radar_site->interfer[0];
    fit_prms->interfer_y=radar_site->interfer[1];
    fit_prms->interfer_z=radar_site->interfer[2];
    fit_prms->bmsep=radar_site->bmsep;
    fit_prms->phidiff=radar_site->phidiff;
    fit_prms->tdiff=radar_site->tdiff;
    fit_prms->vdir=radar_site->vdir;
    fit_prms->maxbeam=radar_site->maxbeam;
    fit_prms->xcf=radar_prms->xcf;
    fit_prms->tfreq=radar_prms->tfreq;
    fit_prms->noise=radar_prms->noise.search;
    fit_prms->nrang=radar_prms->nrang;
    fit_prms->smsep=radar_prms->smsep;
    fit_prms->nave=radar_prms->nave;
    fit_prms->mplgs=radar_prms->mplgs;
    fit_prms->mpinc=radar_prms->mpinc;
    fit_prms->txpl=radar_prms->txpl;
    fit_prms->lagfr=radar_prms->lagfr;
    fit_prms->mppul=radar_prms->mppul;
    fit_prms->bmnum=radar_prms->bmnum;
    fit_prms->cp=radar_prms->cp;
    fit_prms->channel=radar_prms->channel;
    fit_prms->offset=radar_prms->offset;  /* stereo offset */
    fit_prms->time.yr = radar_prms->time.yr;
    fit_prms->time.mo = radar_prms->time.mo;
    fit_prms->time.dy = radar_prms->time.dy;
    fit_prms->time.hr = radar_prms->time.hr;
    fit_prms->time.mt = radar_prms->time.mt;
    fit_prms->time.sc = radar_prms->time.sc;
    fit_prms->time.us = radar_prms->time.us;




    for (i=0;i<fit_prms->mppul;i++){
        fit_prms->pulse[i]=radar_prms->pulse[i];
    }

    for (n=0;n<2;n++) {
        for (i=0;i<=fit_prms->mplgs;i++){
            fit_prms->lag[n][i]=radar_prms->lag[n][i];
        }
    }


    for (i=0;i<fit_prms->nrang;i++) {
        fit_prms->pwr0[i]=raw_data->pwr0[i];
    }




    if (raw_data->acfd != NULL){
        if (*(raw_data->acfd) != NULL){
            for (i=0;i<fit_prms->nrang;i++) {
                for (j=0;j<fit_prms->mplgs;j++) {
                    fit_prms->acfd[i*fit_prms->mplgs+j][0]=raw_data->acfd[0][i*fit_prms->mplgs+j];
                    fit_prms->acfd[i*fit_prms->mplgs+j][1]=raw_data->acfd[1][i*fit_prms->mplgs+j];
                }
            }
        }
        else{/*If second pointer is NULL then fill with zeros*/
            for (i=0;i<fit_prms->nrang;i++) {
                for (j=0;j<fit_prms->mplgs;j++) {
                    fit_prms->acfd[i*fit_prms->mplgs+j][0]=0;
                    fit_prms->acfd[i*fit_prms->mplgs+j][1]=0;
                }
            }
        }
    }
    else{ /*If first pointer is NULL then fill with zeros*/
        for (i=0;i<fit_prms->nrang;i++) {
            for (j=0;j<fit_prms->mplgs;j++) {
                fit_prms->acfd[i*fit_prms->mplgs+j][0]=0;
                fit_prms->acfd[i*fit_prms->mplgs+j][1]=0;
            }
        }
    }


    if (raw_data->xcfd != NULL){
        if(*(raw_data->xcfd) != NULL){
            for (i=0;i<fit_prms->nrang;i++) {
                for (j=0;j<fit_prms->mplgs;j++) {
                    fit_prms->xcfd[i*fit_prms->mplgs+j][0]=raw_data->xcfd[0][i*fit_prms->mplgs+j];
                    fit_prms->xcfd[i*fit_prms->mplgs+j][1]=raw_data->xcfd[1][i*fit_prms->mplgs+j];
                }
            }
        }
        else{/*If second pointer is NULL then fill with zeros*/
            for (i=0;i<fit_prms->nrang;i++) {
                for (j=0;j<fit_prms->mplgs;j++) {
                    fit_prms->xcfd[i*fit_prms->mplgs+j][0]=0;
                    fit_prms->xcfd[i*fit_prms->mplgs+j][1]=0;
                }
            }
        }
    }
    else{ /*If first pointer is NULL then fill with zeros*/
        for (i=0;i<fit_prms->nrang;i++) {
            for (j=0;j<fit_prms->mplgs;j++) {
                fit_prms->xcfd[i*fit_prms->mplgs+j][0]=0;
                fit_prms->xcfd[i*fit_prms->mplgs+j][1]=0;
            }
        }
    }
}

/**
Runs the full ACF/XCF fitting procedure and adds determinations to the FitData structure*/
int FitACF(FITPRMS *fit_prms, struct FitData *fit_data) {

    llist ranges, lags;
    double noise_pwr;

    fit_data->revision.major=MAJOR;
    fit_data->revision.minor=MINOR;


    ranges = llist_create(NULL,range_node_eq,0);
    lags = llist_create(compare_lags,NULL,0);

    /*Create a list of lag information. More informative and reliable than the lag array
    in the raw data
    */
    Determine_Lags(lags,fit_prms);
    /*llist_for_each_arg(lags,(node_func_arg)print_lag_node, fit_prms, NULL);*/

    /*Here we determine the fluctuation level for which ACFs are pure noise*/

    /*Set this to 1 for processing simulated data without the noise.*/
    /*We check number of averages < 0 since this will cause invalid
    division in the noise calculation*/
    noise_pwr = (fit_prms->nave <= 0) ? 1.0 : ACF_cutoff_pwr(fit_prms);
   /* noise_pwr = 1; */

    /*Here we fill the list of ranges with range nodes.*/
    Fill_Range_List(fit_prms, ranges);

    /*For each range we find the CRI of each pulse*/
    llist_for_each_arg(ranges,(node_func_arg)Find_CRI,fit_prms,NULL);  	/*Comment this out for simulted data without CRI*/

    /*Now that we have CRI, we find alpha for each range*/
    llist_for_each_arg(ranges,(node_func_arg)Find_Alpha,lags,fit_prms);

    /*Each range node has its ACF power, ACF phase, and XCF phase(elevation) data lists filled*/
    llist_for_each_arg(ranges,(node_func_arg)Fill_Data_Lists_For_Range,lags,fit_prms);

    /*llist_for_each_arg(ranges,(node_func_arg)print_uncorrected_phase,fit_prms, NULL);*/
    /* llist_for_each(ranges,print_range_node);*/

    /*Tx overlapped data is removed from consideration*/
    Filter_TX_Overlap(ranges, lags, fit_prms); 	/*Comment this out for simulted data without TX overlap*/
    /*llist_for_each_arg(ranges,(node_func_arg)print_range_node,fit_prms,NULL);*/
    /*Criterion is applied to filter low power lags that are considered too close to
    statistical fluctuations*/
    llist_for_each_arg(ranges,(node_func_arg)Filter_Low_Pwr_Lags,fit_prms,NULL);

    /*Criterion is applied to filter ranges that hold no merit*/
    Filter_Bad_ACFs(fit_prms,ranges,noise_pwr);

    /*At this point all data is now processed and valuable so we perform power fits.
    The phase fitting stage is dependant on fitted power and must be done first*/
    llist_for_each(ranges,(node_func)Power_Fits);

    /*We perform the phase fits for velocity and elevation. The ACF phase fit improves the
    fit of the XCF phase fit and must be done first*/
    ACF_Phase_Fit(ranges,fit_prms);

    XCF_Phase_Fit(ranges,fit_prms);

    /*llist_for_each_arg(ranges,(node_func_arg)print_range_node,fit_prms,NULL);*/

    /*Now the fits are completed, we can make our final determinations from those fits*/
    ACF_Determinations(ranges, fit_prms, fit_data, noise_pwr);

    llist_destroy(lags,TRUE,free);
    llist_destroy(ranges,TRUE,free_range_node);



    return 0;
}
