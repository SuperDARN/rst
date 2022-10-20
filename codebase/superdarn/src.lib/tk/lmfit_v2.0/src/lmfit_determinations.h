/*
 ACF determination functions

 Copyright (c) 2015 University of Saskatchewan
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
 along with this program. If not, see <https://www.gnu.org/licenses/>.

 Modifications:


*/



#ifndef _ACFDETERMINATIONS_H
#define _ACFDETERMINATIONS_H

#include "lmfit_structures.h"
#include "fitdata.h"
#include "fitblk.h"
#include "llist.h"

#define LN_TO_LOG 1/log(10)
#define V_max 30.0
#define W_max 90.0
#define C 299792458 
#define CLOSE_GATE_HEIGHT 120.0
#define FAR_GATE_HEIGHT 250.0
#define EARTH_RADIUS 6371.0
#define MAGNETIC_DIP_ANGLE 1.0


struct FitRange* new_range_array(FITPRMS* fit_prms);
void ACF_Determinations(llist ranges, FITPRMS* fit_prms,struct FitData* fit_data,double noise_pwr);

void lag_0_pwr_in_dB(struct FitRange* fit_range_array,FITPRMS* fit_prms,double noise_pwr);
void set_qflg(llist_node range,struct FitRange* fit_range_array);
void set_p_l(llist_node range, struct FitRange* fit_range_array, double* noise_pwr);
void set_p_l_err(llist_node range, struct FitRange* fit_range_array);
void set_s_l(llist_node range, struct FitRange* fit_range_array, double* noise_pwr);
void set_s_l_err(llist_node range, struct FitRange* fit_range_array);
void set_v(llist_node range, struct FitRange* fit_range_array, FITPRMS* fit_prms);
void set_v_err(llist_node range, struct FitRange* fit_range_array, FITPRMS* fit_prms);
void set_w_l(llist_node range, struct FitRange* fit_range_array, FITPRMS* fit_prms);
void set_w_l_err(llist_node range, struct FitRange* fit_range_array, FITPRMS* fit_prms);
void set_w_s(llist_node range, struct FitRange* fit_range_array, FITPRMS* fit_prms);
void set_w_s_err(llist_node range, struct FitRange* fit_range_array, FITPRMS* fit_prms);
void set_sdev_l(llist_node range, struct FitRange* fit_range_array);
void set_sdev_s(llist_node range, struct FitRange* fit_range_array);
void set_sdev_phi(llist_node range, struct FitRange* fit_range_array);
void set_gsct(llist_node range, struct FitRange* fit_range_array);
void set_nump(llist_node range, struct FitRange* fit_range_array);

void find_elevation(llist_node range, struct FitElv* fit_elev_array, FITPRMS* fit_prms);
void set_xcf_phi0(llist_node range, struct FitRange* fit_range_array, FITPRMS* fit_prms);
void set_xcf_phi0_err(llist_node range, struct FitRange* fit_range_array);
void set_xcf_sdev_phi(llist_node range, struct FitRange* fit_range_array);

void refractive_index(llist_node range, struct FitElv* fit_elev_array);


#endif
