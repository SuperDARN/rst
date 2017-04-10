/* normalize.c
   ===========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void ACFNormalize(float *pwr0,float *acfd,float *xcfd,
		  int nrang,int mplgs,float atten) {
  int range,lag;
  if (atten==0) return;
  for (range = 0; range < nrang ;range++) {
    for (lag=0;lag < mplgs;lag++) {
      acfd[range*(2*mplgs)+2*lag] = 
      acfd[range*(2*mplgs)+2*lag]/ atten;
      acfd[range*(2*mplgs)+2*lag+1] =  
      acfd[range*(2*mplgs)+2*lag+1]/atten; 
      if(xcfd !=NULL) {
        xcfd[range*(2*mplgs)+2*lag] =  
        xcfd[range*(2*mplgs)+2*lag] / atten;
        xcfd[range*(2*mplgs)+2*lag+1] =  
        xcfd[range*(2*mplgs)+2*lag+1] / atten; 
      }
    } 
    pwr0[range] = pwr0[range]/atten;
  }
}


