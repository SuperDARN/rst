/* average.c
   =========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int ACFAverage(float *pwr0,float *acfd,
		 float *xcfd,int nave,int nrang,int mplgs) {
   int range;
   int lag;
   
   float *acfdptr=NULL;
   float *xcfdptr=NULL;
   float *pwr0ptr=NULL;
 

   pwr0ptr=pwr0;
  
   
   for (range = 0; range < nrang ; range++) {
     acfdptr = &acfd[range*(2*mplgs)];
     if (xcfd !=NULL) xcfdptr = &xcfd[range*(2*mplgs)];
	      
       for(lag=0;lag < mplgs ; lag++) {

       *acfdptr = *acfdptr / nave;   
	   acfdptr++;
       *acfdptr = *acfdptr / nave;   
       acfdptr++; 
       if(xcfdptr !=NULL) {
         *xcfdptr = *xcfdptr / nave;  /* real */
         xcfdptr++;
         *xcfdptr = *xcfdptr / nave;   /* imag */
         xcfdptr++; 
        }
      } 
      *pwr0ptr = *pwr0ptr / nave;
      pwr0ptr++;
   } 
   return 0;
}


