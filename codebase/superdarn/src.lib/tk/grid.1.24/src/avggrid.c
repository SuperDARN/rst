/* avggrid.c
   =========
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
#include <sys/types.h>
#include "rtypes.h"
#include "rmath.h"
#include "rfile.h"
#include "griddata.h"
     


int GridLocateCell(int npnt,struct GridGVec *ptr,int index) {
  int i;
  for (i=0;i<npnt && (ptr[i].index !=index);i++);
  return i;
}

void GridAverage(struct GridData *mptr,struct GridData *ptr,int flg) {

  int i;
  int k;
 

  ptr->st_time=mptr->st_time;
  ptr->ed_time=mptr->ed_time;
  ptr->xtd=mptr->xtd;  
  ptr->vcnum=0;
  ptr->stnum=1;
  
  if (ptr->sdata !=NULL) ptr->sdata=realloc(ptr->sdata,
					    sizeof(struct GridSVec));
  else ptr->sdata=malloc(sizeof(struct GridSVec));
   
  ptr->sdata[0].st_id=0;
  ptr->sdata[0].chn=0;
  ptr->sdata[0].freq0=0;
  ptr->sdata[0].major_revision=mptr->sdata[0].major_revision;
  ptr->sdata[0].minor_revision=mptr->sdata[0].minor_revision;

  /* need to verify that these are correct */

  ptr->sdata[0].prog_id=mptr->sdata[0].prog_id;
  ptr->sdata[0].noise.mean=mptr->sdata[0].noise.mean;
  ptr->sdata[0].noise.sd=mptr->sdata[0].noise.sd;

  ptr->sdata[0].gsct=mptr->sdata[0].gsct;

  ptr->sdata[0].vel.min=mptr->sdata[0].vel.min;
  ptr->sdata[0].vel.max=mptr->sdata[0].vel.max;
  ptr->sdata[0].pwr.min=mptr->sdata[0].pwr.min;
  ptr->sdata[0].pwr.max=mptr->sdata[0].pwr.max;
  ptr->sdata[0].wdt.min=mptr->sdata[0].wdt.min;
  ptr->sdata[0].wdt.max=mptr->sdata[0].wdt.max;


  if (ptr->data !=NULL) {
    free(ptr->data);
    ptr->data=NULL;
  }

  
  for (i=0;i<mptr->vcnum;i++) {

      k=GridLocateCell(ptr->vcnum,ptr->data,mptr->data[i].index);
      if (k==ptr->vcnum) {
        /* new cell */
        ptr->vcnum++;
        if (ptr->data==NULL) ptr->data=malloc(sizeof(struct GridGVec));
        else ptr->data=realloc(ptr->data,sizeof(struct GridGVec)*ptr->vcnum);

        ptr->data[k].azm=mptr->data[i].azm;
        ptr->data[k].srng=mptr->data[i].srng;
        ptr->data[k].vel.median=mptr->data[i].vel.median;
        ptr->data[k].vel.sd=mptr->data[i].vel.sd;
        ptr->data[k].pwr.median=mptr->data[i].pwr.median;
        ptr->data[k].pwr.sd=mptr->data[i].pwr.sd;
        ptr->data[k].wdt.median=mptr->data[i].wdt.median;
        ptr->data[k].wdt.sd=mptr->data[i].wdt.sd;
        ptr->data[k].mlon=mptr->data[i].mlon;
        ptr->data[k].mlat=mptr->data[i].mlat;
        ptr->data[k].index=mptr->data[i].index;      
     
        ptr->data[k].st_id=1;
        ptr->data[k].chn=0;
        if (flg !=0) {
          ptr->data[k].st_id=mptr->data[i].st_id;
          ptr->data[k].chn=mptr->data[i].chn;
	}  
      } else { 
        if (flg==0) {
                ptr->data[k].index=mptr->data[i].index;          
                ptr->data[k].mlon=mptr->data[i].mlon;
                ptr->data[k].mlat=mptr->data[i].mlat;
                ptr->data[k].azm+=mptr->data[i].azm;
                ptr->data[k].srng+=mptr->data[i].srng;


                ptr->data[k].vel.median+=mptr->data[i].vel.median;
                ptr->data[k].vel.sd+=mptr->data[i].vel.sd;
                ptr->data[k].pwr.median+=mptr->data[i].pwr.median;
                ptr->data[k].pwr.sd+=mptr->data[i].pwr.sd;
                ptr->data[k].wdt.median+=mptr->data[i].wdt.median;
                ptr->data[k].wdt.sd+=mptr->data[i].wdt.sd;

                ptr->data[k].st_id++;
              } else if ((flg==1) && 
                       (mptr->data[i].pwr.median>ptr->data[k].pwr.median)) {
                ptr->data[k].index=mptr->data[i].index;          
                ptr->data[k].mlon=mptr->data[i].mlon;
                ptr->data[k].mlat=mptr->data[i].mlat;
                ptr->data[k].azm=mptr->data[i].azm;
                ptr->data[k].srng=mptr->data[i].srng;
                ptr->data[k].vel.median=mptr->data[i].vel.median;
                ptr->data[k].vel.sd=mptr->data[i].vel.sd;
                ptr->data[k].pwr.median=mptr->data[i].pwr.median;
                ptr->data[k].pwr.sd=mptr->data[i].pwr.sd;
                ptr->data[k].wdt.median=mptr->data[i].wdt.median;
                ptr->data[k].wdt.sd=mptr->data[i].wdt.sd;
                ptr->data[k].st_id=mptr->data[i].st_id;
	      } else if ((flg==2) && 
                       (mptr->data[i].vel.median>ptr->data[k].vel.median)) {
                ptr->data[k].index=mptr->data[i].index;          
                ptr->data[k].mlon=mptr->data[i].mlon;
                ptr->data[k].mlat=mptr->data[i].mlat;
                ptr->data[k].azm=mptr->data[i].azm;
                ptr->data[k].srng=mptr->data[i].srng;
                ptr->data[k].vel.median=mptr->data[i].vel.median;
                ptr->data[k].vel.sd=mptr->data[i].vel.sd;
                ptr->data[k].pwr.median=mptr->data[i].pwr.median;
                ptr->data[k].pwr.sd=mptr->data[i].pwr.sd;
                ptr->data[k].wdt.median=mptr->data[i].wdt.median;
                ptr->data[k].wdt.sd=mptr->data[i].wdt.sd;
                ptr->data[k].st_id=mptr->data[i].st_id;
              } else if ((flg==3) && 
                       (mptr->data[i].wdt.median>ptr->data[k].wdt.median)) {
                ptr->data[k].index=mptr->data[i].index;          
                ptr->data[k].mlon=mptr->data[i].mlon;
                ptr->data[k].mlat=mptr->data[i].mlat;
                ptr->data[k].azm=mptr->data[i].azm;
                ptr->data[k].srng=mptr->data[i].srng;

                ptr->data[k].vel.median=mptr->data[i].vel.median;
                ptr->data[k].vel.sd=mptr->data[i].vel.sd;
                ptr->data[k].pwr.median=mptr->data[i].pwr.median;
                ptr->data[k].pwr.sd=mptr->data[i].pwr.sd;
                ptr->data[k].wdt.median=mptr->data[i].wdt.median;
                ptr->data[k].wdt.sd=mptr->data[i].wdt.sd;

                ptr->data[k].st_id=mptr->data[i].st_id;
              } else if ((flg==4) && 
                       (mptr->data[i].pwr.median<ptr->data[k].pwr.median)) {
                ptr->data[k].index=mptr->data[i].index;          
                ptr->data[k].mlon=mptr->data[i].mlon;
                ptr->data[k].mlat=mptr->data[i].mlat;
                ptr->data[k].azm=mptr->data[i].azm;
                ptr->data[k].srng=mptr->data[i].srng;

                ptr->data[k].vel.median=mptr->data[i].vel.median;
                ptr->data[k].vel.sd=mptr->data[i].vel.sd;
                ptr->data[k].pwr.median=mptr->data[i].pwr.median;
                ptr->data[k].pwr.sd=mptr->data[i].pwr.sd;
                ptr->data[k].wdt.median=mptr->data[i].wdt.median;
                ptr->data[k].wdt.sd=mptr->data[i].wdt.sd;

                ptr->data[k].st_id=mptr->data[i].st_id;
	      } else if ((flg==5) && 
			(mptr->data[i].vel.median<ptr->data[k].vel.median)) {
                ptr->data[k].index=mptr->data[i].index;          
                ptr->data[k].mlon=mptr->data[i].mlon;
                ptr->data[k].mlat=mptr->data[i].mlat;
                ptr->data[k].azm=mptr->data[i].azm;
                ptr->data[k].srng=mptr->data[i].srng;

                ptr->data[k].vel.median=mptr->data[i].vel.median;
                ptr->data[k].vel.sd=mptr->data[i].vel.sd;
                ptr->data[k].pwr.median=mptr->data[i].pwr.median;
                ptr->data[k].pwr.sd=mptr->data[i].pwr.sd;
                ptr->data[k].wdt.median=mptr->data[i].wdt.median;
                ptr->data[k].wdt.sd=mptr->data[i].wdt.sd;

                ptr->data[k].st_id=mptr->data[i].st_id;
              } else if ((flg==6) && 
                       (mptr->data[i].wdt.median<ptr->data[k].wdt.median)) {
                ptr->data[k].index=mptr->data[i].index;          
                ptr->data[k].mlon=mptr->data[i].mlon;
                ptr->data[k].mlat=mptr->data[i].mlat;
                ptr->data[k].azm=mptr->data[i].azm;
                ptr->data[k].srng=mptr->data[i].srng;

                ptr->data[k].vel.median=mptr->data[i].vel.median;
                ptr->data[k].vel.sd=mptr->data[i].vel.sd;
                ptr->data[k].pwr.median=mptr->data[i].pwr.median;
                ptr->data[k].pwr.sd=mptr->data[i].pwr.sd;
                ptr->data[k].wdt.median=mptr->data[i].wdt.median;
                ptr->data[k].wdt.sd=mptr->data[i].wdt.sd;

                ptr->data[k].st_id=mptr->data[i].st_id;
              }
	    }
        
  }
  if (flg==0) {
    for (i=0;i<ptr->vcnum;i++) {
      ptr->data[i].azm=ptr->data[i].azm/ptr->data[i].st_id;
      ptr->data[i].srng=ptr->data[i].srng/ptr->data[i].st_id;
      ptr->data[i].vel.median=ptr->data[i].vel.median/ptr->data[i].st_id;
      ptr->data[i].vel.sd=ptr->data[i].vel.sd/ptr->data[i].st_id;

      ptr->data[i].pwr.median=ptr->data[i].pwr.median/ptr->data[i].st_id;
      ptr->data[i].pwr.sd=ptr->data[i].pwr.sd/ptr->data[i].st_id;
      ptr->data[i].wdt.median=ptr->data[i].wdt.median/ptr->data[i].st_id;
      ptr->data[i].wdt.sd=ptr->data[i].wdt.sd/ptr->data[i].st_id;
    }
  }
  ptr->sdata[0].npnt=ptr->vcnum;
  return;
}

















