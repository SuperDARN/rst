/* acf.h
   =====
   Author: R.J.Barnes
*/

/* 
   See license.txt
*/




#ifndef _ACF_H
#define _ACF_H


#define ACF_PART 0
#define XCF_PART 1









int ACFCalculate(struct TSGprm *prm,
 		 int16 *inbuf,int rngoff,int dflg,
		 int roffset,int ioffset,
		 int mplgs,int *lagtable[2],
  	         float *acfbuf,
	         int xcf,int xcfoff,
                 int badrange,float atten,float *dco);

int ACFAverage(float *pwr0,float *acfd,
	       float *xcfd,
               int nave,int nrang,int mplgs);

int ACFBadLagZero(struct TSGprm *prm,int mplgs,int *lagtable[2]);

void ACFNormalize(float *pwr0,float *acfd,float *xcfd,
                  int nrang,int mplgs,float atten);

int ACFSumPower(struct TSGprm *prm,int mplgs,
                int *lagtable[2],float *acfpwr0,
		int16 *inbuf,int rngoff,int dflg,
		int roffset,int ioffset,
		int badrng,float noise,float mxpwr,
		float atten,
	        int thr,int lmt,
                int *abort);

int ACFSumProduct(int16 *buffer,float *avepower,
		  int numsamples,float *mxpwr,float *dco);













#endif
