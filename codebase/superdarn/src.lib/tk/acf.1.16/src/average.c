/* average.c
   =========
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


