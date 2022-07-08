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

#ifndef _ACFPROC_H
#define _ACFPROC_H

#include "llist.h"
#include "lmfit_leastsquares.h"
#include "lmfit_structures.h"

#define MIN_LAGS 3
#define ACF_SNR_CUTOFF 10.0
#define ALPHA_CUTOFF 2
#define FLUCTUATION_CUTOFF_COEFF 2

RANGENODE* new_range_node(int range, FITPRMS *fit_prms);
PWRNODE* new_pwr_node(int range, LAGNODE* lag, FITPRMS *fit_prms);
ACFNODE* new_acf_node(int range, LAGNODE* lag, FITPRMS *fit_prms);
SCNODE* new_sc_node(int range, LAGNODE* lag, FITPRMS *fit_prms);
PHASENODE* new_elev_node(int range, LAGNODE* lag, FITPRMS *fit_prms);
void free_range_node(llist_node range);

int compare_doubles(llist_node first,llist_node second);
int compare_ints(llist_node first,llist_node second);
int compare_lags(llist_node first,llist_node second);
bool pwr_node_eq(llist_node node,llist_node cmp);
bool acf_node_eq(llist_node node,llist_node cmp);
bool phase_node_eq(llist_node node,llist_node cmp);
bool sample_node_eq(llist_node node,llist_node cmp);
bool range_node_eq(llist_node node,llist_node cmp);
void print_acf_node(llist_node node,FILE* fp);

void print_node(llist_node node);
void print_range_node(llist_node node);
void print_lag_node(llist_node node);
void print_pwr_node(llist_node node,FILE* fp);
void print_scpwr_node(llist_node node,FILE* fp);

void mark_bad_samples(FITPRMS *fit_prms, llist bad_samples);
void filter_tx_overlapped_lags(llist_node range, llist lags, llist bad_samples);
double ACF_cutoff_pwr(FITPRMS *fit_prms);

void Determine_Lags(llist lags,FITPRMS *fit_prms);
void Fill_Range_List(FITPRMS *fit_prms, llist good_ranges);
void Fill_Data_Lists_For_Range(llist_node range,llist lags,FITPRMS *fit_prms);
void Filter_TX_Overlap(llist ranges, llist lags, FITPRMS *fit_prms);

bool Is_Converged(llist *ranges,double tol);
void Estimate_Self_Clutter(llist *ranges,FITPRMS *fit_prms);
void Estimate_First_Order_Error(llist *ranges,FITPRMS *fit_prms, double noise_pwr);
void Estimate_Re_Im_Error(llist *ranges,FITPRMS *fit_prms, double noise_pwr);
void Check_Range_Nodes(llist ranges);


#endif
