/* fit_mem_helpers.c
     =========
     refactored from fit_acf.c by Pasha and AJ
     Author: R.J.Barnes & K.Baker & P.Ponomarenko
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


#include <math.h>
#include <stdlib.h>

#include "fitblk.h"
#include "fit_mem_helpers.h"


void free_arrays(double **sum_wk2_arr, double **phi_res, double **tau, 
                    double **tau2, double **phi_k, double **w, double **pwr, 
                    double **wt, double **wt2, double **wp, int **bad_pwr){
    if (*sum_wk2_arr != NULL) free(*sum_wk2_arr);
    if (*phi_res != NULL)  free(*phi_res);
    if (*tau != NULL) free(*tau);
    if (*tau2 != NULL) free(*tau2);
    if (*phi_k != NULL) free(*phi_k);
    if (*w != NULL) free(*w);
    if (*pwr != NULL) free(*pwr);
    if (*wt != NULL) free(*wt);
    if (*wt2 != NULL) free(*wt2);
    if (*wp != NULL) free(*wp);
    if (*bad_pwr != NULL) free(*bad_pwr);
}

void zero_fitrange(struct FitRange *ptr){
    ptr->p_l = 0.0;
    ptr->p_s = 0.0;
    ptr->p_l_err = 0.0;
    ptr->p_s_err = 0.0;
    ptr->v = 0.0;
    ptr->v_err = 0.0;
    ptr->w_l = 0.0;
    ptr->w_l_err = 0.0;
    ptr->w_s = 0.0;
    ptr->w_s_err = 0.0;
    ptr->phi0 = 0.0;
    ptr->phi0_err = 0.0;
    ptr->sdev_l = 0.0;
    ptr->sdev_s = 0.0;
}

/*allocate the arrays for the least squares fit*/
int allocate_ls_arrays(struct FitPrm *prm, double **sum_wk2_arr, double **phi_res, double **tau, 
                    double **tau2, double **phi_k, double **w, double **pwr, 
                    double **wt, double **wt2, double **wp, int **bad_pwr){

    int s=0;

    *sum_wk2_arr=malloc(sizeof(double)*prm->mplgs);
    if (*sum_wk2_arr==NULL) s=-1;
    if (s==0) *phi_res=malloc(sizeof(double)*prm->mplgs);
    if (*phi_res==NULL) s=-1;
    if (s==0) *tau=malloc(sizeof(double)*prm->mplgs);
    if (*tau==NULL) s=-1;
    if (s==0) *tau2=malloc(sizeof(double)*prm->mplgs);
    if (*tau2==NULL) s=-1;
    if (s==0) *phi_k=malloc(sizeof(double)*prm->mplgs);
    if (*phi_k==NULL) s=-1;
    if (s==0) *w=malloc(sizeof(double)*prm->mplgs);
    if (*w==NULL) s=-1;
    if (s==0) *pwr=malloc(sizeof(double)*prm->mplgs);
    if (*pwr==NULL) s=-1;
    if (s==0) *wt=malloc(sizeof(double)*prm->mplgs);
    if (*wt==NULL) s=-1;
    if (s==0) *wt2=malloc(sizeof(double)*prm->mplgs);
    if (*wt2==NULL) s=-1;
    if (s==0) *wp=malloc(sizeof(double)*prm->mplgs);
    if (*wp==NULL) s=-1;
    if (s==0) *bad_pwr=malloc(sizeof(int)*prm->mplgs);
    if (*bad_pwr==NULL) s=-1;

    if (s != 0) {
        free_arrays(sum_wk2_arr, phi_res, tau, tau2, 
                    phi_k, w, pwr, wt, wt2, wp, bad_pwr);
        return -1;
    }

    return 0;
}

/*
This function allocates and initializes a new least squares data structure*/
LS_DATA* new_least_squares_data(struct FitPrm *fitted_prms){
    SUMS *new_sums;
    LS_DATA *new_ls_data;
    int s;

    new_sums=malloc(sizeof(SUMS));

    new_sums->num_points = 0.0; 
    new_sums->w = 0.0;          
    new_sums->wk = 0.0;         
    new_sums->wk2 = 0.0;    
    new_sums->wk2_arr = NULL;   
    new_sums->wk4 = 0.0;        
    new_sums->p = 0.0;        
    new_sums->pk = 0.0;        
    new_sums->pk2 = 0.0;        
    new_sums->phi = 0.0;        
    new_sums->kphi = 0.0; 

    new_ls_data = malloc(sizeof(LS_DATA));

    new_ls_data->sums = new_sums;
    new_ls_data->t0 = 0.0;
    new_ls_data->t2 = 0.0;
    new_ls_data->t4 = 0.0;
    new_ls_data->phi_res = NULL;
    new_ls_data->tau = NULL;
    new_ls_data->tau2 = NULL;
    new_ls_data->phi_k = NULL;
    new_ls_data->w = NULL;
    new_ls_data->ln_pwr = NULL;
    new_ls_data->wt = NULL;
    new_ls_data->wt2 = NULL;
    new_ls_data->wp = NULL;
    new_ls_data->omega_loc = 0.0;
    new_ls_data->omega_err_loc = 0.0;
    new_ls_data->phi_loc = 0.0;
    new_ls_data->omega_base = 0.0;
    new_ls_data->omega_high = 0.0;
    new_ls_data->omega_low = 0.0;
    new_ls_data->phase_sdev = 0.0;
    new_ls_data->phi_err = 0.0;
    new_ls_data->omega_err = 0.0;
    new_ls_data->pwr_level =NULL;

    s = allocate_ls_arrays(fitted_prms,
                           &new_ls_data->sums->wk2_arr,
                           &new_ls_data->phi_res,
                           &new_ls_data->tau,
                           &new_ls_data->tau2, 
                           &new_ls_data->phi_k,
                           &new_ls_data->w, 
                           &new_ls_data->ln_pwr,
                           &new_ls_data->wt,
                           &new_ls_data->wt2,
                           &new_ls_data->wp,
                           &new_ls_data->pwr_level);

    return s > -1 ? new_ls_data : NULL;

}

/**
This function frees all data associated with a least squares 
data structure
*/
void free_ls_data(LS_DATA* ls_data){
    free_arrays(&ls_data->sums->wk2_arr,
                &ls_data->phi_res,
                &ls_data->tau,
                &ls_data->tau2, 
                &ls_data->phi_k,
                &ls_data->w, 
                &ls_data->ln_pwr, 
                &ls_data->wt, 
                &ls_data->wt2, 
                &ls_data->wp, 
                &ls_data->pwr_level);

    free(ls_data);

}


