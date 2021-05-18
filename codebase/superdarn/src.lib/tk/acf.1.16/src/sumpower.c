/* sumpower.c
   ==========
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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "rtypes.h"
#include "tsg.h"




/* 
   dflg      turn sample delay on and off 
   rngoff    range scaling factor 2 or 4 depending on XCF.
   roffset   real offset into the A/D buffer
   ioffset   imaginary offset into the A/D buffer 
   badrng    bad range
   noise     noise value
   atten     attenuation level (0 = no attenuation)
*/

int ACFSumPower(struct TSGprm *prm,int mplgs,
                int *lagtable[2],float *acfpwr0,
		int16 *inbuf,int rngoff,int dflg,
		int roffset,int ioffset,
		int badrng,float noise,float mxpwr,
		float atten,
	        int thr,int lmt,
                int *abort) {

  int sdelay=0;
  int sampleunit;
  int range;
  unsigned inbufind;
  int maxrange;
  float *pwr0;
  float rpwr;
  float ipwr;

  float tmpminpwr,minpwr,maxpwr;  
  int slcrng=0;
  int lag0msample;
  int16 *inbufadr;
  int newlag0msample;
  float ltemp;

  int cnt=0;

  *abort=0;
  
  if (dflg) sdelay=prm->smdelay; /* digital receiver delay term */
  sampleunit = prm->mpinc / prm->smsep; 
  maxrange = prm->nrang;

  pwr0=malloc(sizeof(float)*maxrange);
  if (pwr0==NULL) return -1;

  lag0msample = lagtable[0][0] * sampleunit;
  minpwr = 1e16;
  newlag0msample = 0;

  for(range=0; range < maxrange; range++) {
    /* check to see if there is a need for changeing the lag */
    if((range >= badrng)  && (newlag0msample == 0)) {
       lag0msample = lagtable[0][mplgs]*sampleunit;
       newlag0msample = 1;
    }

    inbufind = (lag0msample + range + sdelay ) * rngoff;
    inbufadr = inbuf + inbufind + roffset; 
    ltemp = (float) *inbufadr;
    rpwr = ltemp * ltemp;

    inbufadr = inbuf + inbufind + ioffset;
    ltemp = (float) *inbufadr;
    ipwr = ltemp * ltemp;
   
    pwr0[range] = rpwr + ipwr; 

    if (minpwr > pwr0[range]) minpwr = pwr0[range];
    if (atten !=0) pwr0[range] = pwr0[range] / atten;
 
    if ((thr !=0) &&   
       (pwr0[range]<(thr*noise))) cnt++; 
 

  } 
 
  if ((lmt !=0) && (cnt<lmt)) {
    free(pwr0);
    *abort=1;
    return -1;
  }

  /* accumulate the power in rawdata which is the output buffer */

  maxpwr = 0L;
  slcrng = 0;
  for(range=0; range < maxrange;range++)  {
    acfpwr0[range] = acfpwr0[range] + pwr0[range];
    if(acfpwr0[range] > maxpwr) {
      maxpwr = acfpwr0[range];
      slcrng = range;
    }
  }



  if (mxpwr==0) {
    free(pwr0);
    return slcrng;
  }
  
  tmpminpwr=0; /* tmpminpwr=4*minpwr */

  if((pwr0[slcrng] < mxpwr)  ||
     (pwr0[slcrng] < tmpminpwr)) slcrng=-1;
  free(pwr0);
				   
  return slcrng;
}

