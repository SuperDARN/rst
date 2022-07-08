/*
ACF Processing main functions

//TODO Add copyright notice

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


*/

#include "lmfit_preprocessing.h"
#include "selfclutter.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

/********************LIST NODE STUFF*********************/
/**
Initializes a new range node and returns a pointer to it.
*/

RANGENODE* new_range_node(int range, FITPRMS *fit_prms){
	RANGENODE* new_node;
	new_node = malloc(sizeof(RANGENODE));
	new_node->range = range;
	new_node->SC_pow = calloc(fit_prms->mppul,sizeof(*new_node->SC_pow));
	new_node->refrc_idx = 1;
	new_node->acf = NULL;
	new_node->xcf = NULL;
	new_node->phases = NULL;
	new_node->pwrs = NULL;
	new_node->elev = NULL;
	new_node->scpwr = NULL;
	new_node->l_acf_fit = new_lmfit_data();
 	new_node->q_acf_fit = new_lmfit_data();
 	new_node->l_xcf_fit = new_lmfit_data();
 	new_node->q_xcf_fit = new_lmfit_data();
	new_node->prev_pow = 0;
	new_node->prev_phase = 0;
	new_node->prev_width = 0;
	return new_node;
}


/**
Initializes a new pwr node and returns a pointer to it. Returns a null 
pointer if the power at this node is below the fluctuation level.
*/
PWRNODE* new_pwr_node(int range, LAGNODE* lag, FITPRMS *fit_prms){
	PWRNODE* new_pwr_node;
	double P,real,imag;

	real = fit_prms->acfd[range * fit_prms->mplgs + lag->lag_num][0];
	imag = fit_prms->acfd[range * fit_prms->mplgs + lag->lag_num][1];
	P = sqrt(real * real + imag * imag);

    /* Check to make sure lag0 power is not negative or zero */
	if(P <= 0.0) return NULL;
	
	new_pwr_node = malloc(sizeof(*new_pwr_node));

	new_pwr_node->lag_num = lag->lag_num;
	new_pwr_node->pwr = P;
	/* Error in estimation of lag0 power (P) is P/sqrt(nave) */
	/* But P = S + N + C, S = actual lag0 power, N = noise power, C = clutter power */
	/* But we are fitting to log(P) so error bar needs to be propagated through log() */
	/* so we'll use relative error instead: P/sqrt(nave) * (log(P)/P) */
	new_pwr_node->sigma = P / sqrt(fit_prms->nave);
	new_pwr_node->t = lag->pulse_diff * fit_prms->mpinc * 1.0e-6; /* Time for each lag */

	return new_pwr_node;
}


ACFNODE* new_acf_node(int range, LAGNODE* lag, FITPRMS *fit_prms){
	ACFNODE* new_acf_node;
	double P,real,imag;

	real = fit_prms->acfd[range * fit_prms->mplgs + lag->lag_num][0];
	imag = fit_prms->acfd[range * fit_prms->mplgs + lag->lag_num][1];
	P = sqrt(real * real + imag * imag);

    /* Check to make sure lag0 power is not negative or zero */
	if(P <= 0.0) return NULL;
	

	new_acf_node = malloc(sizeof(*new_acf_node));

	new_acf_node->lag_num = lag->lag_num;
	new_acf_node->re = real;
	new_acf_node->im = imag;
	/* First order error in estimation of acf lag estimate is (P+N+C)/sqrt(nave) */
	/* But only have P here so for now, just P/sqrt(nave) */
	new_acf_node->sigma_re = P / sqrt(fit_prms->nave);
	new_acf_node->sigma_im = P / sqrt(fit_prms->nave);
	new_acf_node->t = lag->pulse_diff * fit_prms->mpinc * 1.0e-6; /* Time for each lag */

	return new_acf_node;
}


ACFNODE* new_xcf_node(int range, LAGNODE* lag, FITPRMS *fit_prms){
	ACFNODE* new_xcf_node;
	double P,real,imag;

	real = fit_prms->xcfd[range * fit_prms->mplgs + lag->lag_num][0];
	imag = fit_prms->xcfd[range * fit_prms->mplgs + lag->lag_num][1];
	P = sqrt(real * real + imag * imag);

    /* Check to make sure lag0 power is not negative or zero */
	if(P <= 0.0) return NULL;
	

	new_xcf_node = malloc(sizeof(*new_xcf_node));

	new_xcf_node->lag_num = lag->lag_num;
	new_xcf_node->re = real;
	new_xcf_node->im = imag;
	/* First order error in estimation of xcf lag estimate is (P+N+C)/sqrt(nave) */
	/* But only have P here so for now, just P/sqrt(nave) */
	new_xcf_node->sigma_re = P / sqrt(fit_prms->nave);
	new_xcf_node->sigma_im = P / sqrt(fit_prms->nave);
	new_xcf_node->t = lag->pulse_diff * fit_prms->mpinc * 1.0e-6; /* Time for each lag */

	return new_xcf_node;
}


/**
Initializes a new pwr node and returns a pointer to it. Returns a null 
pointer if the power at this node is below the fluctuation level.
*/
SCNODE* new_sc_node(int range, LAGNODE* lag, FITPRMS *fit_prms){
	SCNODE* new_sc_node;

    /* Just initialize things to zero, we'll fill in actual values with another function */
	new_sc_node = malloc(sizeof(*new_sc_node));
	new_sc_node->lag_num = lag->lag_num;
	new_sc_node->clutter = 0.0;
	new_sc_node->sigma = 0;
	new_sc_node->t = lag->pulse_diff * fit_prms->mpinc * 1.0e-6; /* Time for each lag */

	return new_sc_node;
}


/**
Initializes a new elevation phase node and returns a pointer to it.
*/
PHASENODE* new_elev_node(int range, LAGNODE* lag, FITPRMS *fit_prms){
	PHASENODE* new_elev_node;
	double real,imag;

	new_elev_node = malloc(sizeof(*new_elev_node));

	real = fit_prms->xcfd[range * fit_prms->mplgs + lag->lag_num][0];
	imag = fit_prms->xcfd[range * fit_prms->mplgs + lag->lag_num][1];

	new_elev_node->lag_num = lag->lag_num;
	new_elev_node->phi = atan2(imag,real);
		
	/* This is merely a placeholder for the error. This gets changed later 
	   once we've fit the power */
	new_elev_node->sigma = 1.0;

	new_elev_node->t = lag->pulse_diff * fit_prms->mpinc * 1.0e-6;

	return new_elev_node;
}


/**
Frees all memory for range data
*/
void free_range_node(llist_node range){
	RANGENODE* range_node;

	range_node = (RANGENODE*) range;

	if(range_node->SC_pow !=NULL){
		free(range_node->SC_pow);
	}

	if(range_node->acf != NULL){
		llist_destroy(range_node->acf,TRUE,free);
	}

	if(range_node->phases != NULL){
		llist_destroy(range_node->phases,TRUE,free);
	}

	if(range_node->pwrs != NULL){
		llist_destroy(range_node->pwrs,TRUE,free);
	}

	if(range_node->scpwr != NULL){
		llist_destroy(range_node->scpwr,TRUE,free);
	}

	if(range_node->elev != NULL){
		llist_destroy(range_node->elev,TRUE,free);
	}

	if(range_node->l_acf_fit != NULL){
		free_lmfit_data(range_node->l_acf_fit);
	}

	if(range_node->q_acf_fit != NULL){
		free_lmfit_data(range_node->q_acf_fit);
	}

	if(range_node->l_xcf_fit != NULL){
		free_lmfit_data(range_node->l_xcf_fit);
	}

	if(range_node->q_xcf_fit != NULL){
		free_lmfit_data(range_node->q_xcf_fit);
	}

	if(range_node != NULL){
		free(range_node);
	}
}


/*++++++++++++++++++++CALLBACKS FOR COMPARISONS+++++++++++++++++++++*/
/**
Callback for comparing two doubles in qsort
*/
int dbl_cmp(const void *x,const void *y) {
  double *a,*b;
  a=(double *) x;
  b=(double *) y;
  if (*a > *b) return 1;
  else if (*a == *b) return 0;
  else return -1;
}


/**
Callback used to compare two lags 
*/
int compare_lags(llist_node first,llist_node second){
	LAGNODE* lag1;
	LAGNODE* lag2;
	lag1 = (LAGNODE*)(first);
	lag2 = (LAGNODE*)(second);

	if(lag1->pulse_diff < lag2->pulse_diff){
		return -1;
	}
	else if(lag1->pulse_diff == lag2->pulse_diff){
		return 0;
	}
	else{
		return 1;
	}
}


/**
Callback used to identify a pwr node by its lag number
*/
bool pwr_node_eq(llist_node node,llist_node cmp){
	PWRNODE* pwr_node;
	int value;

	pwr_node = (PWRNODE*) node;
	value = *(int*)(cmp);

	return (pwr_node->lag_num == value);
}


bool acf_node_eq(llist_node node,llist_node cmp){
	ACFNODE* acf_node;
	int value;

	acf_node = (ACFNODE*) node;
	value = *(int*)(cmp);

	return (acf_node->lag_num == value);
}


bool sc_node_eq(llist_node node,llist_node cmp){
	SCNODE* sc_node;
	int value;

	sc_node = (SCNODE*) node;
	value = *(int*)(cmp);

	return (sc_node->lag_num == value);
}


/**
Callback used to identify a phase node by its lag number
*/
bool phase_node_eq(llist_node node,llist_node cmp){
	PHASENODE* phase_node;
	int value;

	phase_node = (PHASENODE*) node;
	value = *(int*)(cmp);

	return (phase_node->lag_num == value);
}


/**
Callback used to compares doubles within a list
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
Callback used to compare samples
*/
bool sample_node_eq(llist_node node,llist_node cmp){
	int smp;
	int value;

	smp = *(int*)(node);
	value = *(int*)(cmp);

	return (smp == value);
}


/**
Callback used to compare ranges
*/
bool range_node_eq(llist_node node,llist_node cmp){
	RANGENODE* range_node;
	int value;

	range_node = (RANGENODE*) node;
	value = *(int*)(cmp);

	return (range_node->range == value);
}


/*++++++++++++++++++++PRINT CALLBACKS FOR DIFFERENT NODES+++++++++++++++++++*/
void print_node(llist_node node){
	printf("sample %d\n",*(int*)(node));
}


void print_lag_node(llist_node node){
	LAGNODE* lag;
	lag = (LAGNODE*)(node);
	printf("lag %d %d %d %d\n",lag->lag_num,lag->pulse_diff,lag->pulses[0],lag->pulses[1]);
}


void print_pwr_node(llist_node node,FILE* fp){
	PWRNODE* pwr;
	pwr = (PWRNODE*)(node);
	fprintf(fp,"pwr %f sigma %f t %f\n",pwr->pwr,pwr->sigma,pwr->t);
}


void print_acf_node(llist_node node,FILE* fp){
	ACFNODE* acf;
	acf = (ACFNODE*)(node);
	fprintf(fp,"re %f im %f sigma_re %f sigma_im %f t %f\n",acf->re,acf->im,acf->sigma_re,acf->sigma_im,acf->t);
}


void print_scpwr_node(llist_node node,FILE* fp){
	SCNODE* scpwr;
	scpwr = (SCNODE*)(node);
	fprintf(fp,"scpwr %f sigma %f t %f\n",scpwr->clutter,scpwr->sigma,scpwr->t);
}


void print_range_node(llist_node node){
	int i;
	FILE* fp;

	fp = fopen("fullrangeinfo.txt","a");
	fprintf(fp,"range %d\n",((RANGENODE*)node)->range);
	fprintf(fp,"prev_pow %f\n",((RANGENODE*)node)->prev_pow);
	fprintf(fp,"prev_phase %f\n",((RANGENODE*)node)->prev_phase);
	fprintf(fp,"prev_width %f\n",((RANGENODE*)node)->prev_width);

	fprintf(fp,"SC_pow ");
	for(i=0;i<8;i++){
		fprintf(fp,"%f ",((RANGENODE*)node)->SC_pow[i]);
	}
	fprintf(fp,"\nACF\n");

	llist_for_each_arg(((RANGENODE*)node)->acf, (node_func_arg)print_acf_node,fp,NULL);
	fprintf(fp,"\n");
	llist_for_each_arg(((RANGENODE*)node)->pwrs, (node_func_arg)print_pwr_node,fp,NULL);
	fprintf(fp,"SCPWR\n");
	llist_for_each_arg(((RANGENODE*)node)->scpwr, (node_func_arg)print_scpwr_node,fp,NULL);
	fprintf(fp,"LINEAR ACF FIT\n");
	print_lmfit_data(((RANGENODE*)node)->l_acf_fit,fp);
	fprintf(fp,"QUADRATIC ACF FIT\n");
	print_lmfit_data(((RANGENODE*)node)->q_acf_fit,fp);

	fclose(fp);
}


/*++++++++++++++++++++++VARIOUS HELPER FUNCTIONS+++++++++++++++++++*/
/**
This function finds the bad samples that are blacked out by TX overlap
*/
void mark_bad_samples(FITPRMS *fit_prms, llist bad_samples){
	int i, sample;
	long ts, t1=0, t2=0;
	int* bad_sample;
	i = -1;
	ts = (long) fit_prms->lagfr;
	sample = 0;

	t2 = 0L;

	while (i < (fit_prms->mppul - 1)) {
	/* first, skip over any pulses that occur before the first sample
	   defines transmitter pulse blanket window*/

		while ((ts > t2) && (i < (fit_prms->mppul - 1))) {
			i++;
			t1 = (long) (fit_prms->pulse[i]) * (long) (fit_prms->mpinc)
			- fit_prms->txpl/2;
	  		t2 = t1 + 3*fit_prms->txpl/2 + 100; /* adjust for rx-on delay */
		}	

	/*	we now have a pulse that occurs after the current sample.  Start
		incrementing the sample number until we find a sample that lies
		within the pulse */

		while (ts < t1) {
			sample++;
			ts = ts + fit_prms->smsep;
		}

	/*	ok, we now have a sample which occurs after the pulse starts.
		check to see if it occurs before the pulse ends, and if so, mark
		it as a bad sample */
		while ((ts >= t1) && (ts <= t2)) {
			bad_sample = malloc(sizeof(int));
			*bad_sample = sample;
			if(llist_add_node(bad_samples,(llist_node)bad_sample,0) != LLIST_SUCCESS){
				fprintf(stderr,"list node failed to add in Mark_Bad_Samples\n");
			};
			sample++;
			ts = ts + fit_prms->smsep;
		}
	}
}


/**
Helper callback function to remove phase and pwr lags that are marked by
bad samples due to TX overlap.
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

			llist_delete_node(range_node->pwrs,&lag->lag_num,TRUE,free);
		    llist_delete_node(range_node->acf,&lag->lag_num,TRUE,free);
		    /* add logic to delete xcf node */
			llist_delete_node(range_node->scpwr,&lag->lag_num,TRUE,free);
			llist_delete_node(range_node->phases,&lag->lag_num,TRUE,free);
			llist_delete_node(range_node->elev,&lag->lag_num,TRUE,free);
		}
	}while(llist_go_next(lags) != LLIST_END_OF_LIST);
}


/*+++++++++++++++++++MAIN ACF PROCESSING FUNCTIONS+++++++++++++++++++*/
/**
Fills the list of ranges with new range nodes.
*/
void Fill_Range_List(FITPRMS *fit_prms, llist ranges){
	int i;
	RANGENODE* temp;

	for(i=0; i<fit_prms->nrang; i++){
	    if(fit_prms->pwr0[i] != 0.0){
		temp = new_range_node(i,fit_prms);
		llist_add_node(ranges,(llist_node)temp,0);
	    }
	}
}


/**
This function filters out bad phase and pwr lags due to TX overlap and
filters out pwr lags that are below a fluctuation level
*/
void Filter_TX_Overlap(llist ranges, llist lags, FITPRMS *fit_prms){
	llist bad_samples = NULL;

	bad_samples = llist_create(NULL,sample_node_eq,0);

	mark_bad_samples(fit_prms,bad_samples);

	llist_for_each_arg(ranges, (node_func_arg)filter_tx_overlapped_lags, lags, bad_samples);

	llist_destroy(bad_samples,TRUE,free);
}


/**
The commented out code can take a given pulse sequence, and create a sorted list of lags
that can be formed from combining pulses. This algorithm doesnt need anything more
than the pulse sequence and can be used for custom pulse sequences or if no lag table
exists. The uncommented code determines lags from the lag table supplied in the raw
data.
*/
void Determine_Lags(llist lags,FITPRMS *fit_prms){
	int i, j;
	LAGNODE* temp;

	/****Find lags from pulse sequence****/
/*	for(i=fit_prms->mppul-1;i>=0;i--){
		for(j=i-1;j>=0;j--){
			if((fit_prms->pulse[i] - fit_prms->pulse[j]) <= fit_prms->mplgs){
				temp = malloc(sizeof(LAGNODE));
				temp->pulse_diff = fit_prms->pulse[i] - fit_prms->pulse[j];
				temp->pulses[0] = j;
				temp->pulses[1] = i;
				temp->sample_base1 = fit_prms->pulse[j] * (fit_prms->mpinc/fit_prms->smsep);
				temp->sample_base2 = fit_prms->pulse[i] * (fit_prms->mpinc/fit_prms->smsep);
				llist_add_node(lags,(llist_node)temp,0);

		    }

		}
	}

	llist_sort(lags,SORT_LIST_ASCENDING);

	temp = malloc(sizeof(LAGNODE));
	temp->pulse_diff = 0;
	temp->pulses[0] = fit_prms->pulse[0];
	temp->pulses[1] = fit_prms->pulse[0];
	temp->sample_base1 = fit_prms->pulse[0] * (fit_prms->mpinc/fit_prms->smsep);
	temp->sample_base2 = fit_prms->pulse[0] * (fit_prms->mpinc/fit_prms->smsep);
	llist_add_node(lags,(llist_node)temp,ADD_NODE_FRONT);

	llist_reset_iter(lags);
	i=0;
	do{
		llist_get_iter(lags,(void**)&temp);
		temp->lag_num = i++;
	}while(llist_go_next(lags) != LLIST_END_OF_LIST);*/

	/***Find lags from the lag table****/
	for(i=0;i<fit_prms->mplgs;i++){
		temp = malloc(sizeof(LAGNODE));
		temp->pulse_diff = fit_prms->lag[1][i] - fit_prms->lag[0][i];
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
		temp->lag_num = i++;
	}while(llist_go_next(lags) != LLIST_END_OF_LIST);
}


/**
For each range, this function creates lists of associated ACF phases, XCF phases and ACF pwrs
for each lag of that range from the raw data.
*/
void Fill_Data_Lists_For_Range(llist_node range,llist lags,FITPRMS *fit_prms){
	RANGENODE* range_node;
	ACFNODE* acf_node;
	ACFNODE* xcf_node;
	PWRNODE* pwr_node;
	SCNODE* sc_node;
	LAGNODE* lag;
	int i;

	range_node = (RANGENODE*) range;

	range_node->pwrs = llist_create(compare_doubles,pwr_node_eq,0);
	range_node->acf = llist_create(compare_doubles,acf_node_eq,0);
	if (fit_prms->xcf == 1){
	    range_node->xcf = llist_create(compare_doubles,acf_node_eq,0);
	}
	range_node->scpwr = llist_create(compare_doubles,sc_node_eq,0);
	range_node->phases = llist_create(compare_doubles,phase_node_eq,0);
	range_node->elev = llist_create(compare_doubles,phase_node_eq,0);

	/* llist_reset_iter(range_node->alpha_2); */
	llist_reset_iter(lags);

	for(i=0;i<fit_prms->mplgs;i++){

		/* llist_get_iter(range_node->alpha_2,(void**)&alpha_2); */
		llist_get_iter(lags,(void**)&lag);

		pwr_node = new_pwr_node(range_node->range,lag,fit_prms);
		acf_node = new_acf_node(range_node->range,lag,fit_prms);
        if (fit_prms->xcf == 1){
		    xcf_node = new_xcf_node(range_node->range,lag,fit_prms);
	    }
		sc_node = new_sc_node(range_node->range,lag,fit_prms);
		if(pwr_node != NULL){ /* A NULL ptr means the pwr level was too low*/
			llist_add_node(range_node->pwrs,(llist_node)pwr_node,0);
			llist_add_node(range_node->acf,(llist_node)acf_node,0);
			llist_add_node(range_node->xcf,(llist_node)xcf_node,0);
		}
		llist_add_node(range_node->scpwr,(llist_node)sc_node,0);

		llist_go_next(lags);
	}
}

/* Check to see if lists have any data in them, if not, delete the range node */
void Check_Range_Nodes(llist ranges){
    RANGENODE* range_node;
    int status;

    llist_reset_iter(ranges);
	do
	{
		llist_get_iter(ranges,(void**)&range_node);
/*		fprintf(stderr,"range: %d pwrs: %d \n", range_node->range, llist_size(range_node->pwrs)); */
		if (llist_size(range_node->pwrs) == 0) {
		   	if(range_node == llist_peek(ranges)){ 
				llist_delete_node(ranges,&range_node->range,TRUE,free_range_node);
				status = 0;

			}else{
				llist_delete_node(ranges,&range_node->range,TRUE,free_range_node);
				status = llist_go_next(ranges);
			}
		}else{
			status = llist_go_next(ranges);
		}
	}while(status != LLIST_END_OF_LIST);

/*    llist_reset_iter(ranges);
	do
	{
		llist_get_iter(ranges,(void**)&range_node);
		fprintf(stderr,"range: %d pwrs: %d \n", range_node->range, llist_size(range_node->pwrs));
	}while(llist_go_next(ranges) != LLIST_END_OF_LIST); */
		
}

/**
This function determines the minimum power level for which an ACF is pure
noise. This is used to filter bad ACFs.
*/
double ACF_cutoff_pwr(FITPRMS *fit_prms){
    int i;
    double min_pwr = 0.0;
    double *pwr_levels;
    int ni = 0;

    pwr_levels = malloc(fit_prms->nrang * sizeof(double));

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

    qsort(pwr_levels, fit_prms->nrang, sizeof(double), dbl_cmp);

    /* determine the average lag0 power of the 10 lowest power acfs */
    /*  look for the lowest 10 values of lag0 power and average to 
            get the noise level.  Ignore values that are exactly 0.  If
            you can't find 10 useable values within the first 1/3 of the
            sorted power list, then just use whatever you got in that 
            first 1/3.  If you didn't get any useable values, then use
            the NOISE parameter */
    i=0;    
    while ((ni < 10) && (i < fit_prms->nrang/3)) {
        if (pwr_levels[i]) ++ni;
        min_pwr += pwr_levels[i++];  
    }

    ni = (ni > 0) ? ni :  1;
    min_pwr = min_pwr/ni;
    if (min_pwr < 1.0) min_pwr = fit_prms->noise;

    free(pwr_levels);
    return min_pwr;
}


/**
For each range, this function estimates the self clutter in each lag of the ACF.
*/
bool Is_Converged(llist *ranges,double tol){
	RANGENODE* range_node;
    bool converged = true;

    /* Check each fitted value against the previously obtained fitted value */
    llist_reset_iter(ranges);
	do
	{
		llist_get_iter(ranges,(void**)&range_node);
        if (fabs(range_node->l_acf_fit->P - range_node->prev_pow) > tol*fabs(range_node->l_acf_fit->P)) 
        {
        	/*fprintf(stderr,"range: %d pow: %f    %f\n",range_node->range,range_node->l_pwr_fit->a, range_node->prev_pow);*/
        	converged = false;
        	break;
        }
        if (fabs(range_node->l_acf_fit->wid - range_node->prev_width) > tol*fabs(range_node->l_acf_fit->wid))
        {
        	/*fprintf(stderr,"range: %d width: %f    %f\n",range_node->range,range_node->l_pwr_fit->b, range_node->prev_width);*/
        	converged = false;
        	break;
        }
        if (fabs(range_node->l_acf_fit->vel - range_node->prev_phase) > tol*fabs(range_node->l_acf_fit->vel))
        {
        	/*fprintf(stderr,"range: %d phases: %f    %f\n",range_node->range,range_node->phase_fit->b, range_node->prev_phase);*/
        	converged = false;
        	break;
        }
	} while(llist_go_next(ranges) != LLIST_END_OF_LIST);

    /* If we converged then we can simply return! If not, we need to save the current fitted
       values and use them to check convergence on the next iteration */
    if (converged)
    {
    	return true;
    } else {
    	llist_reset_iter(ranges);
		do
		{
			llist_get_iter(ranges,(void**)&range_node);
	        range_node->prev_pow = range_node->l_acf_fit->P;
	        range_node->prev_width = range_node->l_acf_fit->wid;
	        range_node->prev_phase = range_node->l_acf_fit->vel;

		} while(llist_go_next(ranges) != LLIST_END_OF_LIST);
    	return false;
    }
}


/**
For each range, this function estimates the self clutter in each lag of the ACF.
*/
void Estimate_Self_Clutter(llist *ranges,FITPRMS *fit_prms){
	RANGENODE* range_node;
	SCNODE* sc_node;
	PWRNODE* pwr_node;
	double lag0pwrs[fit_prms->nrang];
	double selfclutter[fit_prms->mplgs];
	
	/* initialize some things */
	int i=0;
    for(i=0;i<fit_prms->nrang;i++)
    {
    	lag0pwrs[i] = 0;
    }
    for(i=0;i<fit_prms->mplgs;i++)
    {
    	selfclutter[i] = 0;
    }

    /* Now get a list of the fitted lag0 power from each range gate. */
    llist_reset_iter(ranges);
	do
	{
		llist_get_iter(ranges,(void**)&range_node);
		llist_reset_iter(range_node->pwrs);
		llist_get_iter(range_node->pwrs,(void**)&pwr_node);
		lag0pwrs[range_node->range] = pwr_node->pwr;

	}while(llist_go_next(ranges) != LLIST_END_OF_LIST);

    /* Now iterate over all the ranges and estimate the self clutter */
    llist_reset_iter(ranges);
	do
	{
		llist_get_iter(ranges,(void**)&range_node);

		/* Estimate the self clutter at range_node->range */
		Estimate_Maximum_Self_Clutter(range_node->range,fit_prms,lag0pwrs,selfclutter);

        /* Now add self clutter estimate to range_node */
        llist_reset_iter(range_node->scpwr);
        do
        {
        	llist_get_iter(range_node->scpwr,(void**)&sc_node);
        	sc_node->clutter = selfclutter[sc_node->lag_num];
        }while(llist_go_next(range_node->scpwr) != LLIST_END_OF_LIST);
    }while(llist_go_next(ranges) != LLIST_END_OF_LIST);
}


void Estimate_First_Order_Error(llist *ranges,FITPRMS *fit_prms, double noise_pwr){
	RANGENODE* range_node;
	PWRNODE* pwr_node;
	ACFNODE* acf_node;
	SCNODE* sc_node;
	double lag0pwr, selfclutter, error;

    /* Now iterate over all the ranges and lags and estimate the error */
    /* in the magnitude: approx = (S + N + C)/sqrt(nave) */
    llist_reset_iter(ranges);
	do
	{
		llist_get_iter(ranges,(void**)&range_node);

        /* Get lag0 power */
        llist_reset_iter(range_node->pwrs);
        llist_get_iter(range_node->pwrs,(void**)&pwr_node);
        lag0pwr = pwr_node->pwr;

        /* Now add self clutter estimate to range_node */
        llist_reset_iter(range_node->acf);
        llist_reset_iter(range_node->scpwr);

        do
        {
        	llist_get_iter(range_node->acf, (void**)&acf_node);
        	llist_get_iter(range_node->scpwr,(void**)&sc_node);
        	selfclutter = sc_node->clutter;

            /* first order error bar */
        	error = (lag0pwr + noise_pwr + selfclutter)/sqrt(fit_prms->nave);

        	pwr_node->sigma = error;
        	acf_node->sigma_re = error;
        	acf_node->sigma_im = error;

        	llist_go_next(range_node->acf);
        }while(llist_go_next(range_node->scpwr) != LLIST_END_OF_LIST);
    }while(llist_go_next(ranges) != LLIST_END_OF_LIST);
}


void Estimate_Re_Im_Error(llist *ranges,FITPRMS *fit_prms, double noise_pwr){
	RANGENODE* range_node;
	ACFNODE* acf_node;
	SCNODE* sc_node;
	double selfclutter;

	double rho, rho_r, rho_i;
	double P, wid, vel, t, re_error, im_error;

	const double lambda = 299792458.0/(fit_prms->tfreq*1000.0);

    /* Now iterate over all the ranges and lags and estimate the error */
    /* in the magnitude: approx = (S + N + C)/sqrt(nave) */
    llist_reset_iter(ranges);
	do
	{
		llist_get_iter(ranges,(void**)&range_node);

        P = range_node->l_acf_fit->P;
        wid = range_node->l_acf_fit->wid;
        vel = range_node->l_acf_fit->vel;

        /* Now add self clutter estimate to range_node */
        llist_reset_iter(range_node->acf);
        llist_reset_iter(range_node->scpwr);
        do
        {
        	llist_get_iter(range_node->acf, (void**)&acf_node);
        	llist_get_iter(range_node->scpwr,(void**)&sc_node);
        	selfclutter = sc_node->clutter;
        	t = acf_node->t;

        	/* Use fitted parameters to estimate estimation errors */
        	/* assume that ACF decays exponentially */
            rho = exp(-2.0*M_PI*wid*t/lambda);

            if (rho > 0.999){
	            rho = 0.999;
	        }
            rho = rho * P/(P + noise_pwr + selfclutter);
            rho_r = rho*cos(4*M_PI*vel*t/lambda);
            rho_i = rho*sin(4*M_PI*vel*t/lambda);


            /* first order error bar */
        	re_error = (P + noise_pwr + selfclutter)*sqrt((1-rho*rho)/2.0 + (rho_r*rho_r))/sqrt(fit_prms->nave);
        	im_error = (P + noise_pwr + selfclutter)*sqrt((1-rho*rho)/2.0 + (rho_i*rho_i))/sqrt(fit_prms->nave);

        	acf_node->sigma_re = re_error;
        	acf_node->sigma_im = im_error;

        	llist_go_next(range_node->acf);
        }while(llist_go_next(range_node->scpwr) != LLIST_END_OF_LIST);
    }while(llist_go_next(ranges) != LLIST_END_OF_LIST);
}
