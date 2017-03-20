/* badlag.c
   ========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "rtypes.h"
#include "tsg.h"



int ACFBadLagZero(struct TSGprm *prm,int mplgs,int *lagtable[2]) {

  int i;
  int badrng=-1;
  int sampleunit;
  int numrng;
  int numfrng;
  int mpincdiff;
  int pulserisetime;
  int halftxpl;

  /* search which pulse in the pulse-pattern that is used to compute lag 0
      power  */
 
  for(i=0; i < prm->mppul;i++)
    if(lagtable[0][0] == prm->pat[i]) break;
    if(i >= (prm->mppul - 1)) badrng = -1;  
    else {   
      sampleunit = prm->mpinc / prm->smsep;
      mpincdiff = prm->pat[i+1] - 
                   prm->pat[i];
      numrng = mpincdiff * sampleunit;
   
      /* compute the pulse rise time effect */

      halftxpl = prm->txpl / 2;
      pulserisetime = halftxpl /  prm->smsep;
      if ((halftxpl % prm->smsep) != 0)
         pulserisetime++;   /* add one because of the int div */
      numrng = numrng - pulserisetime;  /* subtract pulse rise time */
      numfrng = prm->lagfr / prm->smsep;

      /* now compute the start of the bad range */

      badrng = numrng - numfrng;
      if (badrng < 0) badrng = 0;
   }      
   return badrng;
}

