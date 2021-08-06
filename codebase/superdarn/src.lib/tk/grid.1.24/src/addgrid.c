/* addgrid.c
   ==========
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
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rfile.h"
#include "griddata.h"



void GridAdd(struct GridData *a,struct GridData *b,int recnum) {
  int i;
  int j;

  if (b==NULL)  { /* tidy up */
    if (recnum>1) {
      for (i=0;i<a->stnum;i++) {
        a->sdata[i].freq0=a->sdata[i].freq0/recnum;
        a->sdata[i].noise.mean=a->sdata[i].noise.mean/recnum;
        a->sdata[i].noise.sd=a->sdata[i].noise.sd/recnum;
      }
    }
    return;
  }
  a->xtd=b->xtd; 
  if (b->stnum !=0) {
    if (a->stnum==0) {
      a->stnum=b->stnum;
      if (a->sdata==NULL) a->sdata=malloc(sizeof(struct GridSVec)*b->stnum);
      else a->sdata=realloc(a->sdata,sizeof(struct GridSVec)*b->stnum);
      memcpy(a->sdata,b->sdata,sizeof(struct GridSVec)*b->stnum);
    } else {
      int st_id;
      /* merge the header information */
      for (i=0;i<b->stnum;i++) {
         st_id=b->sdata[i].st_id;
         for (j=0;(j<a->stnum) && (a->sdata[j].st_id !=st_id);j++);
         if (j<a->stnum) { /* merge record */
            a->sdata[j].npnt+=b->sdata[i].npnt;
            a->sdata[j].freq0+=b->sdata[i].freq0;
            if (b->sdata[i].gsct !=0) a->sdata[i].gsct=b->sdata[i].gsct !=0;
            a->sdata[j].noise.mean+=b->sdata[i].noise.mean;
            a->sdata[j].noise.sd+=b->sdata[i].noise.sd;
            if (b->sdata[i].vel.min<a->sdata[j].vel.min)
                a->sdata[j].vel.min=b->sdata[i].vel.min;
            if (b->sdata[i].pwr.min<a->sdata[j].pwr.min)
                a->sdata[j].pwr.min=b->sdata[i].pwr.min;
            if (b->sdata[i].wdt.min<a->sdata[j].wdt.min)
                a->sdata[j].wdt.min=b->sdata[i].wdt.min;
            if (b->sdata[i].vel.max>a->sdata[j].vel.max)
                a->sdata[j].vel.max=b->sdata[i].vel.max;
            if (b->sdata[i].pwr.max>a->sdata[j].pwr.max)
                a->sdata[j].pwr.max=b->sdata[i].pwr.max;
            if (b->sdata[i].wdt.max>a->sdata[j].wdt.max)
                a->sdata[j].wdt.max=b->sdata[i].wdt.max;
	 } else {
           a->sdata=realloc(a->sdata,sizeof(struct GridSVec)*(j+1));
           memcpy(&a->sdata[j],&b->sdata[i],sizeof(struct GridSVec));
           a->stnum++;
	 }
      }
    }
  }

  i=a->vcnum; 
  j=a->vcnum+b->vcnum;

  if (b->vcnum !=0) {  
    if (a->data==NULL) a->data=malloc(sizeof(struct GridGVec)*j);
    else a->data=realloc(a->data,sizeof(struct GridGVec)*j);
    memcpy(&a->data[i],b->data,
          sizeof(struct GridGVec)*b->vcnum);
    a->vcnum=j;
  }

}
