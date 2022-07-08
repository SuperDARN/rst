/*
ACF determinations from fitted parameters

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

#include "lmfit_determinations.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**
Returns a newly allocated array of FitRanges to fill
*/
struct FitRange* new_range_array(FITPRMS* fit_prms){
	struct FitRange* new_range_array;
	new_range_array = malloc(sizeof(struct FitRange) * fit_prms->nrang);
	if(new_range_array == NULL) return NULL;

	return new_range_array;
}


void allocate_fit_data(struct FitData* fit_data, FITPRMS* fit_prms){
	if(fit_data->rng == NULL) fit_data->rng = new_range_array(fit_prms);
    if(fit_data->rng == NULL){
    	fprintf(stderr,"COULD NOT ALLOCATE fit_data->rng\n");
    }
    else{
    	memset(fit_data->rng,0,sizeof(struct FitRange) * fit_prms->nrang);
    }

    if(fit_data->xrng == NULL) fit_data->xrng = new_range_array(fit_prms);
    if(fit_data->rng == NULL){
    	fprintf(stderr,"COULD NOT ALLOCATE fit_data->xrng\n");
    }
    else{
    	memset(fit_data->xrng,0,sizeof(struct FitRange) * fit_prms->nrang);
    }

   	if(fit_data->elv == NULL) fit_data->elv = malloc(sizeof(struct FitElv) * fit_prms->nrang);
   	if (fit_data->elv == NULL){
   		fprintf(stderr,"COULD NOT ALLOCATE fit_data->xrng\n");
   	}
   	else{
		memset(fit_data->elv,0,sizeof(struct FitElv) * fit_prms->nrang);
   	}
}


/**
Performs all the determinations for parameters from the fitted data for all good ranges
*/
void ACF_Determinations(llist ranges, FITPRMS* fit_prms,struct FitData* fit_data,double noise_pwr){

	fit_data->revision.major=3;
    fit_data->revision.minor=0;

    allocate_fit_data(fit_data,fit_prms);

   	fit_data->noise.vel = 0.0;
   	fit_data->noise.skynoise = noise_pwr;
   	fit_data->noise.lag0 = 0.0;

   	lag_0_pwr_in_dB(fit_data->rng,fit_prms,noise_pwr);

   	llist_for_each_arg(ranges,(node_func_arg)find_elevation,fit_data->elv,fit_prms);
   	llist_for_each_arg(ranges,(node_func_arg)set_xcf_phi0,fit_data->xrng,NULL);
   	llist_for_each_arg(ranges,(node_func_arg)set_xcf_phi0_err,fit_data->xrng,NULL);

#ifdef _RFC_IDX
   	llist_for_each_arg(ranges,(node_func_arg)refractive_index,fit_data->elv,NULL);
#endif

   	llist_for_each_arg(ranges,(node_func_arg)set_qflg,fit_data->rng,NULL);
   	llist_for_each_arg(ranges,(node_func_arg)set_p_l,fit_data->rng,&noise_pwr);
   	llist_for_each_arg(ranges,(node_func_arg)set_p_l_err,fit_data->rng,&noise_pwr);
   	llist_for_each_arg(ranges,(node_func_arg)set_s_l,fit_data->rng,&noise_pwr);
   	llist_for_each_arg(ranges,(node_func_arg)set_s_l_err,fit_data->rng,&noise_pwr);
   	llist_for_each_arg(ranges,(node_func_arg)set_v,fit_data->rng,fit_prms);
   	llist_for_each_arg(ranges,(node_func_arg)set_v_err,fit_data->rng,fit_prms);
   	llist_for_each_arg(ranges,(node_func_arg)set_w_l,fit_data->rng,fit_prms);
   	llist_for_each_arg(ranges,(node_func_arg)set_w_l_err,fit_data->rng,fit_prms);
   	llist_for_each_arg(ranges,(node_func_arg)set_w_s,fit_data->rng,fit_prms);
   	llist_for_each_arg(ranges,(node_func_arg)set_w_s_err,fit_data->rng,fit_prms);
   	llist_for_each_arg(ranges,(node_func_arg)set_sdev_l,fit_data->rng,NULL);
   	llist_for_each_arg(ranges,(node_func_arg)set_sdev_s,fit_data->rng,NULL);
   	llist_for_each_arg(ranges,(node_func_arg)set_sdev_phi,fit_data->rng,NULL);
   	llist_for_each_arg(ranges,(node_func_arg)set_gsct,fit_data->rng,NULL);
   	llist_for_each_arg(ranges,(node_func_arg)set_nump,fit_data->rng,NULL);
}


/**
Finds the refractive index for a particular range.
*/
void refractive_index(llist_node range, struct FitElv* fit_elev_array){
	double height;
	RANGENODE *range_node;
	double cos_elev_angle;
	double height_ratio;

	range_node = (RANGENODE*) range;

	height = (range_node->range <= 10) ? CLOSE_GATE_HEIGHT : FAR_GATE_HEIGHT;

	cos_elev_angle = cos(M_PI/180 * fit_elev_array[range_node->range].normal);

	height_ratio = (EARTH_RADIUS/(EARTH_RADIUS + height));

	range_node->refrc_idx =  height_ratio * (cos_elev_angle/MAGNETIC_DIP_ANGLE);
}


/**
Converts lag 0 powers to dB
*/
void lag_0_pwr_in_dB(struct FitRange* fit_range_array,FITPRMS* fit_prms,double noise_pwr){
	int i;

	for(i=0;i<fit_prms->nrang;i++){
		if((fit_prms->pwr0[i] - noise_pwr) > 0.00001){
			fit_range_array[i].p_0 = 10 * log10((fit_prms->pwr0[i] - noise_pwr) / noise_pwr);
		}
		else{
			fit_range_array[i].p_0 = -50.0;
		}
	}
}


/**
Sets a flag showing that data is valid
*/
void set_qflg(llist_node range,struct FitRange* fit_range_array){
	RANGENODE* range_node;

	range_node = (RANGENODE*) range;

	fit_range_array[range_node->range].qflg = 1;
}


/**
Sets the value of the linear fitted lag 0 power in dB
*/
void set_p_l(llist_node range, struct FitRange* fit_range_array, double* noise_pwr){
	RANGENODE* range_node;
	double noise_dB;

	range_node = (RANGENODE*) range;
	noise_dB = 10 * log10( *noise_pwr);

	fit_range_array[range_node->range].p_l = 10 * log10(range_node->l_acf_fit->P) - noise_dB;
}


/**
Sets the value of the linear fitted lag 0 power error in dB
*/
void set_p_l_err(llist_node range, struct FitRange* fit_range_array){
	RANGENODE* range_node;

	range_node = (RANGENODE*) range;

	fit_range_array[range_node->range].p_l_err = sqrt(range_node->l_acf_fit->sigma_2_P) / (log(10.) * range_node->l_acf_fit->P);  /* Here pwr_fit_err!*/
}


/**
Sets the value of the quadratic fitted lag 0 power in dB
*/
void set_s_l(llist_node range, struct FitRange* fit_range_array, double* noise_pwr){
	RANGENODE* range_node;
	double noise_dB;

	range_node = (RANGENODE*) range;
	noise_dB = 10 * log10( *noise_pwr);

	fit_range_array[range_node->range].p_s = 10 * log10(range_node->q_acf_fit->P) - noise_dB;
}


/**
Sets the value of the quadratic fitted lag 0 power error in dB
*/
void set_s_l_err(llist_node range, struct FitRange* fit_range_array){
	RANGENODE* range_node;

	range_node = (RANGENODE*) range;

	fit_range_array[range_node->range].p_s_err = sqrt(range_node->q_acf_fit->sigma_2_P) / (log(10.) * range_node->q_acf_fit->P);
}


/**
Sets the value of the determined velocity from the phase fit
*/
void set_v(llist_node range, struct FitRange* fit_range_array, FITPRMS* fit_prms){
	RANGENODE* range_node;
	double velocity;

	range_node = (RANGENODE*) range;

	velocity = range_node->l_acf_fit->vel * (1/range_node->refrc_idx);

	fit_range_array[range_node->range].v = velocity;
}


/**
Sets the value of the determined velocity error from the phase fit
*/
void set_v_err(llist_node range, struct FitRange* fit_range_array, FITPRMS* fit_prms){
	RANGENODE* range_node;
	double velocity_err;

	range_node = (RANGENODE*) range;

	velocity_err = sqrt(range_node->l_acf_fit->sigma_2_vel) * (1/range_node->refrc_idx);

	fit_range_array[range_node->range].v_err = velocity_err;
}


/**
Sets the value of the determined spectral width from the linear power fit
*/
void set_w_l(llist_node range, struct FitRange* fit_range_array, FITPRMS* fit_prms){
	RANGENODE* range_node;

	range_node = (RANGENODE*) range;

	fit_range_array[range_node->range].w_l = range_node->l_acf_fit->wid;
}


/**
Sets the value of the determined spectral width error from the linear power fit
*/
void set_w_l_err(llist_node range, struct FitRange* fit_range_array, FITPRMS* fit_prms){
	RANGENODE* range_node;

	range_node = (RANGENODE*) range;

	fit_range_array[range_node->range].w_l_err = sqrt(range_node->l_acf_fit->sigma_2_wid);
}


/**
Sets the value of the determined spectral width from the quadratic power fit
*/
void set_w_s(llist_node range, struct FitRange* fit_range_array, FITPRMS* fit_prms){
	RANGENODE* range_node;

	range_node = (RANGENODE*) range;

	fit_range_array[range_node->range].w_s = range_node->q_acf_fit->wid;
}


/**
Sets the value of the determined spectral width error from the quadratic power fit
*/
void set_w_s_err(llist_node range, struct FitRange* fit_range_array, FITPRMS* fit_prms){
	RANGENODE* range_node;

	range_node = (RANGENODE*) range;

	fit_range_array[range_node->range].w_s_err = range_node->q_acf_fit->sigma_2_wid;
}


/**
Sets the value of chi squared from the linear power fit
*/
void set_sdev_l(llist_node range, struct FitRange* fit_range_array){
	RANGENODE* range_node;

	range_node = (RANGENODE*) range;

	fit_range_array[range_node->range].sdev_l = range_node->l_acf_fit->chi_2;
}


/**
Sets the value of chi squared from the quadratic power fit
*/
void set_sdev_s(llist_node range, struct FitRange* fit_range_array){
	RANGENODE* range_node;

	range_node = (RANGENODE*) range;

	fit_range_array[range_node->range].sdev_s = range_node->q_acf_fit->chi_2;
}


/**
Sets the value of chi squared from the phase fit
*/
void set_sdev_phi(llist_node range, struct FitRange* fit_range_array){
	RANGENODE* range_node;

	range_node = (RANGENODE*) range;

	fit_range_array[range_node->range].sdev_phi = -1;
}


/**
Sets the flag of whether a range is ground scatter
*/
void set_gsct(llist_node range, struct FitRange* fit_range_array){
	RANGENODE* range_node;
	double v_abs,w;

	range_node = (RANGENODE*) range;

	v_abs = fabs(fit_range_array[range_node->range].v);
	w = fit_range_array[range_node->range].w_l;
	fit_range_array[range_node->range].gsct = (v_abs - w * (V_max/W_max) < 0) ? 1 : 0;
}


/**
Sets the number of good points used in the power fitting
*/
void set_nump(llist_node range, struct FitRange* fit_range_array){
	RANGENODE* range_node;

	range_node = (RANGENODE*) range;

	fit_range_array[range_node->range].nump = llist_size(range_node->pwrs);
}


/**
Determines the elevation angle from the fitted XCF phase
*/
void find_elevation(llist_node range, struct FitElv* fit_elev_array, FITPRMS* fit_prms){
	double x,y,z;
	double antenna_sep,elev_corr;
	int phi_sign;
	double azi_offset,phi_0,c_phi_0;
	double wave_num;
	double cable_offset;
	double phase_diff_max;
	double psi_uncorrected;
	double psi,theta,psi_kd,psi_k2d2,df_by_dy;
	double elevation;
	double real,imag;
	double psi_uncorrected_unfitted;
	double xcf0_p;

	RANGENODE* range_node;

	range_node = (RANGENODE*) range;

	x = fit_prms->interfer_x;
	y = fit_prms->interfer_y;
	z = fit_prms->interfer_z;

	antenna_sep = sqrt(x*x + y*y + z*z);

	elev_corr = fit_prms->phidiff * asin(z/antenna_sep);

	if (y > 0.0){
		phi_sign = 1;
	}
	else{
		phi_sign = -1;
		elev_corr = -elev_corr;
	}

	azi_offset = fit_prms->maxbeam/2 - 0.5;
	phi_0 = fit_prms->bmsep * (fit_prms->bmnum - azi_offset) * M_PI/180;
	c_phi_0 = cos(phi_0);

	wave_num = 2 * M_PI * fit_prms->tfreq * 1000/C;

	cable_offset = -2 * M_PI * fit_prms->tfreq * 1000 * fit_prms->tdiff * 1.0e-6;

	phase_diff_max = phi_sign * wave_num * antenna_sep * c_phi_0 + cable_offset;

	psi_uncorrected = range_node->l_xcf_fit->phi0 + 2 * M_PI * floor((phase_diff_max-range_node->l_xcf_fit->phi0)/(2*M_PI));

	if(phi_sign < 0) psi_uncorrected += 2 * M_PI;

	psi = psi_uncorrected - cable_offset;

	psi_kd = psi/(wave_num * antenna_sep);
	theta = c_phi_0 * c_phi_0 - psi_kd * psi_kd;

	if( (theta < 0.0) || (fabs(theta) > 1.0) ){
		elevation = -elev_corr;
	}
	else{
		elevation = asin(sqrt(theta));
	}

	fit_elev_array[range_node->range].normal = 180/M_PI * (elevation + elev_corr);

	/*Elevation errors*/
	psi_k2d2 = psi/(wave_num * wave_num * antenna_sep * antenna_sep);
	df_by_dy = psi_k2d2/sqrt(theta * (1 - theta));
	fit_elev_array[range_node->range].low = 180/M_PI * sqrt(range_node->l_xcf_fit->sigma_2_phi0) * fabs(df_by_dy);

	/*Experiment to compare fitted and measured elevation*/
	real = fit_prms->xcfd[range_node->range * fit_prms->mplgs][0];
	imag = fit_prms->xcfd[range_node->range * fit_prms->mplgs][1];
	xcf0_p = atan2(imag,real);

	psi_uncorrected_unfitted = xcf0_p + 2 * M_PI * floor((phase_diff_max-xcf0_p)/(2*M_PI));

	if(phi_sign < 0) psi_uncorrected_unfitted += 2 * M_PI;

	psi = psi_uncorrected_unfitted - cable_offset;

	psi_kd = psi/(wave_num * antenna_sep);
	theta = c_phi_0 * c_phi_0 - psi_kd * psi_kd;

	if( (theta < 0.0) || (fabs(theta) > 1.0) ){
		elevation = -elev_corr;
	}
	else{
		elevation = asin(sqrt(theta));
	}
	fit_elev_array[range_node->range].high = 180/M_PI * (elevation + elev_corr);
}


/**
Sets the fitted phase offset for the XCF
*/
void set_xcf_phi0(llist_node range, struct FitRange* fit_range_array, FITPRMS* fit_prms){
	RANGENODE* range_node;

	range_node = (RANGENODE*) range;

	fit_range_array[range_node->range].phi0 = range_node->l_xcf_fit->phi0;
}


/**
Sets the fitted phase offset error for the XCF
*/
void set_xcf_phi0_err(llist_node range, struct FitRange* fit_range_array){
	RANGENODE* range_node;

	range_node = (RANGENODE*) range;

	fit_range_array[range_node->range].phi0_err = sqrt(range_node->l_xcf_fit->sigma_2_phi0);
}


/**
Sets the fitted phase chi squared value for the XCF
*/
void set_xcf_sdev_phi(llist_node range, struct FitRange* fit_range_array){
	RANGENODE* range_node; 

	range_node = (RANGENODE*) range;

	fit_range_array[range_node->range].sdev_phi = range_node->l_xcf_fit->chi_2;
}










