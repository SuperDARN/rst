/* mergegrid.c
   ===========
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



void GridLinReg(int num,struct GridGVec **data,double *vpar,double *vper) {

  int k;
  double sx=0,cx=0,ysx=0,ycx=0,cxsx=0;
  double den;
  for (k=0;k<num;k++) {
    sx=sx+sin(data[k]->azm*PI/180.0)*sin(data[k]->azm*PI/180.0);
    cx=cx+cos(data[k]->azm*PI/180.0)*cos(data[k]->azm*PI/180.0);
    ysx=ysx+data[k]->vel.median*sin(data[k]->azm*PI/180.0);
    ycx=ycx+data[k]->vel.median*cos(data[k]->azm*PI/180.0);
    cxsx=cxsx+sin(data[k]->azm*PI/180.0)*cos(data[k]->azm*PI/180.0);
  }   
    
  
  den=sx*cx-cxsx*cxsx;
  if (den !=0) {
    *vpar=(sx*ycx-cxsx*ysx)/den;
    *vper=(cx*ysx-cxsx*ycx)/den;
  } else {
    *vpar=0;
    *vper=0;
  }

}


void GridMerge(struct GridData *mptr,struct GridData *ptr) {

  int i;
  int k;
  int *index=NULL;
  int *count=NULL;

  struct GridGVec **data=NULL;
  int data_count=0;
  int num=0;

  double vpar,vper;

  ptr->st_time=mptr->st_time;
  ptr->ed_time=mptr->ed_time;

  ptr->xtd=0;  
  ptr->vcnum=0;
  ptr->stnum=0;

  if (mptr->stnum==0) return;

  ptr->stnum=1;
   
  if (ptr->sdata !=NULL) ptr->sdata=realloc(ptr->sdata,
					    sizeof(struct GridSVec));
  else ptr->sdata=malloc(sizeof(struct GridSVec));
   
  ptr->sdata[0].st_id=255;
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
  
  if (ptr->data !=NULL) free(ptr->data);
  ptr->data=NULL;
  
  /* scan the grid file for unique cell numbers */
 
  for (i=0;i<mptr->vcnum;i++) {
   
    if (num==0) {
      index=malloc(sizeof(int));
      count=malloc(sizeof(int));
      index[0]=mptr->data[i].index;
      count[0]=1;
      num++;
    }  else {
      /* search to determine if this is a new cell */
      for (k=0;(index[k] !=mptr->data[i].index) && (k<num);k++);
      if (k==num) {
        /* new cell */
        index=realloc(index,sizeof(int)*(num+1));
        count=realloc(count,sizeof(int)*(num+1));
        index[num]=mptr->data[i].index;
        count[num]=1;
        num++;
      } else count[k]++; /* existing cell */
    }
  }
 

  for (k=0;k<num;k++) {
    if (count[k]<2) continue;
    data_count=0;         
    /* locate all the data points in the array */
   
    for (i=0;i<mptr->vcnum;i++) {
      
      if (mptr->data[i].index==index[k]) {
        /* got a point from a grid cell so add to the list */
        if (data==NULL) data=malloc(sizeof(struct gvec *));
        else data=realloc(data,sizeof(struct gvec *)*(data_count+1));
        data[data_count]=&mptr->data[i];
        data_count++;
      }
    }
    /* done the search and data is an array pointing to all the vectors */
   
    if (data==NULL) continue;
  
    /* add the new point to the merge grid */

    if (ptr->data==NULL) ptr->data=malloc(sizeof(struct GridGVec));
    else ptr->data=realloc(ptr->data,sizeof(struct GridGVec)*(ptr->vcnum+1));


    for (i=0;i<data_count;i++) data[i]->azm=90-data[i]->azm;
    GridLinReg(data_count,data,&vpar,&vper);

    if (vper !=0) {         
      ptr->data[ptr->vcnum].azm=atan(vpar/vper)*180.0/PI;
      if (vper<0) ptr->data[ptr->vcnum].azm+=180.0;
    } else ptr->data[ptr->vcnum].azm=0;

    ptr->data[ptr->vcnum].mlon=data[0]->mlon;
    ptr->data[ptr->vcnum].mlat=data[0]->mlat;
    ptr->data[ptr->vcnum].srng=data[0]->srng;
    ptr->data[ptr->vcnum].vel.median=sqrt(vpar*vpar+vper*vper);
    ptr->data[ptr->vcnum].vel.sd=0;
    ptr->data[ptr->vcnum].pwr.median=0;
    ptr->data[ptr->vcnum].pwr.sd=0;
    ptr->data[ptr->vcnum].wdt.median=0;
    ptr->data[ptr->vcnum].wdt.sd=0;
    ptr->data[ptr->vcnum].st_id=255;
    ptr->data[ptr->vcnum].chn=0;   
    ptr->data[ptr->vcnum].index=index[k];
    ptr->vcnum++;
    ptr->sdata[0].npnt++;
    free(data);
    data=NULL;
    data_count=0;


  }

  if (index !=NULL) free(index);
  if (count !=NULL) free(count);
 
}
     

















