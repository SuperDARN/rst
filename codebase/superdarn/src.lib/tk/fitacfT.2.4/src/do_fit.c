/* do_fit.c
   ========
   Author: R.J.Barnes,K.Baker and D.Andre, P Ponomarenko
*/

/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.2-beta-4-g32f7302.txt for more information.
 
 
  
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

int do_fit(struct FitBlock *iptr,int lag_lim,int goose,
	   struct FitRange *ptr,struct FitRange *xptr,
           struct FitElv *elv,
	   struct FitNoise *nptr,int print) {

  struct FitACFBadSample badsmp;
  int *badlag=NULL;

  int i=0,k;

  double *pwrd=NULL,*pwrt=NULL;
  double mnpwr, skylog, freq_to_vel, range;
  double xomega=0.0;

  double noise_pwr=0.0; 
  int ni;

  nptr->skynoise=0.0;
  nptr->lag0=0.0;
  nptr->vel=0.0;


  if (iptr->prm.nave <= 1) return 0;

  freq_to_vel = C/(4*PI)/(iptr->prm.tfreq * 1000.0);

  badlag=malloc(sizeof(int)*iptr->prm.nrang*iptr->prm.mplgs);
  if (badlag==NULL) return -1;

  pwrd=malloc(sizeof(double)*iptr->prm.nrang);
  if (pwrd==NULL) {
    free(badlag);
    return -1;
  }
  pwrt=malloc(sizeof(double)*iptr->prm.nrang);
  if (pwrt==NULL) {
    free(badlag);
    free(pwrd);
    return -1;
  }

  if (iptr->prm.channel==0) FitACFBadlags(&iptr->prm,&badsmp);	
  else FitACFBadlagsStereo(&iptr->prm,&badsmp);


  /* Determine the lag_0 noise level (0 dB reference) and the noise level at 
   which fit_acf is to quit (average power in the 
   fluctuations of the acfs which are pure noise) */

  for (i=0; i < iptr->prm.nrang; i++) {
	  pwrd[i] = (double) iptr->prm.pwr0[i];	
          /* transfer powers into local array */
	  pwrt[i] = pwrd[i];
  }
  qsort(pwrt, iptr->prm.nrang, sizeof(double), dbl_cmp);
  /* determine the average lag0 power of the 10 lowest power acfs */
	
  ni = 0;
  i = 0;
  mnpwr = 0.0;
	
  /*  look for the lowest 10 values of lag0 power and average to 
      get the noise level.  Ignore values that are exactly 0.  If
      you can't find 10 useable values within the first 1/3 of the
      sorted power list, then just use whatever you got in that 
      first 1/3.  If you didn't get any useable values, then use
      the NOISE parameter */
		
  while ((ni < 10) && (i < iptr->prm.nrang/3)) {
    if (pwrt[i]) ++ni;
    mnpwr += pwrt[i++];  
  }

  ni = (ni > 0) ? ni :  1;
  mnpwr = mnpwr/ni;
  if (mnpwr < 1.0) mnpwr = iptr->prm.noise;
  nptr->skynoise = mnpwr;




  /* Now determine the level which will be used as the cut-off power 
     for fit_acf.  This is the average power at all non-zero lags of all
     acfs which have lag0 power < 1.6*mnpwr + 1 stnd. deviation from that
     average power level */

  noise_pwr = noise_stat(mnpwr,&iptr->prm,&badsmp,iptr->acfd); 

  /*	convert the lag0 powers to dB */

  if (nptr->skynoise > 0.0) skylog = 10.0 * log10(nptr->skynoise);
  else skylog = 0.0;

  for (i=0; i<iptr->prm.nrang; i++) {	

    pwrd[i] = pwrd[i] - nptr->skynoise;
    if (pwrd[i] <= 0.0) ptr[i].p_0 = -50.0;
    else ptr[i].p_0 = 10.0*log10(pwrd[i]) - skylog;
  }

  /*	reset the output arrays */

  for (i=0; i<iptr->prm.nrang; i++) {
	ptr[i].p_l = -50.0;
	ptr[i].p_s = -50.0;
	ptr[i].p_l_err= 0.0;
	ptr[i].p_s_err= 0.0;
	ptr[i].w_l = 0.0;
	ptr[i].w_s = 0.0;
	ptr[i].w_l_err = 0.0;
	ptr[i].w_s_err = 0.0;
	ptr[i].v = 0.0;
	ptr[i].v_err = 0.0;
	ptr[i].phi0 = 0.0;/*
	ptr[i].phi0_err=0.0;*/
	ptr[i].sdev_l = 0.0;
	ptr[i].sdev_s = 0.0;
	ptr[i].sdev_phi = 0.0;
	ptr[i].gsct = 0.0;
        ptr[i].qflg = 0;
	ptr[i].nump=0;
        if (xptr !=NULL) {
    	  xptr[i].p_l = -50.0;
	  xptr[i].p_s = -50.0;
	  xptr[i].p_l_err= 0.0;
	  xptr[i].p_s_err= 0.0;
	  xptr[i].w_l = 0.0;
	  xptr[i].w_s = 0.0;
	  xptr[i].w_l_err = 0.0;
	  xptr[i].w_s_err = 0.0;
	  xptr[i].v = 0.0;
	  xptr[i].v_err = 0.0;
	  xptr[i].phi0 = 0.0;
	  xptr[i].phi0_err=0.0;
	  xptr[i].sdev_l = 0.0;
	  xptr[i].sdev_s = 0.0;
	  xptr[i].sdev_phi = 0.0;
	  xptr[i].gsct = 0.0;
	  xptr[i].qflg = 0;
          xptr[i].nump=0;

	  elv[i].normal= 0.0;
	  elv[i].low = 0.0;
	  elv[i].high = 0.0;
	}

  }

  /* ----------------------------------------------------------------------*/
  /* 	Now do the fits for each acf */

	if(print && iptr->prm.channel < 2)
		fprintf(stdout,"%d  %d  %lf  %d  %lf  %d  %d  %d  %d  %lf\n",iptr->prm.nrang,
                iptr->prm.mplgs,nptr->skynoise,iptr->prm.tfreq,
                iptr->prm.mpinc*1.e-6,iptr->prm.lagfr,iptr->prm.smsep,
                iptr->prm.nave,iptr->prm.cp,iptr->prm.vdir);

  for (k=0, i=0; k<iptr->prm.nrang;k++)
	{

    ptr[k].qflg = fit_acf(&iptr->acfd[k*iptr->prm.mplgs], k+1,
                              &badlag[k*iptr->prm.mplgs],&badsmp,
                              lag_lim,&iptr->prm,noise_pwr,0,0.0,&ptr[k],print);/*

		fprintf(stderr,"ONE  qflg  %d  pwr  %lf\n",ptr[k].qflg,ptr[k].p_l);*/
    xomega=ptr[k].v;
		if(print && iptr->prm.channel < 2)
			fprintf(stdout,"%d\n",ptr[k].qflg);
    if (ptr[k].qflg == 1)	{
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

      ptr[k].p_l = ptr[k].p_l*LN_TO_LOG - skylog;
      ptr[k].p_s = ptr[k].p_s*LN_TO_LOG - skylog;

      ptr[k].p_l_err = (ptr[k].p_l_err == HUGE_VAL) ?
	                   HUGE_VAL :
	                   ptr[k].p_l_err*LN_TO_LOG;

      ptr[k].p_s_err = (ptr[k].p_s_err == HUGE_VAL) ?
	                   HUGE_VAL :
	                   ptr[k].p_s_err*LN_TO_LOG;

      /* convert Doppler frequency to velocity */

      ptr[k].v = iptr->prm.vdir*freq_to_vel*ptr[k].v;

      /* flag absurdly high velocities with qflg of 8 */

      if (ptr[k].v > (freq_to_vel* (PI* 1000.0* 1000.0)/ iptr->prm.mpinc))
         ptr[k].qflg= 8;

      ptr[k].v_err = (ptr[k].v_err == HUGE_VAL) ?
			  HUGE_VAL : 
			  freq_to_vel*ptr[k].v_err;

      /* convert decay parameters to spectral widths */

      ptr[k].w_l = freq_to_vel*2*ptr[k].w_l;
      ptr[k].w_l_err = (ptr[k].w_l_err == HUGE_VAL) ?
	                   HUGE_VAL :
	                   freq_to_vel*2*ptr[k].w_l_err;

      /* sigma is returned as sigma**2 so check the sign for validity
         if sigma**2 is negative take sqrt of the abs and transfer the sign */

      ptr[k].w_s = (ptr[k].w_s >= 0) ? 
                      sqrt(ptr[k].w_s) : -sqrt(-ptr[k].w_s);


      if ((ptr[k].w_s !=0.0) && (ptr[k].w_s_err != HUGE_VAL))
				ptr[k].w_s_err = 0.5*ptr[k].w_s_err/fabs(ptr[k].w_s);
      else ptr[k].w_s_err=HUGE_VAL;




      ptr[k].w_s = 3.33*freq_to_vel*ptr[k].w_s;
      ptr[k].w_s_err = (ptr[k].w_s_err == HUGE_VAL) ?
	                    HUGE_VAL :
	                    3.33*freq_to_vel*ptr[k].w_s_err;


			ptr[k].phi0_err = mnpwr;



      /*  Now check the values of power, velocity and width
          to see if this should be flagged as ground-scatter */

      if (ptr[k].gsct == 0) ptr[k].gsct=ground_scatter(&ptr[k]);

			if(print && iptr->prm.channel < 2)
				fprintf(stdout,"%lf  %lf  %lf  %lf\n",ptr[k].v,ptr[k].v_err,ptr[k].p_l,ptr[k].w_l);/*


			fprintf(stderr,"TWO  qflg  %d  pwr  %lf\n",ptr[k].qflg,ptr[k].p_l);*/
    }
	
    if ((iptr->prm.xcf==0) || (ptr[k].qflg !=1)) {
      if( (ptr[k].qflg == 1)) i++;
      continue;
    }

	
    xptr[k].qflg = fit_acf(&iptr->xcfd[k*iptr->prm.mplgs], k+1,
                                  &badlag[k*iptr->prm.mplgs],&badsmp,
                                  lag_lim,&iptr->prm,noise_pwr,1,xomega,
				  &xptr[k],0);
       
    if (xptr[k].qflg == 1) {
      xptr[k].p_l = xptr[k].p_l*LN_TO_LOG - skylog;
      xptr[k].p_s = xptr[k].p_s*LN_TO_LOG - skylog;
      xptr[k].p_l_err = (xptr[k].p_l_err == HUGE_VAL) ?
	                    HUGE_VAL :
	                    xptr[k].p_l_err*LN_TO_LOG;

      xptr[k].p_s_err = (xptr[k].p_s_err == HUGE_VAL) ?
	                    HUGE_VAL :
	                    xptr[k].p_s_err*LN_TO_LOG;

      /* convert Doppler frequency to velocity */

      xptr[k].v = iptr->prm.vdir*freq_to_vel*xptr[k].v;
      xptr[k].v_err = (xptr[k].v_err == HUGE_VAL) ?
	                  HUGE_VAL :
	                  freq_to_vel*xptr[k].v_err;

      /* convert decay parameters to spectral widths */

      xptr[k].w_l = freq_to_vel*2*xptr[k].w_l;
      xptr[k].w_l_err = (xptr[k].w_l_err == HUGE_VAL) ?
	                    HUGE_VAL :
	                    freq_to_vel*2*xptr[k].w_l_err;

      /* sigma is returned as sigma**2 so check the sign for validity  
	 if sigma**2 is negative take sqrt of the abs and transfer the sign */

      xptr[k].w_s = (xptr[k].w_s >= 0) ? 
                      sqrt(xptr[k].w_s) : -sqrt(-xptr[k].w_s);

      if ((xptr[k].w_s !=0.0) && (xptr[k].w_s_err != HUGE_VAL))
        xptr[k].w_s_err = 0.5*xptr[k].w_s_err/fabs(xptr[k].w_s);
      else xptr[k].w_s_err=HUGE_VAL;

      xptr[k].w_s = 3.33*freq_to_vel*xptr[k].w_s;
      xptr[k].w_s_err = (xptr[k].w_s_err == HUGE_VAL) ?
	                    HUGE_VAL :
	                    3.33*freq_to_vel*xptr[k].w_s_err;


		
      /* calculate the elevation angle */
	
      if (xptr[k].phi0 > PI)  xptr[k].phi0 = xptr[k].phi0 - 2*PI;
      if (xptr[k].phi0 < -PI) xptr[k].phi0 = xptr[k].phi0 + 2*PI;
      if (iptr->prm.phidiff != 0) 
        xptr[k].phi0 = xptr[k].phi0*iptr->prm.phidiff;
 
      /* changes which array is first */
		
      range = 0.15*(iptr->prm.lagfr + iptr->prm.smsep*(k-1));
      if (goose==0) {
        elv[k].normal = elevation(&iptr->prm,range, xptr[k].phi0);
	elv[k].low = 
        elevation(&iptr->prm,range, xptr[k].phi0+xptr[k].phi0_err);
	elv[k].high = 
        elevation(&iptr->prm,range,xptr[k].phi0-xptr[k].phi0_err);
      } else {
  	elv[k].normal = elev_goose(&iptr->prm,range, xptr[k].phi0);
	elv[k].low = 
        elev_goose(&iptr->prm,range, xptr[k].phi0+xptr[k].phi0_err);
        elv[k].high = 
        elev_goose(&iptr->prm,range, xptr[k].phi0-xptr[k].phi0_err);
      }
    }
    if( (ptr[k].qflg == 1)) i++;
  }

  free(badlag);
  free(pwrd);
  free(pwrt);
  return i;
}

