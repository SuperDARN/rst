/* acfcalculate.c
   ==============
   Author: R.J.Barnes, R.A.Greenwald, K.Oksavik
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
#include <math.h>
#include "rtypes.h"
#include "tsg.h"
#include "acfex.h"

int ACFexPwrSort(const void *a,const void *b) {
  float *x,*y;
  x=(float *) a;
  y=(float *) b;
  if (*x>*y) return 1;
  if (*x<*y) return -1;
  return 0;
}
 

int ACFexCalculate(struct TSGprm *prm,int16 *iqptr,int iqnum,
		 int nave,int nsample,
		 int roffset,int ioffset,
         int mplgs,int lagnum,int *lagtable[2],int *lagsum,
         float *pwr0,float *acfbuf,float *noise) {


   float *smpwr=NULL;
   float *srtpwr=NULL;
   float *acfr=NULL;
   float *acfi=NULL;

   float minpwrlvl = 2.0;
   float p0,p1,p2,v1,v2;
   float sumv1, sumv2, v1_off, v2_off;
   float sumnzlagpwr,lagpwr;
   int R,L,i0,rxlag,n,i,x,lag;
   int i1min,i2min,i1,i2,lastlag;
   int srtn=0, lagcnt=0;
   long ncnt=0;

/* Storage allocation for acfr and acfi allows up to 10 pair 
   combinations per lag. */

   acfr=malloc(sizeof(float)*nave*10);
   acfi=malloc(sizeof(float)*nave*10);
   smpwr=malloc(sizeof(float)*nsample);
   srtpwr=malloc(sizeof(float)*prm->nrang);

/* The following code determines the median power at each of the *nsample
   complex samples of the data arrays from the digital rx over the integration
   interval. */

   for (n=0;n<nsample;n++) {
      for (i=0;i<nave;i++) {
         x  = i*nsample+n;
         v1 = iqptr[2*x];
         v2 = iqptr[2*x+1];
         acfr[i] = v1*v1 + v2*v2;
       }
      qsort(acfr,nave,sizeof(float),ACFexPwrSort);
      smpwr[n]=acfr[nave/2];
      }

/* Put -1's into all array elements prior to the first sample and into
   those array elements for which a Tx pulse is occurring. Assume bad
   data during the Tx pulse and during the following two samples. */

     rxlag = prm->smdelay - 1;
     for (n=0;n<rxlag;n++) smpwr[n] = -1.0;
     for (n=1;n<prm->mppul;n++) {
       smpwr[rxlag - prm->frang/prm->rsep + 
                    prm->pat[n]*prm->mpinc/prm->smsep + 0] = -1.0;
       smpwr[rxlag - prm->frang/prm->rsep + 
                    prm->pat[n]*prm->mpinc/prm->smsep + 1] = -1.0;
       smpwr[rxlag - prm->frang/prm->rsep + 
                    prm->pat[n]*prm->mpinc/prm->smsep + 2] = -1.0;
     }

/* Determine the power profile of the backscattered signal for all R ranges.
   If a transmitter pulse is encountered before the last range is reached, 
   use an alternative transmitter pulse for the final ranges. */
     
     for (R=0;R<prm->nrang;R++) {
       i0 = rxlag + lagtable[0][0]*prm->mpinc/prm->smsep + R; 
       if (((lagtable[0][lagnum]-lagtable[1][lagnum])==0) && 
          (i0>=(rxlag - prm->frang/prm->rsep + 
                       prm->pat[1]*prm->mpinc/prm->smsep + R))) {
              i0 = rxlag + lagtable[0][lagnum]*prm->mpinc/prm->smsep + R;
              }
       pwr0[R] = smpwr[i0];
       if (pwr0[R]>0.0) {
         srtpwr[srtn] = pwr0[R];
         srtn++;
       }
     }

/* Reorder the non -1.0 power values in terms of increasing backscatter power.
   Then average values 10-19 to get a noise power determination. */

    qsort(srtpwr,srtn,sizeof(float),ACFexPwrSort);
    *noise = 0.0;
    for (srtn=10;srtn<20;srtn++) *noise += srtpwr[srtn];
    *noise = *noise/10.0;

/* Remove residual DC offset from iq sample arrays.  This process only uses
   data with power levels less than 3*noise for offset determination.  */

     ncnt = 0;
     sumv1 = 0;
     sumv2 = 0;
     for (n=0;n<nsample;n++) {
        for (i=0;i<nave;i++) {
            x  = i*nsample+n;
            v1 = iqptr[2*x];
            v2 = iqptr[2*x+1];
		    if ((v1*v1+v2*v2)<3.0*(*noise)) {
		       sumv1 += v1;
	    	   sumv2 += v2;
	   	       ncnt++;
	 		   }
            }
        }
     v1_off = sumv1/ncnt;
     v2_off = sumv2/ncnt;
     if (v1_off >= 0) (v1_off += 0.5); else (v1_off -= 0.5);
     if (v2_off >= 0) (v2_off += 0.5); else (v2_off -= 0.5);
     for (n=0;n<nsample;n++) {
          for (i=0;i<nave;i++) {
             x  = i*nsample+n;
             iqptr[2*x]   -= v1_off;
             iqptr[2*x+1] -= v2_off;
             }
          }      

/*  Restart routine with DC offset removed */

/* The following code determines the median power at each of the *nsample
   complex samples of the data arrays from the digital rx over the integration
   interval. */

     for (n=0;n<nsample;n++) {
        for (i=0;i<nave;i++) {
           x  = i*nsample+n;
           v1 = iqptr[2*x];
           v2 = iqptr[2*x+1];
           acfr[i] = v1*v1 + v2*v2;
           }
     qsort(acfr,nave,sizeof(float),ACFexPwrSort);
     smpwr[n]=acfr[nave/2];
     } 

/* Put -1's into all array elements prior to the first sample and into
   those array elements for which a Tx pulse is occurring. Assume bad
   data during the Tx pulse and during the following two samples. */

     rxlag = prm->smdelay - 1;
     for (n=0;n<rxlag;n++) smpwr[n] = -1.0;
     for (n=1;n<prm->mppul;n++) {
        smpwr[rxlag - prm->frang/prm->rsep + 
                    prm->pat[n]*prm->mpinc/prm->smsep + 0] = -1.0;
        smpwr[rxlag - prm->frang/prm->rsep + 
                    prm->pat[n]*prm->mpinc/prm->smsep + 1] = -1.0;
        smpwr[rxlag - prm->frang/prm->rsep + 
                    prm->pat[n]*prm->mpinc/prm->smsep + 2] = -1.0;
        }  

/* Determine the power profile of the backscattered signal for all R ranges.
   If a transmitter pulse is encountered before the last range is reached, 
   use an alternative transmitter pulse for the final ranges. */

     srtn=0;
	 for (R=0;R<prm->nrang;R++) {
        i0 = rxlag + lagtable[0][0]*prm->mpinc/prm->smsep + R; 
        if (((lagtable[0][lagnum]-lagtable[1][lagnum])==0) && 
           (i0>=(rxlag - prm->frang/prm->rsep + 
                       prm->pat[1]*prm->mpinc/prm->smsep + R))) {
              i0 = rxlag + lagtable[0][lagnum]*prm->mpinc/prm->smsep + R;
              }
        pwr0[R] = smpwr[i0];
        if (pwr0[R]>0.0) {
           srtpwr[srtn] = pwr0[R];
           srtn++;
           }  
        } 
/* Reorder the non -1.0 power values in terms of increasing backscatter power.
   Then average values 10-19 to get a noise power determination. */

      qsort(srtpwr,srtn,sizeof(float),ACFexPwrSort);
      *noise = 0.0;
      for (srtn=10;srtn<20;srtn++) *noise += srtpwr[srtn];
      *noise = *noise/10.0;  



 
/* Calculate the autocorrelation function for each range for which the    
   lag0[R] power is greater than 10*10^(minpwrlvl/10)*noise. 
   The ACFs are stored sequentially as 
   series of 2*mplgs elements (real, imag, real, imag....).
   If acfbufreal = acfbufimag = 0.0, there is no value.  */

     lastlag = 0;
     for (n=0;n<lagnum;n++) {
        if (abs(lagtable[0][n]-lagtable[1][n])>lastlag) {
           lastlag = abs(lagtable[0][n]-lagtable[1][n]); 
           }
        }
     for (R=0;R<prm->nrang;R++) {
        p0 = pwr0[R];
        if (p0>(pow(10,minpwrlvl/10.0)*(*noise))) {
           p0 -= *noise;
           acfbuf[R*2*mplgs]   = p0;
           acfbuf[R*2*mplgs+1] = 0.0;
           sumnzlagpwr = 0.0;
           lagcnt = 0;
           L = 1;

           for (lag=1;lag<=lastlag;lag++) {
             
              if (lagsum[lag]==0) {
		acfbuf[R*2*mplgs+2*L]   = 0;
		acfbuf[R*2*mplgs+2*L+1] = 0;
		L++;}

		else {
		   lagpwr = 0.0;
		   i1min  = -1;
		   i2min  = -1;                  
		   ncnt = 0;
                   for (n=0;n<lagnum;n++) {
                        if (abs(lagtable[0][n]-lagtable[1][n]) !=lag) continue;
                        i1 = rxlag + lagtable[0][n]*prm->mpinc/prm->smsep + R;
                        i2 = rxlag + lagtable[1][n]*prm->mpinc/prm->smsep + R;
                        p1 = smpwr[i1];
                        p2 = smpwr[i2];
                        if ((p1>0) && (p2>0)) {
                            if ((p1<(p0*sqrt(1.0*nave))) && 
                                      (p2<(p0*sqrt(1.0*nave)))) {
                                 i1min = i1;
                                 i2min = i2;
                                 }
                            if ((i1min>=0) && (i2min>=0)) {
                               for (i=0;i<nave;i++) {
                                    acfr[i+ncnt]=
                                          iqptr[2*(i*nsample+i1min)+ioffset]*
                                          iqptr[2*(i*nsample+i2min)+ioffset]+
                                          iqptr[2*(i*nsample+i1min)+roffset]*
                                          iqptr[2*(i*nsample+i2min)+roffset];
                                    acfi[i+ncnt]=
                                           iqptr[2*(i*nsample+i1min)+roffset]*
                                           iqptr[2*(i*nsample+i2min)+ioffset]-
                                           iqptr[2*(i*nsample+i2min)+roffset]*
                                           iqptr[2*(i*nsample+i1min)+ioffset];
                                    }
                                ncnt += nave;
                                i1min  = -1;
                                i2min  = -1;
                                } 
                            }
                        if (ncnt>0) {
                            qsort(acfr,ncnt,sizeof(float),ACFexPwrSort);
                            qsort(acfi,ncnt,sizeof(float),ACFexPwrSort);
                            lagpwr = sqrt(acfr[ncnt/2]*acfr[ncnt/2] + 
                                acfi[ncnt/2]*acfi[ncnt/2]);
                            if (lagpwr>=(p0/sqrt(1.0*nave))) {
                                acfbuf[R*2*mplgs+2*L]   = acfr[ncnt/2];
                                acfbuf[R*2*mplgs+2*L+1] = acfi[ncnt/2];
                                lagcnt++;
                                sumnzlagpwr += lagpwr;
                                }
                            }
                        }
		L++; }

              if (sumnzlagpwr < (*noise*(1.0*lagcnt)/sqrt(1.0*nave))) {
                   for (L=0;L<mplgs;L++) {
                        acfbuf[R*2*mplgs+2*L]   = 0.0;
                        acfbuf[R*2*mplgs+2*L+1] = 0.0;
                        }
                   }
              }
           }
        }
      
     free(smpwr);
     free(srtpwr);
     free(acfr);
     free(acfi);
     return 0;
}

