/*
LMFIT2 main routine

Adapted by: Ashton Reimer
From code by: Keith Kotyk

ISAS
August 2016

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
    2022-06-06 Emma Bland (UNIS): Updated fit_prms->tdiff to match new hardware file format

*/

#include "lmfit_preprocessing.h"
#include "lmfit_fitting.h"
#include "lmfit2toplevel.h"
#include "lmfit_leastsquares.h"
#include "lmfit_determinations.h"
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
void LMFIT2Free(FITPRMS *fit_prms) {
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
    int rows,columns,n;
    size_t is;

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
    if ((radar_prms->offset == 0) || (radar_prms->channel < 2)) {
      fit_prms->tdiff=radar_site->tdiff[0];
    } else {
      fit_prms->tdiff=radar_site->tdiff[1];
    }
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
        for (i=0;i<fit_prms->nrang;i++) {
            if (raw_data->acfd[0] != NULL){
                for (j=0;j<fit_prms->mplgs;j++) {
                    fit_prms->acfd[i*fit_prms->mplgs+j][0]=raw_data->acfd[0][i*fit_prms->mplgs+j];
                    fit_prms->acfd[i*fit_prms->mplgs+j][1]=raw_data->acfd[1][i*fit_prms->mplgs+j];
                }
            }
        } 
    }
    if (raw_data->xcfd != NULL){
        for (i=0;i<fit_prms->nrang;i++) {
            if (raw_data->xcfd[0] != NULL){
                for (j=0;j<fit_prms->mplgs;j++) {
                    fit_prms->xcfd[i*fit_prms->mplgs+j][0]=raw_data->xcfd[0][i*fit_prms->mplgs+j];
                    fit_prms->xcfd[i*fit_prms->mplgs+j][1]=raw_data->xcfd[1][i*fit_prms->mplgs+j];
                }
            }
        }
    }
}


/**
Runs the full ACF/XCF fitting procedure and adds determinations to the FitData structure*/
int LMFIT2(FITPRMS *fit_prms, struct FitData *fit_data) {
    llist ranges, lags;
    double noise_pwr;

    ranges = llist_create(NULL,range_node_eq,0);
    lags = llist_create(compare_lags,NULL,0);

    fit_data->revision.major=MAJOR;
    fit_data->revision.minor=MINOR;

    /* Create a list of lag information. Includes pulses used to make lag and */
    /* real/imaginary components of lag (acfproc.c). LAGNODE structure definition in fit_structures.h */
    Determine_Lags(lags,fit_prms);

    /* Here we determine noise power from 10 smallest power range gates (acfproc.c)*/
    noise_pwr = ACF_cutoff_pwr(fit_prms); /*Set this to 1 for processing simulated data without the noise.*/ 

    /* Here we initialize the list of ranges with range nodes. Simply used to make processing easier */
    Fill_Range_List(fit_prms, ranges);
    /* And then we fill the lists with data from the rawacf structure */
    /* This also calculates the initial sampling uncertainty error estimate for lag0 power */
    /* In here we set phase error to 1 for now. It will be updated after the power is fit */

    /* Check to make sure we actually have data in range list, or else return */
    if (llist_size(ranges) == 0) 
    {
        llist_destroy(lags,TRUE,free);
        llist_destroy(ranges,TRUE,free_range_node);
        return -1; /* -1 means there was a problem with fitting due to no range data*/
    }

    /*llist_for_each_arg(ranges,(node_func_arg)Fill_Data_Lists_For_Range,lags,fit_prms)*/

    llist_for_each_arg(ranges,(node_func_arg)Fill_Data_Lists_For_Range,lags,fit_prms);

    /* Tx overlapped data is removed from consideration */
    /* Ain't nobody got time for that */
    Filter_TX_Overlap(ranges, lags, fit_prms); /*Comment this out for simulted data without TX overlap*/ 

    /* Check to see if data lists have any data in them, if not, delete the range node */
    Check_Range_Nodes(ranges);

    /* With fitted lag0 power, we can now get a rough estimate of the error in magnitude */
    /* First, estimate the self-clutter */
    Estimate_Self_Clutter(ranges,fit_prms); 

    /* The first order ACF lag estimate variances need to be estimated */
    Estimate_First_Order_Error(ranges,fit_prms,noise_pwr);

    /* Then we can do a fit */
    ACF_Fit(ranges,fit_prms);

    /* Now we can better estimate the ACF lag variance */
    Estimate_Re_Im_Error(ranges,fit_prms,noise_pwr);

    /* And then do a final fit */
    ACF_Fit(ranges,fit_prms);

    /* Now that the ACF is fitted, we can fit the XCF */
    /* XCF_Phase_Fit(ranges,fit_prms); */

    /*Now the fits are completed, we can make our final determinations from those fits*/
    ACF_Determinations(ranges, fit_prms, fit_data, noise_pwr);

    /* Free memory */
    llist_destroy(lags,TRUE,free);
    llist_destroy(ranges,TRUE,free_range_node);

    return 0;
}
