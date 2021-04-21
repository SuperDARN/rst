/* wind.c
   ======
   Author: R.J.Barnes
*/

/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
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

int windswe_pos(CDFid id,struct posdata *ptr,double stime,double etime) {
  long yr,mo,dy,hr,mt,sc,ms;
  double rtime;
  int i,cnt=0;
  char *varlist[]={"Epoch","SC_pos_GSM","SC_pos_gse",NULL};
  struct RCDFData *data=NULL;

  int max;
  int rmax;
  int status;

  double *dptr;
  float *fptr;
  strcpy(ptr->sat,"we");
  strcpy(ptr->ins,"swe");

  cnt=ptr->cnt;
  rmax=RCDFMaxRecR(id,varlist[0]);
  max=cnt+rmax;   

  if (rmax==0)return -1;
  data=RCDFMake(3);

  if (ptr->time==NULL) ptr->time=malloc(sizeof(double)*max);
  else ptr->time=realloc(ptr->time,sizeof(double)*max);

  if (ptr->PGSM==NULL) ptr->PGSM=malloc(3*sizeof(float)*max);
  else ptr->PGSM=realloc(ptr->PGSM,3*sizeof(float)*max);

  if (ptr->PGSE==NULL) ptr->PGSE=malloc(3*sizeof(float)*max);
  else ptr->PGSE=realloc(ptr->PGSE,3*sizeof(float)*max);

  for (i=0;i<rmax;i++) {
  
    status=RCDFReadR(id,i,varlist,data);
    
    if (status==0) {
       dptr=(double *) data[0].data;
       EPOCHbreakdown(dptr[0],&yr,&mo,&dy,&hr,&mt,&sc,&ms); 
       rtime=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc+(ms/1000.0));
       if (rtime<stime) continue;
       if (rtime>etime) break;

       ptr->time[cnt]=rtime;
       fptr=(float *) data[1].data;
       ptr->PGSM[cnt*3]=fptr[0]/6370.0;
       ptr->PGSM[cnt*3+1]=fptr[1]/6370.0;
       ptr->PGSM[cnt*3+2]=fptr[2]/6370.;
       fptr=(float *) data[2].data;
       ptr->PGSE[cnt*3]=fptr[0]/6370.0;
       ptr->PGSE[cnt*3+1]=fptr[1]/6370.0;
       ptr->PGSE[cnt*3+2]=fptr[2]/6370.0;
       cnt++;
    }
  }
  RCDFFree(data,3);
  ptr->cnt=cnt; 
  return 0;
}


int windswe_plasma(CDFid id,struct plasmadata *ptr,double stime,double etime) {
  long yr,mo,dy,hr,mt,sc,ms;
  double rtime;
  int i,cnt=0;
  char *varlist[]={"Epoch","V_GSM","V_GSE","THERMAL_SPD","Np",NULL};
  struct RCDFData *data=NULL;

  int max;
  int rmax;
  int status;

  double *dptr;
  float *fptr;
  float Mp=1.6e-27;
  float Vx;
  int pcnt=0;

  strcpy(ptr->sat,"we");
  strcpy(ptr->ins,"swe");

  cnt=ptr->cnt;
  rmax=RCDFMaxRecR(id,varlist[0]);
  max=cnt+rmax;     

  if (rmax==0) return -1;
  data=RCDFMake(5);


  if (ptr->time==NULL) ptr->time=malloc(sizeof(double)*max);
  else ptr->time=realloc(ptr->time,sizeof(double)*max);

  if (ptr->VGSM==NULL) ptr->VGSM=malloc(3*sizeof(float)*max);
  else ptr->VGSM=realloc(ptr->VGSM,3*sizeof(float)*max);

  if (ptr->VGSE==NULL) ptr->VGSE=malloc(3*sizeof(float)*max);
  else ptr->VGSE=realloc(ptr->VGSE,3*sizeof(float)*max);

  if (ptr->vth==NULL) ptr->vth=malloc(sizeof(float)*max);
  else ptr->vth=realloc(ptr->vth,sizeof(float)*max);

  if (ptr->den==NULL) ptr->den=malloc(sizeof(float)*max);
  else ptr->den=realloc(ptr->den,sizeof(float)*max);

 if (ptr->pre==NULL) ptr->pre=malloc(sizeof(float)*max);
  else ptr->pre=realloc(ptr->pre,sizeof(float)*max);


  for (i=0;i<rmax;i++) {
  
    status=RCDFReadR(id,i,varlist,data);
    
    if (status==0) {
       dptr=(double *) data[0].data;
       EPOCHbreakdown(dptr[0],&yr,&mo,&dy,&hr,&mt,&sc,&ms); 
       rtime=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc+(ms/1000.0));
       if (rtime<stime) continue;
       if (rtime>etime) break;

       ptr->time[cnt]=rtime;
       fptr=(float *) data[1].data;
       ptr->VGSM[cnt*3]=fptr[0];
       ptr->VGSM[cnt*3+1]=fptr[1];
       ptr->VGSM[cnt*3+2]=fptr[2];
       fptr=(float *) data[2].data;
       ptr->VGSE[cnt*3]=fptr[0];
       ptr->VGSE[cnt*3+1]=fptr[1];
       ptr->VGSE[cnt*3+2]=fptr[2];
       fptr=(float *) data[3].data;
       ptr->vth[cnt]=fptr[0];
       fptr=(float *) data[4].data;
       ptr->den[cnt]=fptr[0];
       
       Vx=ptr->VGSM[3*cnt];
       if ((ptr->den[cnt]>0) && (Vx > -10000)) {
          ptr->pre[cnt]=(ptr->den[cnt]*Mp*1e6)*
                     fabs(Vx*1e3)*fabs(Vx*1e3)*1e9;
          pcnt++;
       } else ptr->pre[cnt]=ptr->den[cnt];
       

       
       cnt++;
    }
  }
  if (pcnt==0) memset(ptr->pre,0,sizeof(float)*cnt);


  ptr->cnt=cnt;
  return 0;
}

int windmfi_pos(CDFid id,struct posdata *ptr,double stime,double etime) {
  long yr,mo,dy,hr,mt,sc,ms;
  double rtime;
  int i,cnt=0;
  char *varlist[]={"Epoch","PGSM","PGSE",NULL};
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
  if (ptr->time==NULL) ptr->time=malloc(sizeof(double)*max);
  else ptr->time=realloc(ptr->time,sizeof(double)*max);

  if (ptr->PGSM==NULL) ptr->PGSM=malloc(3*sizeof(float)*max);
  else ptr->PGSM=realloc(ptr->PGSM,3*sizeof(float)*max);

  if (ptr->PGSE==NULL) ptr->PGSE=malloc(3*sizeof(float)*max);
  else ptr->PGSE=realloc(ptr->PGSE,3*sizeof(float)*max);

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
       ptr->PGSM[cnt*3]=fptr[0];
       ptr->PGSM[cnt*3+1]=fptr[1];
       ptr->PGSM[cnt*3+2]=fptr[2];
       fptr=(float *) data[2].data;
       ptr->PGSE[cnt*3]=fptr[0];
       ptr->PGSE[cnt*3+1]=fptr[1];
       ptr->PGSE[cnt*3+2]=fptr[2];
       cnt++;
    }
  }
  RCDFFree(data,3);
  ptr->cnt=cnt;
  return 0;
}

int windmfi_imf(CDFid id,struct imfdata *ptr,double stime,double etime) {
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









