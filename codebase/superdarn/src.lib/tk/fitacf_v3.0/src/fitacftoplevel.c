/*
 FITACF main routine

 Copyright (c) 2016 University of Saskatchewan
 Author: Keith Kotyk


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
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 Modifications:
     E.G.Thomas 2021-08: added support for bmoff parameter and multi-channel tdiff values
  
*/



#include "rtypes.h"
#include "llist.h"
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
 * @brief      Frees raw record data.
 *
 * @param      fit_prms  The FITPRM struct holding rawacf record info.
 */
void FitacfFree(FITPRMS *fit_prms) {
  if (fit_prms->pulse != NULL) free(fit_prms->pulse);
  if (fit_prms->lag[0] != NULL) free(fit_prms->lag[0]);
  if (fit_prms->lag[1] != NULL) free(fit_prms->lag[1]);
  if (fit_prms->pwr0 != NULL) free(fit_prms->pwr0);
  if (fit_prms->acfd != NULL){
    free(fit_prms->acfd);
  }
  if (fit_prms->xcfd != NULL){
    free(fit_prms->xcfd);
  }
  free(fit_prms);

}

/**
 * @brief      Allocates space for the FITPRMS struct pointers.
 *
 * @param      radar_prms  All raw data radar parameters.
 * @param      fit_prms    The FITPRM struct holding rawacf record info.
 *
 * @return     0 on success, -1 on error.
 */
int Allocate_Fit_Prm(struct RadarParm *radar_prms, FITPRMS *fit_prms)
{
  int columns, n;
  size_t is, rows;

  if(fit_prms == NULL){
    fprintf(stderr, "fit_prms WAS NOT ALLOCATED CORRECTLY\n");
    return -1;
  }

  /* Allocate space for an integer pointer */
  fit_prms->pulse = realloc(fit_prms->pulse, sizeof(*fit_prms->pulse) * radar_prms->mppul);
  memset(fit_prms->pulse, 0, sizeof(*fit_prms->pulse) * radar_prms->mppul);

  if(fit_prms->pulse == NULL){
    fprintf(stderr, "COULD NOT ALLOCATE fit_prms->pulse\n");
    return -1;
  }

  /* Allocate space for the integer pointer holding the lag data */
  for(n=0; n<2; n++){
    fit_prms->lag[n] = realloc(fit_prms->lag[n], sizeof(*fit_prms->lag[n]) * (radar_prms->mplgs+1));
    memset(fit_prms->lag[n], 0, sizeof(*fit_prms->lag[n]) * (radar_prms->mplgs+1));

    if(fit_prms->lag[n] == NULL){
      fprintf(stderr, "COULD NOT ALLOCATE fit_prms->lag[%d]\n",n);
      return -1;
    }
  }

  /* Allocate space for the double pointer holding the zero-lag power */
  fit_prms->pwr0 = realloc(fit_prms->pwr0, sizeof(*fit_prms->pwr0) * radar_prms->nrang);
  memset(fit_prms->pwr0, 0, sizeof(*fit_prms->pwr0) * radar_prms->nrang);

  if(fit_prms->pwr0 == NULL){
    fprintf(stderr, "COULD NOT ALLOCATE fit_prms->pwr0\n");
    return -1;
  }

  rows = radar_prms->nrang * radar_prms->mplgs;
  columns = 2;

  /* Allocate space for the acfd double pointer using a contiguous set memory. Space for all row
   pointers as well as data are allocated all at once and the row pointers are assigned. */
  fit_prms->acfd = realloc(fit_prms->acfd, (sizeof(*fit_prms->acfd) + columns * sizeof(**fit_prms->acfd)) * rows);
  memset(fit_prms->acfd, 0, (sizeof(*fit_prms->acfd) + columns * sizeof(**fit_prms->acfd)) * rows);

  if(fit_prms->acfd == NULL){
    fprintf(stderr, "COULD NOT ALLOCATE fit_prms->acfd\n");
    return -1;
  }

  fit_prms->acfd[0] = (double *)(fit_prms->acfd + rows);
  for(is=0; is<rows; is++) {
    fit_prms->acfd[is] = (double *)(fit_prms->acfd[0] + (is * columns));
  }

  /* Allocate space for the xcfd double pointer using a contiguous set memory. Space for all row
   pointers as well as data are allocated all at once and the row pointers are assigned. */
  fit_prms->xcfd = realloc(fit_prms->xcfd, (sizeof(*fit_prms->xcfd) + columns * sizeof(**fit_prms->xcfd)) * rows);
  memset(fit_prms->xcfd, 0, (sizeof(*fit_prms->xcfd) + columns * sizeof(**fit_prms->xcfd)) * rows);

  if(fit_prms->xcfd == NULL){
    fprintf(stderr, "COULD NOT ALLOCATE fit_prms->xcfd\n");
    return -1;
  }

  fit_prms->xcfd[0] = (double *)(fit_prms->xcfd + rows);
  for(is=0; is<rows; is++) {
    fit_prms->xcfd[is] = (double *)(fit_prms->xcfd[0] + (is * columns));
  }
  /* All the Fitacf parameter pointers were successfully allocated */
  /* and initialised to zero                                       */
  return 0;
}

/**
 * @brief      Copies fitting parameters from raw data and radar sites into the FITPRMS struct.
 *
 * @param      radar_site  The radar site info struct.
 * @param      radar_prms  The radar raw data parameters struct.
 * @param      raw_data    The raw record data.
 * @param      fit_prms    The FITPRM struct holding rawacf record info.
 *
 * This function copies the info needed in the fitting process into a common struct. Parameters
 * from the radar site info, raw record, and raw data are copied into a common fit parameters
 * struct.
 */
void Copy_Fitting_Prms(struct RadarSite *radar_site, struct RadarParm *radar_prms,
           struct RawData *raw_data,FITPRMS *fit_prms){

  int i, j, n;

  /* Use the year (1993) to determine whether this data follows the old */
  /* format or the current format                                       */
  fit_prms->old = 1 ? (radar_prms->time.yr < 1993) : 0;

  /* Initialise the radar parameters in the FitACF parameter structure */
  fit_prms->interfer[0]=radar_site->interfer[0];
  fit_prms->interfer[1]=radar_site->interfer[1];
  fit_prms->interfer[2]=radar_site->interfer[2];
  fit_prms->bmoff=radar_site->bmoff;
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
 * @brief      Calls the overall stack of fitting operations for a set of raw data.
 *
 * @param      fit_prms  The FITPRM struct holding rawacf record info.
 * @param      fit_data  A pointer to a struct that holds parameters that have been extracted from
 *                       fitted data.
 *
 * @return     0 on success.
 */
int Fitacf(FITPRMS *fit_prms, struct FitData *fit_data, int elv_version) {

  llist ranges, lags;
  double noise_pwr;
  int list_null_flag = LLIST_SUCCESS;
  llist_node node;
  fit_data->revision.major=MAJOR;
  fit_data->revision.minor=MINOR;

  ranges = llist_create(NULL,range_node_eq,0);
  lags = llist_create(compare_lags,NULL,0);

  /* Create a list of lag information. More informative and reliable than the
     lag array in the raw data
  */
  Determine_Lags(lags,fit_prms);

  /*Here we determine the fluctuation level for which ACFs are pure noise*/

  /*Set this to 1 for processing simulated data without the noise.*/
  /*We check number of averages < 0 since this will cause invalid
  division in the noise calculation*/
  noise_pwr = (fit_prms->nave <= 0) ? 1.0 : ACF_cutoff_pwr(fit_prms);

  /*Here we fill the list of ranges with range nodes.*/
  Fill_Range_List(fit_prms, ranges);

  /*For each range we find the CRI of each pulse*/
  /*Comment this out for simulated data without CRI*/
  llist_reset_iter(ranges);
  llist_get_iter(ranges, &node);
  while(node != NULL && list_null_flag == LLIST_SUCCESS)
  {
    Find_CRI(node, fit_prms);
    Find_Alpha(node, lags, fit_prms);
    Fill_Data_Lists_For_Range(node, lags, fit_prms);
    list_null_flag = llist_go_next(ranges);
    llist_get_iter(ranges, &node); 
  }
  llist_reset_iter(ranges);
  list_null_flag = LLIST_SUCCESS;
  /*Now that we have CRI, we find alpha for each range*/

  /*Tx overlapped data is removed from consideration*/
  /*Comment this out for simulated data without TX overlap*/
  Filter_TX_Overlap(ranges, lags, fit_prms);

  llist_reset_iter(ranges);
  llist_get_iter(ranges, &node);
  while(node != NULL && list_null_flag == LLIST_SUCCESS)
  {
    Filter_Low_Pwr_Lags(node, fit_prms);
    list_null_flag = llist_go_next(ranges);
    llist_get_iter(ranges, &node); 
  }
  llist_reset_iter(ranges);
  list_null_flag = LLIST_SUCCESS;

  /*Criterion is applied to filter low power lags that are considered too close to
  statistical fluctuations*/

  /*Criterion is applied to filter ranges that hold no merit*/
  Filter_Bad_ACFs(fit_prms,ranges,noise_pwr);

  /*At this point all remaining data are meaningful so we perform power fits.
  The phase fitting stage is dependent on fitted power so that the power fits must be done first.
  This is happening because the phase variance used as weighting coefficients depends on
  correlation coefficient at a given lag, |R(tau)|, as sqrt((R^(-2)-1)/N_ave). 
  While the theoretical value of |R(tau)| never exceeds unity, its experimetnal estimate 
  can be larger than 1 due to either statistical fluctuations or contribution from 
  cros-range interference so that the variance estimate becomes imaginary, i.e. meaningless.*/
  llist_reset_iter(ranges);
  llist_get_iter(ranges, &node);
  while(node != NULL && list_null_flag == LLIST_SUCCESS)
  {
    Power_Fits(node);
    list_null_flag = llist_go_next(ranges);
    llist_get_iter(ranges, &node); 
  }
  llist_reset_iter(ranges);
  list_null_flag = LLIST_SUCCESS;

  /*We perform the phase fits for velocity and elevation. The ACF phase fit improves the
  fit of the XCF phase fit and must be done first*/
  ACF_Phase_Fit(ranges,fit_prms);

  Filter_Bad_Fits(ranges);

  XCF_Phase_Fit(ranges,fit_prms);

  /*Now the fits are completed, we can make our final determinations from those fits*/
  ACF_Determinations(ranges, fit_prms, fit_data, noise_pwr, elv_version);

  llist_destroy(lags,TRUE,free);
  llist_destroy(ranges,TRUE,free_range_node);

  return 0;
}
