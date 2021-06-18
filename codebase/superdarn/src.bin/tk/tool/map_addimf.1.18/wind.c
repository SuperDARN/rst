/* wind.c
   ======
   Author: R.J.Barnes
*/

/*
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
#include "rtypes.h"
#include "rtime.h"
#include "cdf.h"
#include "rcdf.h"
#include "istp.h"

int windmfi_imf(CDFid id,struct swdata *ptr,double stime,double etime) {
  long yr,mo,dy,hr,mt,sc,ms;
  double rtime;
  int i,cnt=0;
  char *varlist[]={"Epoch3","B3GSM","B3GSE",NULL};
  struct RCDFData *data=NULL;


  int max;
  int rmax;
  int status;

  double *dptr;
  float *fptr;

  strcpy(ptr->sat,"we");
  strcpy(ptr->ins,"mfi");

  cnt=ptr->cnt;
  rmax=RCDFMaxRecZ(id,varlist[0]);
  max=cnt+rmax;

  if (rmax==0) return -1;

  data=RCDFMake(3);

  if (ptr->time==NULL) ptr->time=malloc(sizeof(double)*max);
  else ptr->time=realloc(ptr->time,sizeof(double)*max);

  if (ptr->BGSMc==NULL) ptr->BGSMc=malloc(3*sizeof(float)*max);
  else ptr->BGSMc=realloc(ptr->BGSMc,3*sizeof(float)*max);

  if (ptr->BGSEc==NULL) ptr->BGSEc=malloc(3*sizeof(float)*max);
  else ptr->BGSEc=realloc(ptr->BGSEc,3*sizeof(float)*max);

  if (ptr->Vx==NULL) ptr->Vx=malloc(sizeof(float)*max);
  else ptr->Vx=realloc(ptr->Vx,sizeof(float)*max);

  if (ptr->Kp==NULL) ptr->Kp=malloc(sizeof(float)*max);
  else ptr->Kp=realloc(ptr->Kp,sizeof(float)*max);

  for (i=0;i<rmax;i++) {
    status=RCDFReadZ(id,i,varlist,data);
    if (status==0) {
       dptr=(double *) data[0].data;
       EPOCHbreakdown(dptr[0],&yr,&mo,&dy,&hr,&mt,&sc,&ms); 
       rtime=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc+(ms/1000.0));
       if (rtime<stime) continue;
       if (rtime>etime) break;

       ptr->time[cnt]=rtime;
       fptr=(float *) data[1].data;
       ptr->BGSMc[cnt*3]=fptr[0];
       ptr->BGSMc[cnt*3+1]=fptr[1];
       ptr->BGSMc[cnt*3+2]=fptr[2];
       fptr=(float *) data[2].data;
       ptr->BGSEc[cnt*3]=fptr[0];
       ptr->BGSEc[cnt*3+1]=fptr[1];
       ptr->BGSEc[cnt*3+2]=fptr[2];
       cnt++;
    }
  }
  RCDFFree(data,3);
  ptr->cnt=cnt;
  return 0;
}









