/*
 ACF Processing main functions

 Copyright (c) 2016  University of Saskatchewan
 author: Keith Kotyk


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
     2020-03-11 Marina Schmidt (University of Saskatchewan) removed all defined constants and included rmath.h 
     2021-05-26 Pasha Ponomarenko (University of Saskatchewan) check that the search noise
                is nonzero before using it to replace the skynoise when min_pwr < 1

*/

#include "rtypes.h"
#include "rmath.h"
#include "preprocessing.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

/********************LIST NODE STUFF*********************/

/**
 * @brief      Allocates a new RANGENODE struct to be placed in a list.
 *
 * @param[in]  range     The range number of this data.
 * @param      fit_prms  The parameters to use for fitting obtained from the rawacf record.
 *
 * @return     A pointer to a newly allocated and initialized RANGENODE struct.
 *
 * This struct contains all the info and data associated with a particular range during the fitting
 * process. These structs are allocated and treated as nodes in a linked list for easy pruning and
 * function mapping.
 */
RANGENODE* new_range_node(int range, FITPRMS *fit_prms){
  RANGENODE* new_node;
  new_node = malloc(sizeof(*new_node));
  new_node->range = range;
  new_node->CRI = calloc(fit_prms->mppul,sizeof(*new_node->CRI));
  new_node->refrc_idx = 1;
  new_node->alpha_2 = NULL;
  new_node->phases = NULL;
  new_node->pwrs = NULL;
  new_node->elev = NULL;
  new_node->l_pwr_fit = new_fit_data();
  new_node->q_pwr_fit = new_fit_data();
  new_node->l_pwr_fit_err = new_fit_data();
  new_node->q_pwr_fit_err = new_fit_data();
  new_node->phase_fit = new_fit_data();
  new_node->elev_fit = new_fit_data();
  return new_node;

}

/**
 * @brief      Allocates a new PWRNODE struct to be placed in a list.
 *
 * @param[in]  range     The range number for this power data.
 * @param[in]  alpha_2   The squared alpha value for Bendat and Piersol calculation of power.
 * @param      lag       The lag number of this ACF power value.
 * @param      fit_prms  The parameters to use for fitting obtained from the rawacf record.
 *
 * @return     A pointer to a newly allocated and initialized PWRNODE struct.
 *
 * This struct contains all the info related to an ACF power data point. These structs are allocated
 * and treated as nodes in a linked list for easy pruning and function mapping.
 */
PWRNODE* new_pwr_node(int range, double alpha_2, LAGNODE* lag, FITPRMS *fit_prms){
  PWRNODE* new_pwr_node = NULL;
  double R,R_norm,R_norm_2,real,imag,inverse_alpha_2;



  real = fit_prms->acfd[range * fit_prms->mplgs + lag->lag_idx][0];
  imag = fit_prms->acfd[range * fit_prms->mplgs + lag->lag_idx][1];
  R = sqrt(real * real + imag * imag);

  if(R <= 0.0) return NULL;
  R_norm = R/fit_prms->pwr0[range];
  R_norm_2 = R_norm * R_norm;

  inverse_alpha_2 = 1 / alpha_2;

  new_pwr_node = malloc(sizeof(*new_pwr_node));

  new_pwr_node->lag_idx = lag->lag_idx;
  new_pwr_node->ln_pwr = log(R);
  new_pwr_node->sigma = fit_prms->pwr0[range] * sqrt((R_norm_2 + inverse_alpha_2)/(2 * fit_prms->nave));
  new_pwr_node->t = lag->lag_num * fit_prms->mpinc * 1.0e-6;

  new_pwr_node->alpha_2 = alpha_2;

  return new_pwr_node;

}

/**
 * @brief      Allocates a new PHASENODE struct to be placed in a list of ACF phase values.
 *
 * @param[in]  range     The range number for this power data.
 * @param[in]  alpha_2   The squared alpha value for Bendat and Piersol calculation of phase.
 * @param      lag       The lag number of this ACF phase value.
 * @param      fit_prms  The parameters to use for fitting obtained from the rawacf record.
 *
 * @return     A pointer to a newly allocated and initialized PHASENODE struct.
 *
 * This struct contains all the info related to an XCF phase data point. These structs are
 * allocated and treated as nodes in a linked list for easy pruning and function mapping.
 */
PHASENODE* new_phase_node(int range, double alpha_2, LAGNODE* lag, FITPRMS *fit_prms){
  PHASENODE* new_phase_node;
  double real,imag;

  new_phase_node = malloc(sizeof(*new_phase_node));

  real = fit_prms->acfd[range * fit_prms->mplgs + lag->lag_idx][0];
  imag = fit_prms->acfd[range * fit_prms->mplgs + lag->lag_idx][1];

  new_phase_node->lag_idx = lag->lag_idx;
  new_phase_node->phi = atan2(imag,real);

  new_phase_node->sigma = 0.0;

  new_phase_node->t = lag->lag_num * fit_prms->mpinc * 1.0e-6;

  new_phase_node->alpha_2 = alpha_2;

  return new_phase_node;

}

/**
 * @brief      Allocates a new PHASENODE struct to be placed in a list of XCF phase values.
 *
 * @param[in]  range     The range number for this power data.
 * @param[in]  alpha_2   The squared alpha value for Bendat and Piersol calculation of phase.
 * @param      lag       The lag number of this XCF phase value.
 * @param      fit_prms  The parameters to use for fitting obtained from the rawacf record.
 *
 * @return     A pointer to a newly allocated and initialized PHASENODE struct.
 *
 * This struct contains all the info related to an XCF phase data point. These structs are
 * allocated and treated as nodes in a linked list for easy pruning and function mapping.
 */
PHASENODE* new_elev_node(int range, double alpha_2, LAGNODE* lag, FITPRMS *fit_prms){
    PHASENODE* new_elev_node;
    double real,imag;

    new_elev_node = malloc(sizeof(*new_elev_node));

    real = fit_prms->xcfd[range * fit_prms->mplgs + lag->lag_idx][0];
    imag = fit_prms->xcfd[range * fit_prms->mplgs + lag->lag_idx][1];

    new_elev_node->lag_idx = lag->lag_idx;
    new_elev_node->phi = atan2(imag,real);

    new_elev_node->sigma = 0.0;

    new_elev_node->t = lag->lag_num * fit_prms->mpinc * 1.0e-6;

    new_elev_node->alpha_2 = alpha_2;

    return new_elev_node;

}



/**
 * @brief      Frees the memory of an allocated RANGENODE struct.
 *
 * @param[in]  range  The range list node.
 *
 * This function is meant to be mapped to a list when pruning or cleaning up memory after
 * processing.
 */
void free_range_node(llist_node range){
  RANGENODE* range_node;

  range_node = (RANGENODE*) range;

  if(range_node->CRI !=NULL){
    free(range_node->CRI);
  }

  if(range_node->alpha_2 != NULL){
    llist_destroy(range_node->alpha_2,TRUE,free);
  }

  if(range_node->phases != NULL){
    llist_destroy(range_node->phases,TRUE,free);
  }

  if(range_node->pwrs != NULL){
    llist_destroy(range_node->pwrs,TRUE,free);
  }

  if(range_node->elev != NULL){
    llist_destroy(range_node->elev,TRUE,free);
  }

  if(range_node->l_pwr_fit != NULL){
    free_fit_data(range_node->l_pwr_fit);
  }

  if(range_node->q_pwr_fit != NULL){
    free_fit_data(range_node->q_pwr_fit);
  }

  if(range_node->l_pwr_fit_err != NULL){
    free_fit_data(range_node->l_pwr_fit_err);
  }

  if(range_node->q_pwr_fit_err != NULL){
    free_fit_data(range_node->q_pwr_fit_err);
  }

  if(range_node->phase_fit != NULL){
    free_fit_data(range_node->phase_fit);
  }

  if(range_node->elev_fit != NULL){
    free_fit_data(range_node->elev_fit);
  }

  if(range_node != NULL){
    free(range_node);
  }


}








/*++++++++++++++++++++CALLBACKS FOR COMPARISONS+++++++++++++++++++++*/


/**
 * @brief      Comparison function for qsort.
 *
 * @param[in]  x     Pointer to the first qsort element.
 * @param[in]  y     Pointer to the second qsort element.
 *
 * @return     An int representing one of three states of comparison result.
 *
 * Input parameters are cast to double as qsort is used on the type in this case.
 */
int cmp_dbl(const void *x,const void *y) {
  double *a,*b;
  a=(double *) x;
  b=(double *) y;
  if (*a > *b) return 1;
  else if (*a == *b) return 0;
  else return -1;
}

/**
 * @brief      Comparison of two lag numbers.
 *
 * @param[in]  first   The first lag list node.
 * @param[in]  second  The second lag list node.
 *
 * @return     An int representing one of three states of comparison result.
 */
int compare_lags(llist_node first,llist_node second){
  LAGNODE* lag1;
  LAGNODE* lag2;
  lag1 = (LAGNODE*)(first);
  lag2 = (LAGNODE*)(second);

  if(lag1->lag_num < lag2->lag_num){
    return -1;
  }
  else if(lag1->lag_num == lag2->lag_num){
    return 0;
  }
  else{
    return 1;
  }
}


/**
 * @brief      Compares if the lag index of a power node is equal to an int stored in a list.
 *
 * @param[in]  node  The current ACF power node.
 * @param[in]  cmp   The int value stored in a list.
 *
 * @return     True or false based on equality.
 *
 * This function is used to find a particular node when iterating over a list to when searching for
 * a ACf power lag.
 */
bool pwr_node_eq(llist_node node,llist_node cmp){
  PWRNODE* pwr_node;
  int value;

  pwr_node = (PWRNODE*) node;
  value = *(int*)(cmp);

  return (pwr_node->lag_idx == value);

}

/**
 * @brief      Compares if the lag index of a phase node is equal to an int stored in a list.
 *
 * @param[in]  node  The current ACF/XCF phase node.
 * @param[in]  cmp   The int value stored in a list.
 *
 * @return     True or false based on equality.
 *
 * This function is used to find a particular node when iterating over a list to when searching for
 * a ACF/XCF phase lag.
 */
bool phase_node_eq(llist_node node,llist_node cmp){
  PHASENODE* phase_node;
  int value;

  phase_node = (PHASENODE*) node;
  value = *(int*)(cmp);

  return (phase_node->lag_idx == value);

}

/**
 * @brief      Used to compare doubles in a stored in a list.
 *
 * @param[in]  first   The first list node.
 * @param[in]  second  The second list node.
 *
 * @return     An int representing one of three states of comparison result.
 */
int compare_doubles(llist_node first,llist_node second){
  if(*(double*)(first) < *(double*)(second)){
    return -1;
  }
  else if(*(double*)(first) < *(double*)(second)){
    return 0;
  }
  else{
    return 1;
  }
}


/**
 * @brief      Used to compare ints in a stored in a list.
 *
 * @param[in]  first   The first list node.
 * @param[in]  second  The second list node.
 *
 * @return     An int representing one of three states of comparison result.
 */
int compare_ints(llist_node first,llist_node second){
  if(*(int*)(first) < *(int*)(second)){
    return -1;
  }
  else if(*(int*)(first) < *(int*)(second)){
    return 0;
  }
  else{
    return 1;
  }
}

/**
 * @brief      Used to find a sample number in a list.
 *
 * @param[in]  node  The sample number list node.
 * @param[in]  cmp   A sample number to compare against.
 *
 * @return     True or false depending on if the two list values are equal.
 *
 * This function is used to find a particular sample in a list when iterating over it.
 */
bool sample_node_eq(llist_node node,llist_node cmp){
  int smp;
  int value;

  smp = *(int*)(node);
  value = *(int*)(cmp);

  return (smp == value);

}

/**
 * @brief      Used to find a range node in a list.
 *
 * @param[in]  node  The range node.
 * @param[in]  cmp   The range number from a list to compare against.
 *
 * @return     True or false based on equality.
 *
 * This function is used to find a particular range node in a list by comparing against
 * range number when iterating.
 */
bool range_node_eq(llist_node node,llist_node cmp){
  RANGENODE* range_node;
  int value;

  range_node = (RANGENODE*) node;
  value = *(int*)(cmp);

  return (range_node->range == value);

}

/**
 * @brief      Used to find a value of alpha at a particular lag in a list.
 *
 * @param[in]  node  The alpha node.
 * @param[in]  cmp   The lag value to compare to.
 *
 * @return     True or false depending on equality.
 *
 * This function is used to find a particular alpha node in a list based on its associated lag
 * number when iterating over the list.
 */
bool alpha_node_eq(llist_node node,llist_node cmp){
  ALPHANODE* alpha_node;
  int value;

  alpha_node = (ALPHANODE*) node;
  value = *(int*)(cmp);

  return (alpha_node->lag_idx == value);

}


/*++++++++++++++++++++PRINT CALLBACKS FOR DIFFERENT NODES+++++++++++++++++++*/


void print_node(llist_node node){
  fprintf(stderr,"sample %d\n",*(int*)(node));
}

void print_alpha_node(llist_node node, FILE* fp){
  ALPHANODE* alpha_2;
  alpha_2 = (ALPHANODE*)(node);
  fprintf(fp,"%f ",alpha_2->alpha_2);
}

void print_lag_node(llist_node node,FITPRMS* fit_prms){
  LAGNODE* lag;
  lag = (LAGNODE*)(node);
  FILE* fp;

  fp = fopen("lags.txt","a");
  fprintf(fp,"TIME %d-%02d-%02dT%02d:%02d:%f\n",fit_prms->time.yr, fit_prms->time.mo,
                          fit_prms->time.dy, fit_prms->time.hr,
                          fit_prms->time.mt, fit_prms->time.sc +
                          fit_prms->time.us/1.0e6);
  fprintf(fp,"BEAM %02d\n",fit_prms->bmnum);
  fprintf(fp, "lag %d %d %d %d %d %d\n",lag->lag_idx,lag->lag_num,lag->pulses[0],
    lag->pulses[1],lag->sample_base1,lag->sample_base2);
  fclose(fp);

}

void print_pwr_node(llist_node node,FILE* fp){
  PWRNODE* pwr;
  pwr = (PWRNODE*)(node);
  fprintf(fp,"ln_pwr %f sigma %f t %f\n",pwr->ln_pwr,pwr->sigma,pwr->t);

}

void print_phase_node(llist_node node, FILE* fp){
  PHASENODE* phi;
  phi = (PHASENODE*)(node);
  fprintf(fp,"phi %f sigma %f t %f\n",phi->phi,phi->sigma,phi->t);

}

void print_phase_node_to_file(llist_node node, FILE* fp){
  PHASENODE* phi;
  phi = (PHASENODE*)(node);
  fprintf(fp,"%f\n",phi->phi);
}

void print_uncorrected_phase(llist_node node, FITPRMS* fit_prms){
  FILE* fp;

  int list_null_flag = LLIST_SUCCESS;
  llist_node phase_node;

  fp = fopen("phases.txt","a");
  fprintf(fp,"TIME %d-%02d-%02dT%02d:%02d:%f\n",fit_prms->time.yr, fit_prms->time.mo,
                          fit_prms->time.dy, fit_prms->time.hr,
                          fit_prms->time.mt, fit_prms->time.sc +
                          fit_prms->time.us/1.0e6);
  fprintf(fp,"BEAM %02d\n",fit_prms->bmnum);
  fprintf(fp,"RANGE %d\n",((RANGENODE*)node)->range);
  
  llist_reset_iter(((RANGENODE*)node)->phases);
  llist_get_iter(((RANGENODE*)node)->phases, &phase_node);
  while(phase_node != NULL && list_null_flag == LLIST_SUCCESS)
  {
     print_phase_node_to_file(phase_node, fp);
     list_null_flag = llist_go_next(((RANGENODE*)node)->phases);
     llist_get_iter(((RANGENODE*)node)->phases, &phase_node); 
  }
  list_null_flag = LLIST_SUCCESS;
  llist_reset_iter(((RANGENODE*)node)->phases);

  fclose(fp);
}






/*++++++++++++++++++++++VARIOUS HELPER FUNCTIONS+++++++++++++++++++*/

/**
 * @brief      Calculates the alpha value at each lag.
 *
 * @param[in]  lag        A LAGNODE struct for a particular lag.
 * @param[in]  range      A RANGENODE struct holding range data.
 * @param      lag_0_pwr  The lag 0 power for a range.
 *
 * This function calculates the alpha value used in the Bendat and Piersol calculation for ACF
 * power and phase.
 */
void calculate_alpha_at_lags(llist_node lag, llist_node range, double* lag_0_pwr){
  double pulse_i_cri,pulse_j_cri;
  ALPHANODE* alpha_2;
  LAGNODE* lag_node;
  RANGENODE* range_node;
  double pwr;

  pwr=*lag_0_pwr;

  lag_node = (LAGNODE*)lag;
  range_node = (RANGENODE*)range;

  pulse_i_cri = range_node->CRI[lag_node->pulses[0]];
  pulse_j_cri = range_node->CRI[lag_node->pulses[1]];

  alpha_2 = malloc(sizeof(*alpha_2));
  alpha_2->lag_idx = lag_node->lag_idx;
  alpha_2->alpha_2 = (pwr * pwr)/((pwr + pulse_i_cri) * (pwr + pulse_j_cri));
  llist_add_node(range_node->alpha_2,(llist_node)alpha_2,0);

}

/**
 * @brief      Marks bad samples that are blanked by TX overlap.
 *
 * @param      fit_prms     The FITPRM struct of rawacf record info.
 * @param[in]  bad_samples  The list of bad samples.
 *
 * This function converts pulse numbers to microseconds and then loops to determine what sample
 * numbers would have been used for those pulses. Bad samples are then marked if they are TX
 * overlapped. Other than accounting for stereo modes, much of the core algorithm was adopted
 * straight from FITACF 2.5.
 */
void mark_bad_samples(FITPRMS *fit_prms, llist bad_samples){
  int j, sample;
  long ts, t1=0, t2=0;
  int *bad_sample, *pulse_us;
  int offset, channel;
  llist pulses_in_us, pulses_stereo;
  ts = (long) fit_prms->lagfr;
  offset = fit_prms->offset;
  channel = fit_prms->channel;
  sample = 0;

  t2 = 0L;

  pulses_in_us = llist_create(compare_ints,sample_node_eq,0);
  pulses_stereo = llist_create(compare_ints,sample_node_eq,0);

  for(j=0; j<fit_prms->mppul; j++){
    pulse_us = malloc(sizeof(*pulse_us));
    memset(pulse_us, 0, sizeof(*pulse_us));
    *pulse_us = fit_prms->pulse[j] * fit_prms->mpinc;
    if(llist_add_node(pulses_in_us,(llist_node)pulse_us,0) != LLIST_SUCCESS){
      fprintf(stderr,"pulses_in_us list node failed to add in mark_bad_samples\n");
    };
  }

  if((offset != 0) && ((channel == 1) || (channel == 2))) {

    for(j=0; j< fit_prms->mppul;j++){
      pulse_us = malloc(sizeof(*pulse_us));
      memset(pulse_us, 0, sizeof(*pulse_us));
      if (channel == 1) {
        *pulse_us = fit_prms->pulse[j] * fit_prms->mpinc - offset;
      }
      else {
        *pulse_us = fit_prms->pulse[j] * fit_prms->mpinc + offset;
      }
      if(llist_add_node(pulses_stereo,(llist_node)pulse_us,0) != LLIST_SUCCESS){
        fprintf(stderr,"pulses_stereo list node failed to add in mark_bad_samples\n");
      };
    }

    if(llist_concat(pulses_in_us,pulses_stereo) != LLIST_SUCCESS) {
      fprintf(stderr,"unable to concatenate lists in mark_bad_samples\n");
    }
    if(llist_sort(pulses_in_us,SORT_LIST_ASCENDING) != LLIST_SUCCESS) {
      fprintf(stderr,"unable to sort list in mark_bad_samples\n");
    }

  }

  llist_reset_iter(pulses_in_us);

  do{
    llist_get_iter(pulses_in_us,(void**)&pulse_us);
    t1 = *pulse_us - fit_prms->txpl/2;
    t2 = t1 + 3.0 * fit_prms->txpl/2 + 100;

    while (ts < t1) {
      sample++;
      ts = ts + fit_prms->smsep;
    }

    while ((ts >= t1) && (ts <= t2)) {
      bad_sample = malloc(sizeof(*bad_sample));
      memset(bad_sample, 0, sizeof(*bad_sample));
      *bad_sample = sample;
      if(llist_add_node(bad_samples,(llist_node)bad_sample,0) != LLIST_SUCCESS){
        fprintf(stderr,"list node failed to add in Mark_Bad_Samples\n");
      };
      sample++;
      ts = ts + fit_prms->smsep;
    }

  }while(llist_go_next(pulses_in_us) != LLIST_END_OF_LIST);

  llist_destroy(pulses_in_us,true,free);
  llist_destroy(pulses_stereo,true,free);


}

/**
 * @brief      Filters TX overlapped data within a range.
 *
 * @param[in]  range        The RANGENODE struct with data.
 * @param[in]  lags         The list of LAGNODE structs.
 * @param[in]  bad_samples  The list of bad samples.
 *
 * This function prunes ACF power and ACF/XCF phase nodes from the range at lags that are marked
 * by bad samples from TX overlap.
 */
void filter_tx_overlapped_lags(llist_node range, llist lags, llist bad_samples){
  RANGENODE* range_node;
  LAGNODE* lag;

  int smp1, smp2;

  range_node = (RANGENODE*) range;

  llist_reset_iter(lags);

  do{
    llist_get_iter(lags,(void**)&lag);
    smp1 = lag->sample_base1 + range_node->range;
    smp2 = lag->sample_base2 + range_node->range;

    if((llist_find_node(bad_samples,&smp1,NULL) == LLIST_SUCCESS) ||
       (llist_find_node(bad_samples,&smp2,NULL) == LLIST_SUCCESS)){

      llist_delete_node(range_node->pwrs,&lag->lag_idx,TRUE,free);
      llist_delete_node(range_node->phases,&lag->lag_idx,TRUE,free);
      llist_delete_node(range_node->elev,&lag->lag_idx,TRUE,free);
      llist_delete_node(range_node->alpha_2,&lag->lag_idx,TRUE,free);

    }

  }while(llist_go_next(lags) != LLIST_END_OF_LIST);

}


/**
 * @brief      Applies phase correction to phase points during unwrap.
 *
 * @param      phase                  A PHASENODE struct.
 * @param      slope_est              The estimate of the unwrapped phase slope.
 * @param      total_2pi_corrections  A holder for the total number of 2pi corrections.
 *
 * This function applies a possible number of 2pi corrections to a phase point in order to unwrap
 * the phase points for fitting.
 */
void phase_correction(PHASENODE* phase, double* slope_est, int* total_2pi_corrections ){
  PHASENODE* phase_node;
  double phi_pred;
  double phi_corr;
  double phi_diff;
  phase_node = (PHASENODE*) phase;

  phi_pred = *slope_est * phase_node->t;

  phi_diff = phi_pred - phase_node->phi;
  phi_diff = round(100000.0 * (phi_diff)/(2 * PI))/100000.0;
  phi_corr = round(phi_diff);

  phase_node->phi += phi_corr * 2 * PI;

  *total_2pi_corrections += abs(phi_corr);

}


/**
 * @brief      Calculates a correction factor in determining the noise level at a range.
 *
 * @param      fit_prms  The FITPRM struct holding rawacf record info.
 *
 * @return     The cutoff power correction.
 *
 * Using the 10 lowest values of lag 0 power underestimates the noise level. This function
 * calculates a correction to properly estimate the noise. The correction is based on the normalised Gaussian
 * model of the noise distribution with unit mean and standard deviation of 1/sqrt(Nave).
 */
double cutoff_power_correction(FITPRMS *fit_prms){
  double i=0;
  double corr=1;    /* Correction factor */
  double x;     /* Normalized power for calculating model PDF (Gaussian)*/
  double pdf;     /* PDF */
  double cpdf=0;    /* Cumulative PDF value*/
  double cpdfx=0;   /* Cumulative value of PDF*x -- needed for calculating the mean */
  double s;

  s=1./sqrt(fit_prms->nave); /* Model standard deviation */

  while(cpdf < 10./fit_prms->nrang){
    x=i/1000.;
    pdf=exp(-((x-1.)*(x-1.)/(2.*s*s)))/s/sqrt(2*PI)/1000.;  /* Normalized Gaussian distribution centered at 1 */
    cpdf=cpdf+pdf;
    cpdfx=cpdfx+pdf*x;
    i++;
  }

  corr=1./(cpdfx/cpdf);   /* Correcting factor as the inverse of a normalized mean */
  return corr;

}

/**
 * @brief      Determines the average power level for which an ACF is pure noise.
 *
 * @param      fit_prms  The FITPRM struct holding rawacf record info.
 *
 * @return     ACF Lag 0 noise.
 *
 * This function determines the minimum power level for which an ACF is pure
 * noise. This is used to filter bad ACFs. 
 * IMPORTANT: FITACF1-2 determined the noise level as the average of the 10 lowest
 * lag 0 power values. This method underestimated the noise level because these 
 * power values represent the low-power "tail" of the entire noise distribution. 
 * In FITACF3 we compenseate for this effect by using cutoff_power_correction function (above).
 */
double ACF_cutoff_pwr(FITPRMS *fit_prms){

    int i;
    double min_pwr = 0.0;
    double *pwr_levels;
    int ni = 0;
    double cpc = 0.0;

    pwr_levels = malloc(fit_prms->nrang * sizeof(*pwr_levels));
    memset(pwr_levels, 0, sizeof(*pwr_levels));
    if(pwr_levels == NULL){
        return -1.0;
    }

    /* Determine the lag_0 noise level (0 dB reference) and the noise level at
     which fit_acf is to quit (average power in the
     fluctuations of the acfs which are pure noise) */

    for (i=0; i < fit_prms->nrang; i++) {
        /* transfer powers into local array */
        pwr_levels[i] = fit_prms->pwr0[i];
    }

    qsort(pwr_levels, fit_prms->nrang, sizeof(double), cmp_dbl);
    /* determine the average lag0 power of the 10 lowest power acfs */


    i=0;


    /*  look for the lowest 10 values of lag0 power and average to
            get the noise level.  Ignore values that are exactly 0.  If
            you can't find 10 usable values within the first 1/3 of the
            sorted power list, then just use whatever you got in that
            first 1/3.  If you didn't get any usable values, then use
            the NOISE parameter */

    while ((ni < 10) && (i < fit_prms->nrang/3)) {
        if (pwr_levels[i]) ++ni;
        min_pwr += pwr_levels[i++];
    }

    ni = (ni > 0) ? ni :  1;
    cpc = cutoff_power_correction(fit_prms);
    min_pwr = min_pwr/ni * cpc;
    
    
    /*  The commented line below causes Inf values of the SNR when the search noise 
        is zero. Therefore, we introduce an additional condition that the search noise 
        must be nonzero. This is a temporary fix that allows the affected data to be 
        processed properly while not affecting the processing of any other data. 
        A more universal solution to this issue should be developed for the 
        next release. */
   
    /*if (min_pwr < 1.0) min_pwr = fit_prms->noise;*/
    if ((min_pwr < 1.0) && (fit_prms->noise != 0.0)) min_pwr = fit_prms->noise;
    
    free(pwr_levels);
    return min_pwr;

}





/*+++++++++++++++++++MAIN ACF PROCESSING FUNCTIONS+++++++++++++++++++*/

/**
 * @brief      Prunes ACFs entirely from analysis if they are deemed to be pure noise.
 *
 * @param      fit_prms   The FITPRM struct holding rawacf record info.
 * @param[in]  ranges     The list of RANGENODE structs.
 * @param[in]  noise_pwr  The noise power.
 *
 * Iterates over the ranges and prunes any range that has a lag 0 power lower than the noise power.
 */
void Filter_Bad_ACFs(FITPRMS *fit_prms, llist ranges, double noise_pwr){
  RANGENODE* range_node = NULL;
  PWRNODE* pwr_node = NULL;
  double tmp_pwr = 0.0;
  double cutoff_pwr = 0.0;
  llist bad_ranges_1,bad_ranges_2;
  int *bad_range = NULL;
  int bad_pwrs_flag = 1;

  /*Nothing to filter here*/
  if ((llist_size(ranges) == 0) || (fit_prms->nave <= 0)){
    return;
  }

  bad_ranges_1 = llist_create(NULL,NULL,0);
  bad_ranges_2 = llist_create(NULL,NULL,0);
  cutoff_pwr = noise_pwr * 2.;

  /*Removing low-power ACFs from the analysis whose lag 0 power lies below 0 dB (doubled mean)
   *threshold of the model noise distribution*/
  llist_reset_iter(ranges);

  do{
    llist_get_iter(ranges,(void**)&range_node);
    if((fit_prms->pwr0[range_node->range] <= cutoff_pwr) ||
       (llist_size(range_node->pwrs) < MIN_LAGS))
    {
      bad_range = malloc(sizeof(*bad_range));
      memset(bad_range, 0, sizeof(*bad_range));
      *bad_range = range_node->range;
      llist_add_node(bad_ranges_1,(llist_node)bad_range,0);
    }

  }while(llist_go_next(ranges) != LLIST_END_OF_LIST);

  llist_reset_iter(bad_ranges_1);
  do{
    llist_get_iter(bad_ranges_1,(void**)&bad_range);
    llist_delete_node(ranges,bad_range,TRUE,free_range_node);
  }while(llist_go_next(bad_ranges_1) != LLIST_END_OF_LIST);


  /*Removing ACFs with constant power levels*/
  if (llist_size(ranges) != 0){
    llist_reset_iter(ranges);
    do{
      llist_get_iter(ranges,(void**)&range_node);
      llist_reset_iter(range_node->pwrs);
      llist_get_iter(range_node->pwrs,(void**)&pwr_node);
      tmp_pwr = pwr_node->ln_pwr;
      llist_go_next(range_node->pwrs);
      do{
        llist_get_iter(range_node->pwrs,(void**)&pwr_node);
        if (pwr_node->ln_pwr != tmp_pwr) {
          bad_pwrs_flag = 0;
          break;
        }
      }while(llist_go_next(range_node->pwrs) != LLIST_END_OF_LIST);
      if(bad_pwrs_flag == 1){
        bad_range = malloc(sizeof(*bad_range));
        memset(bad_range, 0, sizeof(*bad_range));
        *bad_range = range_node->range;
        llist_add_node(bad_ranges_2,(llist_node)bad_range,0);
      }
      else {
        bad_pwrs_flag = 1;
      }
    }while(llist_go_next(ranges) != LLIST_END_OF_LIST);

    llist_reset_iter(bad_ranges_2);
    do{
      llist_get_iter(bad_ranges_2,(void**)&bad_range);
      llist_delete_node(ranges,(llist_node)bad_range,TRUE,free_range_node);
    }while(llist_go_next(bad_ranges_2) != LLIST_END_OF_LIST);
  }

  llist_destroy(bad_ranges_1,TRUE,free);
  llist_destroy(bad_ranges_2,TRUE,free);

}

/**
 * @brief      Filters ranges that have bad fits to raw data.
 *
 * @param[in]  ranges  The list of RANGENODE structs.
 *
 * It is possible for a bad fit to occur when the 'b' parameter of a fit is exactly zero since the
 * final determinations depend on divisions of these fitted values. These ranges are filtered out
 * since it is likely that the range has bad data(all constant values, etc)
 */
void Filter_Bad_Fits(llist ranges) {
  RANGENODE* range_node = NULL;
  llist bad_ranges;
  int *bad_range = NULL;

  if(llist_size(ranges) == 0){
    return;
  }

  bad_ranges = llist_create(NULL,NULL,0);

  llist_reset_iter(ranges);

  do{
    llist_get_iter(ranges,(void**)&range_node);
    if(range_node->phase_fit->b == 0.0 ||
       range_node->l_pwr_fit->b == 0.0 ||
       range_node->q_pwr_fit->b == 0.0)
    {
      bad_range = malloc(sizeof(*bad_range));
      memset(bad_range, 0, sizeof(*bad_range));
      *bad_range = range_node->range;
      llist_add_node(bad_ranges,(llist_node)bad_range,0);
    }

  }while(llist_go_next(ranges) != LLIST_END_OF_LIST);

  llist_reset_iter(bad_ranges);
  do{
    llist_get_iter(bad_ranges,(void**)&bad_range);
    llist_delete_node(ranges,(llist_node)bad_range,TRUE,free_range_node);
  }while(llist_go_next(bad_ranges) != LLIST_END_OF_LIST);

  llist_destroy(bad_ranges,TRUE,free);
}


/**
 * @brief      Create a list of RANGENODE structs from raw data.
 *
 * @param      fit_prms  The FITPRM struct holding rawacf record info.
 * @param[in]  ranges    The list of ranges to add nodes.
 */
void Fill_Range_List(FITPRMS *fit_prms, llist ranges){
  RANGENODE* temp;

  for(int i=0; i<fit_prms->nrang; i++){
      if(fit_prms->pwr0[i] != 0.0){
    temp = new_range_node(i,fit_prms);
    llist_add_node(ranges,(llist_node)temp,0);
      }
  }
}


/**
 * @brief      Prunes off low power lags from the list within a range.
 *
 * @param[in]  range     The RANGENODE struct to operate on.
 * @param      fit_prms  The FITPRM struct holding rawacf record info.
 *
 * This function prunes off low power lags determined by cutoff criteria. Once a cuttoff lag is
 * determined, all subsequent lags in the list are removed as they too will be below the fluctuation
 * level. 
 */
void Filter_Low_Pwr_Lags(llist_node range, FITPRMS* fit_prms){
  double log_sigma_fluc = 0;
  /*ALPHANODE* alpha_2 = NULL;*/
  PWRNODE* pwr_node = NULL;
  RANGENODE* range_node = NULL;
  int cutoff_lag = 0;
  llist bad_pwr_indices;
  int *bad_pwr_idx = NULL;

  range_node = (RANGENODE*) range;

  /*Nothing to filter here*/
  if ((llist_size(range_node->pwrs) == 0) || (fit_prms->nave <= 0)){
    return;
  }
  bad_pwr_indices = llist_create(NULL,NULL,0);
  cutoff_lag = fit_prms->mplgs + 1;



  /*Cutoff fluctuation level for filtering*/
  log_sigma_fluc =  log(FLUCTUATION_CUTOFF_COEFF * fit_prms->pwr0[range_node->range]/sqrt(2 * fit_prms->nave));



  llist_reset_iter(range_node->pwrs);
  /*llist_reset_iter(range_node->alpha_2);*/


  do{
    llist_get_iter(range_node->pwrs,(void**)&pwr_node);
    /*llist_get_iter(range_node->alpha_2,(void**)&alpha_2);*/

    if(pwr_node->lag_idx > cutoff_lag){
      /*llist_delete_node(range_node->pwrs,&pwr_node->lag_idx,TRUE,free);*/
      bad_pwr_idx = malloc(sizeof(*bad_pwr_idx));
      memset(bad_pwr_idx, 0, sizeof(*bad_pwr_idx));
      *bad_pwr_idx = pwr_node->lag_idx;
      llist_add_node(bad_pwr_indices,(llist_node)bad_pwr_idx,0);
    }
    else{
      /*Full cutoff criteria for lag filtering*/
      if((1/ sqrt(pwr_node->alpha_2) <= ALPHA_CUTOFF) && (pwr_node->ln_pwr <= log_sigma_fluc)){
        cutoff_lag = pwr_node->lag_idx;
        /*llist_delete_node(range_node->pwrs,&pwr_node->lag_idx,TRUE,free);*/
        bad_pwr_idx = malloc(sizeof(*bad_pwr_idx));
        memset(bad_pwr_idx, 0, sizeof(*bad_pwr_idx));
        *bad_pwr_idx = pwr_node->lag_idx;
        llist_add_node(bad_pwr_indices,(llist_node)bad_pwr_idx,0);
      }
    }

    /*llist_go_next(range_node->alpha_2);*/
  }while(llist_go_next(range_node->pwrs) != LLIST_END_OF_LIST);

  if (llist_size(bad_pwr_indices) != 0) {
    llist_reset_iter(bad_pwr_indices);
    do{
      llist_get_iter(bad_pwr_indices,(void**)&bad_pwr_idx);
      llist_delete_node(range_node->pwrs,(llist_node)bad_pwr_idx,TRUE,free);
    }while(llist_go_next(bad_pwr_indices) != LLIST_END_OF_LIST);
  }

  llist_destroy(bad_pwr_indices,TRUE,free);


}

/**
 * @brief      Calculates the cross-range interference for each range.
 *
 * @param[in]  range     A RANGENODE struct.
 * @param      fit_prms  The FITPRM struct holding rawacf record info.
 *
 * For a given range, this function finds the cross-range interference of each pulse in the
 * pulse sequence.
 */
void Find_CRI(llist_node range,FITPRMS *fit_prms){
  int pulse_to_check;
  int pulse;
  int range_to_check;
  double total_cri;
  int tau;
  int diff_pulse;

  RANGENODE* range_node = (RANGENODE*)range;

  if (fit_prms->smsep != 0) {
    tau = fit_prms->mpinc / fit_prms->smsep;
  } else {
    fprintf( stderr, "r_overlap: WARNING, using txpl instead of smsep...\n");
    tau = fit_prms->mpinc / fit_prms->txpl;
  }

  for (pulse_to_check = 0;  pulse_to_check < fit_prms->mppul; ++pulse_to_check) {
    total_cri = 0.0;

    for (pulse = 0; pulse < fit_prms->mppul; ++pulse) {
          diff_pulse = fit_prms->pulse[pulse_to_check] - fit_prms->pulse[pulse];
      range_to_check = diff_pulse * tau + range_node->range;

      if ((pulse != pulse_to_check) &&
        (0 <= range_to_check) &&
        (range_to_check < fit_prms->nrang)) {
          total_cri += fit_prms->pwr0[range_to_check];
      }
    }

    range_node->CRI[pulse_to_check] = total_cri;
  }

}


/**
 * @brief      Calculates alpha values at a range.
 *
 * @param[in]  range     A RANGENODE struct.
 * @param[in]  lags      The list of LAGNODE structs.
 * @param      fit_prms  The FITPRM struct holding rawacf record info.
 *
 * For a given range this function creates a new list of alpha values at each lag for that range.
 */
void Find_Alpha(llist_node range,llist lags, FITPRMS *fit_prms){
  llist alpha_2;
  RANGENODE* range_node;
  double lag_0_pwr;
  llist_node node; 
  int list_null_flag = LLIST_SUCCESS;

  range_node = (RANGENODE*)range;

  alpha_2 = llist_create(compare_doubles,alpha_node_eq,0);

  range_node->alpha_2 = alpha_2;

  lag_0_pwr = fit_prms->pwr0[range_node->range];

  llist_reset_iter(lags);
  llist_get_iter(lags, &node);
  while(node != NULL && list_null_flag == LLIST_SUCCESS)
  {
     calculate_alpha_at_lags(node, range_node, &lag_0_pwr);
     list_null_flag = llist_go_next(lags);
     llist_get_iter(lags, &node); 
  }
  list_null_flag = LLIST_SUCCESS;
  llist_reset_iter(lags);

}


/**
 * @brief      A two iteration process to unwrap the ACF phase.
 *
 * @param[in]  range     A RANGENODE struct.
 * @param      fit_prms  The FITPRM struct holding rawacf record info.
 *
 * For a given range, this function performs a 2 iteration process to unwrap the phase for
 * fitting. The first iteration comes up with an estimate of the slope to determine whether an
 * unwrap is necessary. The second step does a quick fit to the phase and determines whether
 * the unwrapped or wrapped phase has a better error. The method with a better error is the phase
 * used for true fitting.
 */
void ACF_Phase_Unwrap(llist_node range, FITPRMS* fit_prms){
  RANGENODE* range_node;
  PHASENODE* phase_curr;
/*PHASENODE* phase_prev;*/
  PHASENODE* local_copy;

  int i=0;
  double d_phi,sigma_bar,d_tau;
  double corr_slope_est = 0.0 ,slope_denom = 0.0,slope_num = 0.0, corr_slope_err = 0.0;
  double orig_slope_est = 0.0, orig_slope_err= 0.0;
  double piecewise_slope_est = 0.0;
  int *total_2pi_corrections;
  double S_xy = 0.0, S_xx = 0.0;
  int num_local_phases=0;

  range_node = (RANGENODE*) range;

  total_2pi_corrections = malloc(sizeof(*total_2pi_corrections));
  memset(total_2pi_corrections, 0, sizeof(*total_2pi_corrections));
  *total_2pi_corrections = 0;
  /*Because our list of phases has been filtered, we can use a local copy in an array
   for simple sequential access. The list is used in the first place for easy removal
   without the need of array masking. This local copy will be used to test if unwrap is
   needed in the first place*/
  local_copy = malloc(sizeof(PHASENODE) * llist_size(range_node->phases));
  memset(local_copy, 0, sizeof(*local_copy));

  /*Copy phase into local copy*/
  llist_reset_iter(range_node->phases);
  do{
    llist_get_iter(range_node->phases, (void**)&phase_curr);
        local_copy[num_local_phases++] = *phase_curr;
  }while(llist_go_next(range_node->phases) != LLIST_END_OF_LIST);


  for (int i=0; i<num_local_phases - 1; i++) {
    d_phi = local_copy[i+1].phi - local_copy[i].phi;
    sigma_bar = (local_copy[i+1].sigma + local_copy[i].sigma)/2;
    d_tau = local_copy[i+1].t - local_copy[i].t;

    if(fabs(d_phi) < PI){
      slope_num += d_phi/(sigma_bar * sigma_bar)/d_tau;
      slope_denom += 1/(sigma_bar * sigma_bar);
    }

  }

  piecewise_slope_est = slope_num / slope_denom;


  for (int i=0; i<num_local_phases; i++) {
    phase_correction(&local_copy[i], &piecewise_slope_est, total_2pi_corrections);
  }

  /*We determine whether wrapped or unwrapped phase has better error on fit. Select
   the lower of two.*/
  if (*total_2pi_corrections > 0){

    /*Quickly fit unwrapped phase for slope and err. Needed to compare error*/
    S_xx = 0.0;
    S_xy = 0.0;
    for (int i=0; i<num_local_phases; i++){
      if (local_copy[i].sigma >0){
        S_xy += (local_copy[i].phi * local_copy[i].t) / (local_copy[i].sigma * local_copy[i].sigma);
        S_xx += (local_copy[i].t * local_copy[i].t) / (local_copy[i].sigma * local_copy[i].sigma);
      }
    }

    corr_slope_est = S_xy / S_xx;
    for (int i=0; i<num_local_phases; i++){
      if (local_copy[i].sigma > 0) {
        corr_slope_err += ((corr_slope_est * local_copy[i].t - local_copy[i].phi) *
                           (corr_slope_est * local_copy[i].t - local_copy[i].phi)) /
                           (local_copy[i].sigma * local_copy[i].sigma);
      }
    }

    i=0;
    /*Quick fit of original phase*/
    llist_reset_iter(range_node->phases);
    S_xx = 0.0;
    S_xy = 0.0;
    do{
      llist_get_iter(range_node->phases, (void**)&phase_curr);
      if(phase_curr->sigma > 0){
        S_xy += (phase_curr->phi * phase_curr->t)/(phase_curr->sigma * phase_curr->sigma);
        S_xx += (phase_curr->t * phase_curr->t)/(phase_curr->sigma * phase_curr->sigma);
      }
    }while(llist_go_next(range_node->phases) != LLIST_END_OF_LIST);

    orig_slope_est = S_xy / S_xx;

    llist_reset_iter(range_node->phases);
    do{
      llist_get_iter(range_node->phases, (void**)&phase_curr);
      if(phase_curr->sigma > 0){
        orig_slope_err += ((orig_slope_est * phase_curr->t - phase_curr->phi) *
                (orig_slope_est * phase_curr->t - phase_curr->phi)) /
                (phase_curr->sigma * phase_curr->sigma);
      }
    }while(llist_go_next(range_node->phases) != LLIST_END_OF_LIST);

    /*If the error on the original phase is worse, copy over local copy*/
    if (orig_slope_err > corr_slope_err) {
      i=0;
      llist_reset_iter(range_node->phases);
      do{
        llist_get_iter(range_node->phases, (void**)&phase_curr);
        *phase_curr = local_copy[i++];
      }while(llist_go_next(range_node->phases) != LLIST_END_OF_LIST);
    }

  }

  free(total_2pi_corrections);
  free(local_copy);

}

/**
 * @brief      A one iteration process to unwrap XCF phase.
 *
 * @param[in]  range  A RANGENODE struct.
 *
 * This function unwraps the phase for XCF. Only one iteration is needed to improve the unwrap
 * because the fitted slope of the ACF phase is used as the initial phase unwrapping guess.
 */
void XCF_Phase_Unwrap(llist_node range){
  
  llist_node node; 
  int list_null_flag = LLIST_SUCCESS;

  RANGENODE* range_node;
  PHASENODE* phase_curr;
  double S_xy, S_xx,slope_est;
  int *total_2pi_corrections = NULL;
  range_node = (RANGENODE*) range;

  total_2pi_corrections = malloc(sizeof(*total_2pi_corrections));
  memset(total_2pi_corrections, 0, sizeof(*total_2pi_corrections));
  *total_2pi_corrections = 0;
      
  llist_reset_iter(range_node->elev);
  llist_get_iter(range_node->elev, &node);
  while(node != NULL && list_null_flag == LLIST_SUCCESS)
  {
      phase_correction(node, &range_node->phase_fit->b, total_2pi_corrections);
      list_null_flag = llist_go_next(range_node->elev);
      llist_get_iter(range_node->elev, &node); 
  }
  list_null_flag = LLIST_SUCCESS;
  llist_reset_iter(range_node->elev);

  llist_reset_iter(range_node->elev);
  S_xx = 0;
  S_xy = 0;

  /*one iteration to improve slope estimate by quickly fitting to first round unwrap*/
  do{

    llist_get_iter(range_node->elev, (void**)&phase_curr);

    if(phase_curr->sigma > 0.0){
      S_xy += (phase_curr->phi * phase_curr->t)/(phase_curr->sigma * phase_curr->sigma);

      S_xx += (phase_curr->t * phase_curr->t)/(phase_curr->sigma * phase_curr->sigma);
    }

  }while(llist_go_next(range_node->elev) != LLIST_END_OF_LIST);


  slope_est = S_xy / S_xx;

  llist_reset_iter(range_node->elev);
  llist_get_iter(range_node->elev, &node);
  while(node != NULL && list_null_flag == LLIST_SUCCESS)
  {
      phase_correction(node, &slope_est, total_2pi_corrections);
      list_null_flag = llist_go_next(range_node->elev);
      llist_get_iter(range_node->elev, &node); 
  }
  list_null_flag = LLIST_SUCCESS;
  llist_reset_iter(range_node->elev);

  free(total_2pi_corrections);
}


/**
 * @brief      Filters data points at lags affected by TX overlap.
 *
 * @param[in]  ranges    A RANGENODE struct.
 * @param[in]  lags      The list of LAGNODE structs.
 * @param      fit_prms  The FITPRM struct holding rawacf record info.
 *
 * This function prunes phase and power lags from a range list if they have been affected by TX
 * overlap.
 */
void Filter_TX_Overlap(llist ranges, llist lags, FITPRMS *fit_prms){
    llist bad_samples = NULL;
    llist_node node; 
    int list_null_flag = LLIST_SUCCESS;

    bad_samples = llist_create(NULL,sample_node_eq,0);

    mark_bad_samples(fit_prms,bad_samples);

    llist_reset_iter(ranges);
    llist_get_iter(ranges, &node);
    while(node != NULL && list_null_flag == LLIST_SUCCESS)
    {
        filter_tx_overlapped_lags(node, lags, bad_samples);
        list_null_flag = llist_go_next(ranges);
        llist_get_iter(ranges, &node); 
    }
    list_null_flag = LLIST_SUCCESS;
    llist_reset_iter(ranges);

    llist_destroy(bad_samples,TRUE,free);

}


/**
 * @brief      Creates a list of LAGNODE structs from the lag table.
 *
 * @param[in]  lags      The list of LAGNODE structs.
 * @param      fit_prms  The FITPRM struct holding rawacf record info.
 *
 * This function creates a list of LAGNODE structs using the lag table in the raw record. A lag node
 * contains information about what pulses are used to create the lag as well as the base samples
 * that are used for those pulses.
 */
void Determine_Lags(llist lags,FITPRMS *fit_prms){
  int i, j;
  LAGNODE* temp;

  /***Find lags from the lag table****/
  for(i=0;i<fit_prms->mplgs;i++){
    temp = malloc(sizeof(*temp));
    memset(temp, 0, sizeof(*temp));
    temp->lag_num = fit_prms->lag[1][i] - fit_prms->lag[0][i];
    for(j=0;j<fit_prms->mppul;j++){
      if(fit_prms->pulse[j] == fit_prms->lag[1][i]){
        temp->pulses[1] = j;
      }

      if(fit_prms->pulse[j] == fit_prms->lag[0][i]){
        temp->pulses[0] = j;
      }
    }
    temp->sample_base1 = fit_prms->lag[0][i] * (fit_prms->mpinc/fit_prms->smsep);
    temp->sample_base2 = fit_prms->lag[1][i] * (fit_prms->mpinc/fit_prms->smsep);
    llist_add_node(lags,(llist_node)temp,0);
  }

  llist_reset_iter(lags);
  i=0;
  do{
    llist_get_iter(lags,(void**)&temp);
    temp->lag_idx = i++;
  }while(llist_go_next(lags) != LLIST_END_OF_LIST);

}


/**
 * @brief      Adds a range's data points to its RANGENODE.
 *
 * @param[in]  range     A RANGENODE struct.
 * @param[in]  lags      The list of LAGNODE structs.
 * @param      fit_prms  The FITPRM struct holding rawacf record info.
 *
 * Each RANGENODE contains a list of ACF phases, XCF phases, and ACF power values for each lag. This
 * function takes the range data from a raw record and adds it to the lists for a RANGENODE.
 */
void Fill_Data_Lists_For_Range(llist_node range,llist lags,FITPRMS *fit_prms){
  RANGENODE* range_node;
  PWRNODE* pwr_node;
  PHASENODE* phase_node;
  PHASENODE* elev_node;
  LAGNODE* lag;
  ALPHANODE* alpha_2;

  range_node = (RANGENODE*) range;

  range_node->pwrs = llist_create(NULL,pwr_node_eq,0);;
  range_node->phases = llist_create(NULL,phase_node_eq,0);
  range_node->elev = llist_create(NULL,phase_node_eq,0);

  llist_reset_iter(range_node->alpha_2);
  llist_reset_iter(lags);

  for(int i=0;i<fit_prms->mplgs;i++){

    llist_get_iter(range_node->alpha_2,(void**)&alpha_2);
    llist_get_iter(lags,(void**)&lag);

    pwr_node = new_pwr_node(range_node->range,alpha_2->alpha_2,lag,fit_prms);
    phase_node = new_phase_node(range_node->range,alpha_2->alpha_2,lag,fit_prms);
    elev_node = new_elev_node(range_node->range,alpha_2->alpha_2,lag,fit_prms);
    if(pwr_node != NULL){ /* A NULL ptr means the pwr level was too low*/
      llist_add_node(range_node->pwrs,(llist_node)pwr_node,0);
    }
    llist_add_node(range_node->phases,(llist_node)phase_node,0);
    llist_add_node(range_node->elev,(llist_node)elev_node,0);

    llist_go_next(lags);
    llist_go_next(range_node->alpha_2);

  }


}

