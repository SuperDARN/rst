/* do_fit.c
     ========
     Author: R.J.Barnes,K.Baker and D.Andre, P Ponomarenko
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
#include <stdio.h>
#include <stdlib.h>
#include "rmath.h"
#include "badsmp.h"
#include "fitblk.h"
#include "dbl_cmp.h"
#include "badlags.h"
#include "fit_acf.h"
#include "noise_stat.h"
#include "elevation.h"
#include "elev_goose.h"
#include "ground_scatter.h"

/**
Determines the lag 0 noise level and ACF cut off power
*/
double calc_skynoise(struct FitBlock *fit_blk, double *pwrd, size_t size){

    int i;
    double min_pwr = 0.0;
    double *pwrt;

    pwrt=malloc(size);

    if(pwrt == NULL){
        return -1.0;
    }

    /* Determine the lag_0 noise level (0 dB reference) and the noise level at 
     which fit_acf is to quit (average power in the 
     fluctuations of the acfs which are pure noise) */

    for (i=0; i < fit_blk->prm.nrang; i++) {
        pwrd[i] = (double) fit_blk->prm.pwr0[i];   
                    /* transfer powers into local array */
        pwrt[i] = pwrd[i];
    }

    qsort(pwrt, fit_blk->prm.nrang, sizeof(double), dbl_cmp);
    /* determine the average lag0 power of the 10 lowest power acfs */
    
    int ni = 0;
    i=0;

    
    /*  look for the lowest 10 values of lag0 power and average to 
            get the noise level.  Ignore values that are exactly 0.  If
            you can't find 10 useable values within the first 1/3 of the
            sorted power list, then just use whatever you got in that 
            first 1/3.  If you didn't get any useable values, then use
            the NOISE parameter */
        
    while ((ni < 10) && (i < fit_blk->prm.nrang/3)) {
        if (pwrt[i]) ++ni;
        min_pwr += pwrt[i++];  
    }

    ni = (ni > 0) ? ni :  1;
    min_pwr = min_pwr/ni;
    if (min_pwr < 1.0) min_pwr = fit_blk->prm.noise;

    free(pwrt);
    return min_pwr;
}

/*
    compares lag 0 power with the noise level and if it's below
    the noise then it is given -50dB magnitude, otherwise 
    it is recalculated into SNR by subtraction
*/ 
void power_to_snr(struct FitBlock *fit_blk, struct FitRange *acf_fit_range, struct FitNoise *fit_noise, 
                    double *skylog, double *pwrd){

    int i;
    /*  convert the lag0 powers to dB */
    if (fit_noise->skynoise > 0.0) *skylog = 10.0 * log10(fit_noise->skynoise);
    else *skylog = 0.0;

    for (i=0; i<fit_blk->prm.nrang; i++) { 

        pwrd[i] = pwrd[i] - fit_noise->skynoise;
        if (pwrd[i] <= 0.0) acf_fit_range[i].p_0 = -50.0;
        else acf_fit_range[i].p_0 = 10.0*log10(pwrd[i]) - *skylog;
    }

}

void init_acf_range_data(struct FitRange *acf_fit_range, int nrang){

    int i;

    if(acf_fit_range != NULL)
        for (i=0; i<nrang; i++) {
            acf_fit_range[i].p_l = -50.0;
            acf_fit_range[i].p_s = -50.0;
            acf_fit_range[i].p_l_err= 0.0;
            acf_fit_range[i].p_s_err= 0.0;
            acf_fit_range[i].w_l = 0.0;
            acf_fit_range[i].w_s = 0.0;
            acf_fit_range[i].w_l_err = 0.0;
            acf_fit_range[i].w_s_err = 0.0;
            acf_fit_range[i].v = 0.0;
            acf_fit_range[i].v_err = 0.0;
            acf_fit_range[i].phi0 = 0.0;
            acf_fit_range[i].phi0_err=0.0;
            acf_fit_range[i].sdev_l = 0.0;
            acf_fit_range[i].sdev_s = 0.0;
            acf_fit_range[i].sdev_phi = 0.0;
            acf_fit_range[i].gsct = 0.0;
            acf_fit_range[i].qflg = 0;
            acf_fit_range[i].nump=0;
        }
}

void init_xcf_range_data(struct FitRange *xcf_fit_range, 
                        struct FitElv *elv, int nrang){

    int i;

    if (xcf_fit_range != NULL) {
        for (i=0; i<nrang; i++) {
            xcf_fit_range[i].p_l = -50.0;
            xcf_fit_range[i].p_s = -50.0;
            xcf_fit_range[i].p_l_err= 0.0;
            xcf_fit_range[i].p_s_err= 0.0;
            xcf_fit_range[i].w_l = 0.0;
            xcf_fit_range[i].w_s = 0.0;
            xcf_fit_range[i].w_l_err = 0.0;
            xcf_fit_range[i].w_s_err = 0.0;
            xcf_fit_range[i].v = 0.0;
            xcf_fit_range[i].v_err = 0.0;
            xcf_fit_range[i].phi0 = 0.0;
            xcf_fit_range[i].phi0_err=0.0;
            xcf_fit_range[i].sdev_l = 0.0;
            xcf_fit_range[i].sdev_s = 0.0;
            xcf_fit_range[i].sdev_phi = 0.0;
            xcf_fit_range[i].gsct = 0.0;
            xcf_fit_range[i].qflg = 0;
            xcf_fit_range[i].nump=0;

            elv[i].normal= 0.0;
            elv[i].low = 0.0;
            elv[i].high = 0.0;
        }
    }
}

/**
Once an ACF has been fitted, this function will determine parameters such as
velocity, spectral width, etc from the fitted data for a given range
*/
void determinations_from_fitted_acf(struct FitBlock *fit_blk, struct FitRange *acf_fit_range,
    double skylog, double freq_to_vel, int range){

    /* several changes have been made here to 
     fix an apparent problem in handling HUGE_VAL.
             
     If there are too few points in an ACF to allow
     the error on a parameter to be calculated then
                 the subroutine fit_acf sets the value to HUGE_VAL.

     However, in this routine the error values are converted
     to natural units (e.g. velocity instead of frequency).
     It appears that multiplying HUGE_VAL by something causes
     a floating point exception that then sets the result of
     the calculation to 0.  Thus the error values that were being
     stored in the file would be zero instead of HUGE_VAL.

     The code now checks to see if the value is set to
     HUGE_VAL before doing the conversion.  If it is then
     instead of a converted version the error value is
     reset to HUGE_VAL.
    */

    /* convert power from natural log to dB */

    acf_fit_range[range].p_l = acf_fit_range[range].p_l*LN_TO_LOG - skylog;
    acf_fit_range[range].p_s = acf_fit_range[range].p_s*LN_TO_LOG - skylog;

    acf_fit_range[range].p_l_err = (acf_fit_range[range].p_l_err == HUGE_VAL) ? 
        HUGE_VAL : acf_fit_range[range].p_l_err*LN_TO_LOG;

    acf_fit_range[range].p_s_err = (acf_fit_range[range].p_s_err == HUGE_VAL) ? 
        HUGE_VAL : acf_fit_range[range].p_s_err*LN_TO_LOG;

    /* convert Doppler frequency to velocity */

    acf_fit_range[range].v = fit_blk->prm.vdir*freq_to_vel*acf_fit_range[range].v;

    /* flag absurdly high velocities with qflg of 8 */

    if (acf_fit_range[range].v > (freq_to_vel* (PI* 1000.0* 1000.0)/ fit_blk->prm.mpinc)){
       acf_fit_range[range].qflg= 8;      
    }

    acf_fit_range[range].v_err = (acf_fit_range[range].v_err == HUGE_VAL) ?
        HUGE_VAL : freq_to_vel*acf_fit_range[range].v_err;

    /* convert decay parameters to spectral widths */

    acf_fit_range[range].w_l = freq_to_vel*2*acf_fit_range[range].w_l;
    acf_fit_range[range].w_l_err = (acf_fit_range[range].w_l_err == HUGE_VAL) ?
        HUGE_VAL : freq_to_vel*2*acf_fit_range[range].w_l_err;

    /* sigma is returned as sigma**2 so check the sign for validity
         if sigma**2 is negative take sqrt of the abs and transfer the sign */

    acf_fit_range[range].w_s = (acf_fit_range[range].w_s >= 0) ? sqrt(acf_fit_range[range].w_s) : -sqrt(-acf_fit_range[range].w_s);


    if ((acf_fit_range[range].w_s !=0.0) && (acf_fit_range[range].w_s_err != HUGE_VAL)){  
        acf_fit_range[range].w_s_err = 0.5*acf_fit_range[range].w_s_err/fabs(acf_fit_range[range].w_s);
    }
    else{ 
        acf_fit_range[range].w_s_err=HUGE_VAL;
    }

    acf_fit_range[range].w_s = 3.33*freq_to_vel*acf_fit_range[range].w_s;
    acf_fit_range[range].w_s_err = (acf_fit_range[range].w_s_err == HUGE_VAL) ?
        HUGE_VAL : 3.33*freq_to_vel*acf_fit_range[range].w_s_err;


    /*  Now check the values of power, velocity and width
            to see if this should be flagged as ground-scatter */

    if (acf_fit_range[range].gsct == 0) acf_fit_range[range].gsct=ground_scatter(&acf_fit_range[range]); 

}

/**
Once an XCF has been fitted, this function will determine parameters such as
velocity, spectral width, etc as well as elevation angle
*/
void determinations_from_fitted_xcf(struct FitBlock *fit_blk, struct FitRange *xcf_fit_range,
    struct FitElv *elv, int goose, double skylog, double freq_to_vel, int range){

    double range_gate;
        /* convert power from natural log to dB */

    xcf_fit_range[range].p_l = xcf_fit_range[range].p_l*LN_TO_LOG - skylog;
    xcf_fit_range[range].p_s = xcf_fit_range[range].p_s*LN_TO_LOG - skylog;
    xcf_fit_range[range].p_l_err = (xcf_fit_range[range].p_l_err == HUGE_VAL) ?
        HUGE_VAL : xcf_fit_range[range].p_l_err*LN_TO_LOG;

    xcf_fit_range[range].p_s_err = (xcf_fit_range[range].p_s_err == HUGE_VAL) ?
        HUGE_VAL : xcf_fit_range[range].p_s_err*LN_TO_LOG;

            /* convert Doppler frequency to velocity */

    xcf_fit_range[range].v = fit_blk->prm.vdir*freq_to_vel*xcf_fit_range[range].v;
    xcf_fit_range[range].v_err = (xcf_fit_range[range].v_err == HUGE_VAL) ?
        HUGE_VAL : freq_to_vel*xcf_fit_range[range].v_err;

            /* convert decay parameters to spectral widths */

    xcf_fit_range[range].w_l = freq_to_vel*2*xcf_fit_range[range].w_l;
    xcf_fit_range[range].w_l_err = (xcf_fit_range[range].w_l_err == HUGE_VAL) ?
        HUGE_VAL : freq_to_vel*2*xcf_fit_range[range].w_l_err;

            /* sigma is returned as sigma**2 so check the sign for validity  
            if sigma**2 is negative take sqrt of the abs and transfer the sign */

    xcf_fit_range[range].w_s = (xcf_fit_range[range].w_s >= 0) ? sqrt(xcf_fit_range[range].w_s) : -sqrt(-xcf_fit_range[range].w_s);

    if ((xcf_fit_range[range].w_s !=0.0) && (xcf_fit_range[range].w_s_err != HUGE_VAL)){
        xcf_fit_range[range].w_s_err = 0.5*xcf_fit_range[range].w_s_err/fabs(xcf_fit_range[range].w_s);
    }
    else{ 
        xcf_fit_range[range].w_s_err=HUGE_VAL;
    }

    xcf_fit_range[range].w_s = 3.33*freq_to_vel*xcf_fit_range[range].w_s;
    xcf_fit_range[range].w_s_err = (xcf_fit_range[range].w_s_err == HUGE_VAL) ? 
        HUGE_VAL : 3.33*freq_to_vel*xcf_fit_range[range].w_s_err;



            /* calculate the elevation angle */
    
    if (xcf_fit_range[range].phi0 > PI)  xcf_fit_range[range].phi0 = xcf_fit_range[range].phi0 - 2*PI;
    if (xcf_fit_range[range].phi0 < -PI) xcf_fit_range[range].phi0 = xcf_fit_range[range].phi0 + 2*PI;
    if (fit_blk->prm.phidiff != 0) 
        xcf_fit_range[range].phi0 = xcf_fit_range[range].phi0*fit_blk->prm.phidiff;

            /* changes which array is first */

    range_gate = 0.15*(fit_blk->prm.lagfr + fit_blk->prm.smsep*(range-1));
    if (goose == 0) {
        elv[range].normal = elevation(&fit_blk->prm,range_gate, xcf_fit_range[range].phi0);
        elv[range].low = elevation(&fit_blk->prm,range_gate, xcf_fit_range[range].phi0+xcf_fit_range[range].phi0_err);
        elv[range].high = elevation(&fit_blk->prm,range_gate,xcf_fit_range[range].phi0-xcf_fit_range[range].phi0_err);
    } else {
        elv[range].normal = elev_goose(&fit_blk->prm,range_gate, xcf_fit_range[range].phi0);
        elv[range].low = elev_goose(&fit_blk->prm,range_gate, xcf_fit_range[range].phi0+xcf_fit_range[range].phi0_err);
        elv[range].high = elev_goose(&fit_blk->prm,range_gate, xcf_fit_range[range].phi0-xcf_fit_range[range].phi0_err);
    }
  
}

/**
Do_fit finds intialize noise levels and power cut offs for ACFs. It then marks
bad samples. Each range gate then has its ACF fitted and parameters are then
determined from fitted data
*/
int do_fit(struct FitBlock *fit_blk,int lag_lim,int goose,
         struct FitRange *acf_fit_range,struct FitRange *xcf_fit_range, struct FitElv *elv,
         struct FitNoise *fit_noise) {

    struct FitACFBadSample samples;
    int *lag=NULL;

    int i=0, k, s;

    double *pwrd=NULL,*pwrt=NULL;
    double min_pwr, skylog, freq_to_vel, range;
    double xomega=0.0;

    double noise_pwr=0.0; 

    fit_noise->skynoise=0.0;
    fit_noise->lag0=0.0;
    fit_noise->vel=0.0;


    if (fit_blk->prm.nave <= 1) return 0;

    lag=malloc(sizeof(int)*fit_blk->prm.nrang*fit_blk->prm.mplgs);
    if (lag==NULL){
        return -1;
    }

    pwrd=malloc(sizeof(double)*fit_blk->prm.nrang);
    if (pwrd==NULL) {
        free(lag);
        return -1;
    }

    if (fit_blk->prm.channel==0) FitACFMarkBadSamples(&fit_blk->prm,&samples);    
    else FitACFBadlagsStereo(&fit_blk->prm,&samples);  

    min_pwr = calc_skynoise(fit_blk, pwrd, sizeof(double)*fit_blk->prm.nrang);
    if (min_pwr < 0.0){
        free(lag);
        free(pwrd);
        return -1;
    }
    fit_noise->skynoise = min_pwr;
    /* Now determine the level which will be used as the cut-off power 
         for fit_acf.  This is the average power at all non-zero lags of all
         acfs which have lag0 power < 1.6*min_pwr + 1 stnd. deviation from that
         average power level */

    noise_pwr = noise_stat(min_pwr,&fit_blk->prm,&samples,fit_blk->acfd); 

    /*convert lag0powers to snr AND assign -50dB to those with SNR below 1*/
    power_to_snr(fit_blk, acf_fit_range, fit_noise, &skylog, pwrd);

    /*  reset the output arrays */

    init_acf_range_data(acf_fit_range, fit_blk->prm.nrang);

    init_xcf_range_data(xcf_fit_range, elv,fit_blk->prm.nrang);

    /* ----------------------------------------------------------------------*/


    freq_to_vel = C/(4*PI)/(fit_blk->prm.tfreq * 1000.0);

    /*  Now do the fits for each acf */

    for (k=0, i=0; k<fit_blk->prm.nrang;k++) {

        acf_fit_range[k].qflg = fit_acf(&fit_blk->acfd[k*fit_blk->prm.mplgs], k+1,
                                &lag[k*fit_blk->prm.mplgs],&samples,
                                lag_lim,&fit_blk->prm,noise_pwr,0,0.0,&acf_fit_range[k]);

        xomega=acf_fit_range[k].v;
        if(acf_fit_range[k].qflg == 1){
            determinations_from_fitted_acf(fit_blk, acf_fit_range, skylog, freq_to_vel, k);
            i++;
        }
        
    
        if ((fit_blk->prm.xcf==0) || (acf_fit_range[k].qflg !=1)) {
            continue;
        }

        
        xcf_fit_range[k].qflg = fit_acf(&fit_blk->xcfd[k*fit_blk->prm.mplgs], k+1,
                                &lag[k*fit_blk->prm.mplgs],&samples,
                                lag_lim,&fit_blk->prm,noise_pwr,1,xomega,
                                &xcf_fit_range[k]);

        if(xcf_fit_range[k].qflg == 1){
            determinations_from_fitted_xcf(fit_blk, xcf_fit_range, elv, goose, skylog, freq_to_vel, k);
        }
             
    }

    free(lag);
    free(pwrd);
    free(pwrt);
    return i;
}

