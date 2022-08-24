/*
 ACF Processing main functions

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

*/


#ifndef _ACFPROC_H
#define _ACFPROC_H
#include "rtypes.h"
#include "llist.h"
#include "leastsquares.h"
#include "fit_structures.h"

#define MIN_LAGS 3
#define ALPHA_CUTOFF 2
#define NOISE_SIGMA_THRESHOLD 3.0
#define FLUCTUATION_CUTOFF_COEFF 2



RANGENODE* new_range_node(int range, FITPRMS *fit_prms);
PWRNODE* new_pwr_node(int range, double alpha_2, LAGNODE* lag, FITPRMS *fit_prms);
PHASENODE* new_phase_node(int range, double alpha_2, LAGNODE* lag, FITPRMS *fit_prms);
PHASENODE* new_elev_node(int range, double alpha_2, LAGNODE* lag, FITPRMS *fit_prms);
void free_range_node(llist_node range);

int compare_doubles(llist_node first,llist_node second);
int compare_ints(llist_node first,llist_node second);
int compare_lags(llist_node first,llist_node second);
bool pwr_node_eq(llist_node node,llist_node cmp);
bool phase_node_eq(llist_node node,llist_node cmp);
bool sample_node_eq(llist_node node,llist_node cmp);
bool range_node_eq(llist_node node,llist_node cmp);
bool alpha_node_eq(llist_node node,llist_node cmp);

void mark_bad_samples(FITPRMS *fit_prms, llist bad_samples);
void filter_tx_overlapped_lags(llist_node range, llist lags, llist bad_samples);
double ACF_cutoff_pwr(FITPRMS *fit_prms);
double cutoff_power_correction(FITPRMS *fit_prms);
void phase_correction(PHASENODE* phase, double* slope_est, int* total_2pi_corrections);


void Determine_Lags(llist lags,FITPRMS *fit_prms);
void Fill_Range_List(FITPRMS *fit_prms, llist good_ranges);
void Filter_Bad_ACFs(FITPRMS *fit_prms, llist good_ranges, double noise_pwr);
void Find_CRI(llist_node range,FITPRMS *fit_prms);
void Find_Alpha(llist_node range,llist lags,FITPRMS *fit_prms);
void Fill_Data_Lists_For_Range(llist_node range,llist lags,FITPRMS *fit_prms);
void ACF_Phase_Unwrap(llist_node range, FITPRMS* fit_prms);
void XCF_Phase_Unwrap(llist_node range);
void Filter_Low_Pwr_Lags(llist_node range, FITPRMS* fit_prms);
void Filter_TX_Overlap(llist ranges, llist lags, FITPRMS *fit_prms);
void Filter_Bad_Fits(llist ranges);




#endif
