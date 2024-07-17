/* integrategrid.c
   ===============
   Author: R.J.Barnes
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
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rfile.h"
#include "griddata.h"



void GridIntegrate(struct GridData *a,struct GridData *b,double *err) {


  int i=0,j=0,k=0,l=0,m=0;
  int st_id,index;
  int pnt=0;
  double v_e,p_e,w_e;

  if (b==NULL) return;
  GridSort(b);

  a->st_time=b->st_time;
  a->ed_time=b->ed_time;
  a->xtd=b->xtd;
  a->stnum=b->stnum;
  a->vcnum=0;

 
   
  if (b->stnum>0) {
    if (a->sdata==NULL) a->sdata=malloc(sizeof(struct GridSVec)*b->stnum);
    else a->sdata=realloc(a->sdata,sizeof(struct GridSVec)*b->stnum);
  } else if (a->sdata !=NULL) {
    free(a->sdata);
    a->sdata=NULL;
  }

  /* allocate enough space in the output grid to store
     every possible vector in the input */

  if (b->vcnum>0) { 
  if (a->data==NULL) a->data=malloc(sizeof(struct GridGVec)*b->vcnum);
  else a->data=realloc(a->data,sizeof(struct GridGVec)*b->vcnum);
  } else if (a->data !=NULL) {
    free(a->data);
    a->data=NULL;
  }

  
  if (b->stnum !=0) memcpy(a->sdata,b->sdata,
                             sizeof(struct GridSVec)*b->stnum);
  if (b->vcnum ==0) return;

  /* clear the array */
  memset(a->data,0,sizeof(struct GridGVec)*b->vcnum);
    
  /* average together vectors from the same radar that lie
     in the same cell */

 
  i=0;
  while (i<b->vcnum) {
    st_id=b->data[i].st_id;
    j=i+1;
    while (j<b->vcnum) {
      if (b->data[j].st_id !=st_id) break;
      j++;
    }
    
    /* okay i and j mark the start and end of a station */    
    k=i;
    while (k<j) {
      index=b->data[k].index;
      l=k+1;
      while (l<j) {
        if (b->data[l].index !=index) break;      
        l++;
      }
      for (m=k;m<l;m++) { /* add vectors to the output */
        v_e=b->data[m].vel.sd;
        p_e=b->data[m].pwr.sd;
        w_e=b->data[m].wdt.sd;

        if (v_e<err[0]) v_e=err[0];
        if (p_e<err[1]) p_e=err[1];
        if (w_e<err[2]) w_e=err[2];

        a->data[pnt].azm+=b->data[m].azm;
        a->data[pnt].srng+=b->data[m].srng;
        a->data[pnt].vel.median+=b->data[m].vel.median*1/(v_e*v_e);
        a->data[pnt].pwr.median+=b->data[m].pwr.median*1/(p_e*p_e);
        a->data[pnt].wdt.median+=b->data[m].wdt.median*1/(w_e*w_e);
        
        a->data[pnt].vel.sd+=1/(v_e*v_e);
        a->data[pnt].pwr.sd+=1/(p_e*p_e);
        a->data[pnt].wdt.sd+=1/(w_e*w_e);
      }

      a->data[pnt].azm=a->data[pnt].azm/(l-k);
      a->data[pnt].srng=a->data[pnt].srng/(l-k);
      
      a->data[pnt].vel.median=a->data[pnt].vel.median/a->data[pnt].vel.sd;
      a->data[pnt].wdt.median=a->data[pnt].wdt.median/a->data[pnt].wdt.sd;
      a->data[pnt].pwr.median=a->data[pnt].pwr.median/a->data[pnt].pwr.sd;
      

      a->data[pnt].vel.sd=1/sqrt(a->data[pnt].vel.sd);
      a->data[pnt].wdt.sd=1/sqrt(a->data[pnt].wdt.sd);
      a->data[pnt].pwr.sd=1/sqrt(a->data[pnt].pwr.sd);
      a->data[pnt].mlat=b->data[k].mlat;
      a->data[pnt].mlon=b->data[k].mlon;

      a->data[pnt].st_id=st_id;
      a->data[pnt].index=index;
      pnt++; 
      
      k=l;
    }
    i=j;
  }
  a->vcnum=pnt;
  a->data=realloc(a->data,sizeof(struct GridGVec)*b->vcnum);


}

 
