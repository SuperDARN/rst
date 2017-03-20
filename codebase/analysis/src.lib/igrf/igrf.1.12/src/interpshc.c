/* interpshc.c
   ===========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/



#include <math.h>
#include "rmath.h"

int interpshc(double date, 
              double dte1, 
              int nmax1, double *gh1, 
              double dte2, double nmax2, 
              double *gh2, int *nmax, double *gh) {
    
  
    int i, k, l;
    double factor;
  
    --gh;
    --gh2;
    --gh1;

    factor = (date - dte1) / (dte2 - dte1);
    if (nmax1 == nmax2) {
	k = nmax1 * (nmax1 + 2);
	*nmax = nmax1;
    } else if (nmax1 > nmax2) {
	k = nmax2 * (nmax2 + 2);
	l = nmax1 * (nmax1 + 2);
    
	for (i = k + 1; i <= l; ++i) {
	    gh[i] = gh1[i] + factor * (-gh1[i]);
	}
	*nmax = nmax1;
    } else {
	k = nmax1 * (nmax1 + 2);
	l = nmax2 * (nmax2 + 2);
     
	for (i = k + 1; i <= l; ++i) {
	    gh[i] = factor * gh2[i];
	}
	*nmax = nmax2;
    }
  
    for (i = 1; i <= k; ++i) {
	gh[i] = gh1[i] + factor * (gh2[i] - gh1[i]);
    }
    return 0;
} 

