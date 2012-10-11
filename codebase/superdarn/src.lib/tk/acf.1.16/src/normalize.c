/* normalize.c
   ===========
   Author: R.J.Barnes
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


