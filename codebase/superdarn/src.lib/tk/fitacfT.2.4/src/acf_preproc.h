/* acf_preproc.h
   =============
   Author: R.J.Barnes & K.Baker
*/


/*
 (c) 2010 JHU/APL & Others - Please Consult LICENSE.superdarn-rst.3.2-beta-4-g32f7302.txt for more information.
 
 
 
*/






/*	This header file defines the values which an ACF preprocessor should
	return to fitacf */

#define ACF_UNMODIFIED	0
#define ACF_ION_SCAT	1
#define ACF_GROUND_SCAT 2
#define ACF_MIXED_SCAT	3

int acf_preproc(struct complex *acf,
                struct complex *orig_acf,
                double *noise_lev,
                int range,int *badlag,
                int mplgs);

