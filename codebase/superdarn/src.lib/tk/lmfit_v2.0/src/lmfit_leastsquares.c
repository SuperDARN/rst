/*Non-Linear Least squares fitting using Levenburg-Marquardt 
Algorithm implements in C (cmpfit)

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

#include "lmfit_leastsquares.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "mpfit.h"
#include "lmfit_structures.h"

#define ITMAX 100 
#define EPS 3.0e-7 
#define FPMIN 1.0e-30

struct vars_struct {
  double *x;
  double *y;
  double *ey;
  double lambda;
};


/**
Returns a pointer to a new FITDATA structure
*/
LMFITDATA *new_lmfit_data(){
    LMFITDATA *new_lmfit_data;

    new_lmfit_data = malloc(sizeof(*new_lmfit_data));
    new_lmfit_data->P = 0.0;
    new_lmfit_data->wid = 0.0;
    new_lmfit_data->vel = 0.0;
    new_lmfit_data->phi0 = 0.0;
    new_lmfit_data->sigma_2_P = 0.0;
    new_lmfit_data->sigma_2_wid = 0.0;
    new_lmfit_data->sigma_2_vel = 0.0;
    new_lmfit_data->sigma_2_phi0 = 0.0;
    new_lmfit_data->chi_2 = 0.0;

    return new_lmfit_data;
}


void free_lmfit_data(LMFITDATA *lmfit_data){
    if(lmfit_data != NULL){
        free(lmfit_data);
    }
}


/**
prints the contents of a LMFITDATA structure
*/
void print_lmfit_data(LMFITDATA *fit_data, FILE* fp){
    fprintf(fp,"P: %e\n",fit_data->P);
    fprintf(fp,"wid: %e\n",fit_data->wid);
    fprintf(fp,"vel: %e\n",fit_data->vel);
    fprintf(fp,"phi0: %e\n",fit_data->phi0);
    fprintf(fp,"sigma_2_P: %e\n",fit_data->sigma_2_P);
    fprintf(fp,"sigma_2_wid: %e\n",fit_data->sigma_2_wid);
    fprintf(fp,"sigma_2_vel: %e\n",fit_data->sigma_2_vel);
    fprintf(fp,"sigma_2_phi0: %e\n",fit_data->sigma_2_phi0);
    fprintf(fp,"chi_2: %f\n",fit_data->chi_2);
}


/*function to calculate residuals for MPFIT*/
int exp_acf_model(int m, int n, double *params, double *deviates, double **derivs, void *private)
{

    int i;
    double t,P,wid,vel,sigma,acf_data,lambda,exponential,cosine,sine;
    struct vars_struct *v = (struct vars_struct *) private;
    double *x, *y, *ey;

    /* Data arrays and necessary quantities for the model */
    x = v->x;              /* time */
    y = v->y;              /* real and imaginary components */
    ey = v->ey;            /* sqrt of variance in lag estimates */
    lambda = v->lambda;    /* radar Tx wavelength */

    /* Parameters we are fitting for */
    P = params[0];    /* lag0 power */
    wid = params[1];  /* spectral width */
    vel = params[2];  /* Doppler velocity */

    /* Iterate through lags to calculate the deviates */
    /* Half of the data array is real component, other half is imaginary component */
    for (i=0; i<m; i++)
    {

        t = x[i];
        acf_data = y[i];
        sigma = ey[i];

        /* separate out the factors in the model because these
        factors are common to the deviates and the derivatives */
        exponential = exp(-2.0*M_PI*wid*t/lambda);
        cosine = cos(4*M_PI*vel*t/lambda);
        sine = sin(4*M_PI*vel*t/lambda);
        if(i < m/2)
        {
            deviates[i] = (acf_data - P*exponential*cosine)/sigma; /* (data - f_r)/sigma */
            if (derivs) {
                derivs[0][i] = -1*exponential*cosine/sigma;  /* -d(f_r)/dP */
                derivs[1][i] = -1*(-2.0*M_PI*t/lambda) * P*exponential*cosine/sigma; /* -d(f_r)/dwid */
                derivs[2][i] = (4*M_PI*t/lambda) * P*exponential*sine/sigma; /* -d(f_r)/dvel */
            }
        } else {
            deviates[i] = (acf_data - P*exponential*sine)/sigma; /* (data - f_i)/sigma */
            if (derivs) {
                derivs[0][i] = -1*exponential*sine/sigma;  /* -d(f_i)/dP */
                derivs[1][i] = -1*(-2.0*M_PI*t/lambda) * P*exponential*sine/sigma; /* -d(f_i)/dwid */
                derivs[2][i] = -1*(4*M_PI*t/lambda) * P*exponential*cosine/sigma; /* -d(f_i)/dvel */
            }
        }
    }

    return 0;
}


/* Need to perform several LMFITs at various intial velocities due to the large number
of local minima in the velocity axis of the chi2 space. N=30 initial velocities works. */
void lmfit_acf(LMFITDATA *fit_data,llist data, double lambda, int mpinc, int confidence, int model){
    /* confidence - the significance of confidence interval to be constructed
                    example: confidence == 1 corresponds to 1-sigma confidence interval */
    /* model - selects either exponential envelope (0) or gaussian envelope (1) */

    const static int num_init_vel = 30;  /* Number of initial velocities to try */
    int i;
    double min_chi;
    double pwr_fit=0, wid_fit=0, vel_fit=0;
    double pwr_fit_e=0, wid_fit_e=0, vel_fit_e=0;
    int num_lags;
    ACFNODE* data_node;

    /*variable needed for mpfit levenburg-markardt algorithm call*/
    mp_par    params_info[3];
    mp_result result;
    mp_config config;
    int status;
    double best_fit_params[3];
    double paramerrors[3];

    double nyquist_velocity = lambda/(4.0 * (double)(mpinc) * 1.e-6);
    double v_step = (nyquist_velocity/2.0 - (-nyquist_velocity/2.0)-1.0) / ((double)(num_init_vel) - 1);  

    double chi2s[num_init_vel];
    double pows[num_init_vel];
    double wids[num_init_vel];
    double vels[num_init_vel];
    double pows_e[num_init_vel];
    double wids_e[num_init_vel];
    double vels_e[num_init_vel];

    int delta_chi = confidence*confidence;

    /* set up data array */
    num_lags = llist_size(data);

    /*structure needed for mpfit*/
    struct vars_struct * lmdata = malloc(sizeof(struct vars_struct));
    lmdata->x = malloc(num_lags*2*sizeof(double));
    lmdata->y = malloc(2*num_lags*sizeof(double));
    lmdata->ey = malloc(num_lags*2*sizeof(double));
    lmdata->lambda = lambda;

    /* Re-structure the data for mpfit to use */
    llist_reset_iter(data);
    i = 0;
    do
    {
        llist_get_iter(data,(void**)&data_node);
        lmdata->x[i] = data_node->t;
        lmdata->x[i+num_lags] = data_node->t;
        lmdata->y[i] = data_node->re;
        lmdata->y[i+num_lags] = data_node->im;
        lmdata->ey[i] = data_node->sigma_re;
        lmdata->ey[i+num_lags] = data_node->sigma_im;
        
        i++;
    }while(llist_go_next(data) != LLIST_END_OF_LIST);

    memset(&params_info[0], 0, sizeof(params_info));
    memset(&config, 0, sizeof(config));
    memset(&result, 0, sizeof(result));
    result.xerror = paramerrors;

    /*limit values to prevent fit from going to +- inf and breaking*/
    params_info[0].limited[0] = 1;
    params_info[0].limits[0]  = 0;
    params_info[0].side = 3;
    params_info[0].deriv_debug = 0;
      
    params_info[1].limited[0] = 1;
    params_info[1].limits[0]  = -100.0;
    params_info[1].limited[1] = 0;
    params_info[1].side = 3;
    params_info[1].deriv_debug = 0;

    params_info[2].limited[0] = 1;
    params_info[2].limits[0]  = -nyquist_velocity/2.;
    params_info[2].limited[1] = 1;
    params_info[2].limits[1]  = nyquist_velocity/2.;
    params_info[2].side = 3;
    params_info[2].deriv_debug = 0;

    /* CONFIGURE LMFIT */
    config.maxiter = 200;
    config.maxfev = 200;
    config.ftol = .0001;
    config.gtol = .0001;
    config.nofinitecheck=0;

    min_chi = 10e200;
    int save_status;
    for (i=0;i<num_init_vel;i++){
        /* Starting guess */
        best_fit_params[0] = 10000.0;
        best_fit_params[1] = 200.0;
        best_fit_params[2] = -nyquist_velocity/2. + i*v_step;

        /*run a single-component fit*/
        status = mpfit(exp_acf_model,num_lags*2,3,best_fit_params,params_info,&config,(void *)lmdata,&result);

        if (status == 5){
            fprintf(stderr,"Maximum Iterations Reached: %d\n",status);
        }else if ((status > 5)){
            fprintf(stderr, "Tolerance Problem: %d\n",status);
        }else if ((status < 1)){
            fprintf(stderr, "Other Error: %d\n",status);
        }

        chi2s[i] = result.bestnorm;
        pows[i] = best_fit_params[0];
        wids[i] = best_fit_params[1];
        vels[i] = best_fit_params[2];
        pows_e[i] = result.xerror[0];
        wids_e[i] = result.xerror[1];
        vels_e[i] = result.xerror[2];
        /* TO DO */
        /* Check status that mpfit returns to see if fit is worth keeping */
        /* Look at mpfit.h for status codes, basically if 0 < status <= 4 is good */

        if (result.bestnorm < min_chi){
            min_chi = result.bestnorm;

            pwr_fit = best_fit_params[0];
            wid_fit = best_fit_params[1];
            vel_fit = best_fit_params[2];

            pwr_fit_e = confidence*result.xerror[0];
            wid_fit_e = confidence*result.xerror[1];
            vel_fit_e = confidence*result.xerror[2];
            save_status = status;
        }
    }

    /* Check for deviation from Gaussian of fitted parameter errors */
    /* Local minima may be within delta_chi significance of global minimum */
    for (i=0;i<num_init_vel;i++){
        if (chi2s[i] <= min_chi + delta_chi){
            /* Make sure we keep the largest error bar. There may be multiple
               local minima below min_chi + delta_chi */
            if (pwr_fit_e < fabs(pwr_fit - pows[i])){
                pwr_fit_e = fabs(pwr_fit - pows[i]);
            }
            if (wid_fit_e < fabs(wid_fit - wids[i])){
                wid_fit_e = fabs(wid_fit - wids[i]);
            }
            if (vel_fit_e < fabs(vel_fit - vels[i])){
                vel_fit_e = fabs(vel_fit - vels[i]);
            }
        }
    }

    /* Save best fit parameters to output structure */
    fit_data->P = pwr_fit;
    fit_data->wid = wid_fit;
    fit_data->vel = vel_fit;
    fit_data->sigma_2_P = pwr_fit_e*pwr_fit_e;
    fit_data->sigma_2_wid = wid_fit_e*wid_fit_e;
    fit_data->sigma_2_vel = vel_fit_e*vel_fit_e;
    fit_data->chi_2 = min_chi;

    /* Free the memory */
    free(lmdata->x);
    free(lmdata->y);
    free(lmdata->ey);
    free(lmdata);
}
