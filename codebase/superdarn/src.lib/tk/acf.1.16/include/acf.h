/* acf.h
   =====
   Author: R.J.Barnes
*/

/* 
  Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
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
