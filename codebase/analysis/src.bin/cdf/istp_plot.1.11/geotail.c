/* geotail.c
   =========
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
#include "rmath.h"
#include "rtypes.h"
#include "rtime.h"
#include "geopack.h"
#include "cdf.h"
#include "rcdf.h"
#include "istp.h"

int geomgf_pos(CDFid id,struct posdata *ptr,double stime,double etime) {
  long yr,mo,dy,hr,mt,sc,ms;
  double rtime;
  int i,cnt=0;
  char *varlist[]={"Epoch","POS",0,NULL};
  struct RCDFData *data=NULL; 

  int max;
  int rmax;
  int status;

  double gsex,gsey,gsez;
  double gsmx,gsmy,gsmz;


  double *dptr;

  strcpy(ptr->sat,"ge");
  strcpy(ptr->ins,"mgf");

  cnt=ptr->cnt;
  rmax=RCDFMaxRecR(id,varlist[0]);
  max=cnt+rmax;   

  if (rmax==0) return -1;
  data=RCDFMake(2);

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
       dptr=(double *) data[1].data;
       ptr->PGSE[cnt*3]=dptr[0]/6370.0;
       ptr->PGSE[cnt*3+1]=dptr[1]/6370.0;
       ptr->PGSE[cnt*3+2]=dptr[2]/6370.0;


       gsex=dptr[0];
       gsey=dptr[1];
       gsez=dptr[2];

       GeoPackRecalc(yr,mo,dy,hr,mt,(sc+1.0*ms/1000.0));
       GeoPackGseGsm(gsex,gsey,gsez,&gsmx,&gsmy,&gsmz);
   
       ptr->PGSM[cnt*3]=gsmx/6370;
       ptr->PGSM[cnt*3+1]=gsmy/6370;
       ptr->PGSM[cnt*3+2]=gsmz/6370;

       cnt++;
    }
  }


  RCDFFree(data,2);
  ptr->cnt=cnt;
  return 0;
}

int geolep_pos(CDFid id,struct posdata *ptr,double stime,double etime) {
  long yr,mo,dy,hr,mt,sc,ms;
  double rtime;
  int i,cnt=0;
  char *varlist[]={"Epoch","POSITION",0,NULL};
  struct RCDFData *data=NULL;

  int max;
  int rmax;
  int status;

  double gsex,gsey,gsez;
  double gsmx,gsmy,gsmz;

  double *dptr;


  strcpy(ptr->sat,"ge");
  strcpy(ptr->ins,"lep");

  cnt=ptr->cnt;
  rmax=RCDFMaxRecR(id,varlist[0]);
  max=cnt+rmax;   

  if (rmax==0) return -1;
  data=RCDFMake(2);

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

       dptr=(double *) data[1].data;
       ptr->PGSE[cnt*3]=dptr[0]/6370.0;
       ptr->PGSE[cnt*3+1]=dptr[1]/6370.0;
       ptr->PGSE[cnt*3+2]=dptr[2]/6370.0;

       gsex=dptr[0];
       gsey=dptr[1];
       gsez=dptr[2];

       GeoPackRecalc(yr,mo,dy,hr,mt,(sc+1.0*ms/1000.0));
       GeoPackGseGsm(gsex,gsey,gsez,&gsmx,&gsmy,&gsmz);

       ptr->PGSM[cnt*3]=gsmx/6370;
       ptr->PGSM[cnt*3+1]=gsmy/6370;
       ptr->PGSM[cnt*3+2]=gsmz/6370;

       cnt++;
    }
  }

  RCDFFree(data,2);
  ptr->cnt=cnt;
  return 0;
}

int geocpi_plasma(CDFid id,struct plasmadata *ptr,double stime,double etime) {
  long yr,mo,dy,hr,mt,sc,ms;
  double rtime;
  int i,cnt=0;
  char *varlist[]={"Epoch","SW_V","SW_P_Den",NULL};
  struct RCDFData *data=NULL;
 
  int max;
  int rmax;
  int status;

  double gsex,gsey,gsez;
  double gsmx,gsmy,gsmz;

  double *dptr;
  float *fptr;
  float Mp=1.6e-27;
  float Vx;
  int pcnt=0;


  strcpy(ptr->sat,"ge");
  strcpy(ptr->ins,"cpi");

  cnt=ptr->cnt;
  rmax=RCDFMaxRecR(id,varlist[0]);
  max=cnt+rmax;     

  if (rmax==0) return -1;
  data=RCDFMake(3);

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
       
       if (fptr[0]<-1e20) { 
           ptr->VGSE[cnt*3]=1e31;
           ptr->VGSE[cnt*3+1]=1e31;
           ptr->VGSE[cnt*3+2]=1e31;
           ptr->VGSM[cnt*3]=1e31;
           ptr->VGSM[cnt*3+1]=1e31;
           ptr->VGSM[cnt*3+2]=1e31;
           ptr->vth[cnt]=1e31;
           ptr->den[cnt]=1e31;
           ptr->pre[cnt]=1e31;
           cnt++;
           continue;
       }
      
    
      
       ptr->VGSE[cnt*3]=fptr[0];
       ptr->VGSE[cnt*3+1]=fptr[1];
       ptr->VGSE[cnt*3+2]=fptr[2];

       gsex=fptr[0];
       gsey=fptr[1];
       gsez=fptr[2];

       GeoPackRecalc(yr,mo,dy,hr,mt,(sc+1.0*ms/1000.0));
       GeoPackGseGsm(gsex,gsey,gsez,&gsmx,&gsmy,&gsmz);

       ptr->VGSM[cnt*3]=gsmx;
       ptr->VGSM[cnt*3+1]=gsmy;
       ptr->VGSM[cnt*3+2]=gsmz;

       ptr->vth[cnt]=0;
       fptr=(float *) data[2].data;
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

  RCDFFree(data,3);
  ptr->cnt=cnt;
  return 0;
}



int geomgf_imf(CDFid id,struct imfdata *ptr,double stime,double etime) {
  long yr,mo,dy,hr,mt,sc,ms;
  double rtime;
  int i,cnt=0;
  char *varlist[]={"Epoch","IB_vector",NULL,NULL};
  struct RCDFData *data=NULL;

  int max;
  int rmax;
  int status;

  double *dptr;

  int16 *sptr; 

  double gsex,gsey,gsez;
  double gsmx,gsmy,gsmz;

  strcpy(ptr->sat,"ge");
  strcpy(ptr->ins,"mgf");

  cnt=ptr->cnt;
  rmax=RCDFMaxRecR(id,varlist[0]);
  max=cnt+rmax;  

  if (rmax==0) return -1;
  data=RCDFMake(2);

  if (ptr->time==NULL) ptr->time=malloc(sizeof(double)*max);
  else ptr->time=realloc(ptr->time,sizeof(double)*max);

  if (ptr->BGSMc==NULL) ptr->BGSMc=malloc(3*sizeof(float)*max);
  else ptr->BGSMc=realloc(ptr->BGSMc,3*sizeof(float)*max);

  if (ptr->BGSEc==NULL) ptr->BGSEc=malloc(3*sizeof(float)*max);
  else ptr->BGSEc=realloc(ptr->BGSEc,3*sizeof(float)*max);

  for (i=0;i<rmax;i++) {
    status=RCDFReadR(id,i,varlist,data);
    if (status==0) {
       dptr=(double *) data[0].data;
       EPOCHbreakdown(dptr[0],&yr,&mo,&dy,&hr,&mt,&sc,&ms); 
      
       rtime=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc+(ms/1000.0));
       if (rtime<stime) continue;
       if (rtime>etime) break;
       ptr->time[cnt]=rtime;

       sptr=(int16 *) data[1].data;
       ptr->BGSEc[cnt*3]=sptr[0]/10.0;
       ptr->BGSEc[cnt*3+1]=sptr[1]/10.0;
       ptr->BGSEc[cnt*3+2]=sptr[2]/10.0;
       
       gsex=sptr[0]/10.0;
       gsey=sptr[1]/10.0;
       gsez=sptr[2]/10.0;

       GeoPackRecalc(yr,mo,dy,hr,mt,(sc+1.0*ms/1000.0));
       GeoPackGseGsm(gsex,gsey,gsez,&gsmx,&gsmy,&gsmz);

       ptr->BGSMc[cnt*3]=gsmx;
       ptr->BGSMc[cnt*3+1]=gsmy;
       ptr->BGSMc[cnt*3+2]=gsmz;

       cnt++;
    }
  }
  RCDFFree(data,2);


  ptr->cnt=cnt;
  return 0;
}


int geolep_plasma(CDFid id,struct plasmadata *ptr,double stime,double etime) {
  long yr,mo,dy,hr,mt,sc,ms;
  double rtime;
  int i,cnt=0;
  char *varlist[]={"Epoch","V0","N0",NULL};
  struct RCDFData *data=NULL;

  int max;
  int rmax;
  int status;

  double *dptr;

  int16 *sptr;
  float Mp=1.6e-27;
  float Vx;
  int pcnt=0;


  strcpy(ptr->sat,"ge");
  strcpy(ptr->ins,"lep");

  cnt=ptr->cnt;
  rmax=RCDFMaxRecR(id,varlist[0]);
  max=cnt+rmax;  
   
  if (rmax==0) return -1;
  data=RCDFMake(3);

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

       sptr=(int16 *) data[1].data;
       
       if (sptr[0]<-32000) { 
           ptr->VGSE[cnt*3]=1e31;
           ptr->VGSE[cnt*3+1]=1e31;
           ptr->VGSE[cnt*3+2]=1e31;
           ptr->VGSM[cnt*3]=1e31;
           ptr->VGSM[cnt*3+1]=1e31;
           ptr->VGSM[cnt*3+2]=1e31;
           ptr->vth[cnt]=1e31;
           ptr->den[cnt]=1e31;
           ptr->pre[cnt]=1e31;
           cnt++;
           continue;
       }
      
       ptr->VGSM[cnt*3]=fabs(sptr[0]*cos(PI*sptr[1]/1800.0)*
                             cos(PI*sptr[2]/1800.0));
       ptr->VGSM[cnt*3+1]=sptr[0]*cos(PI*sptr[1]/1800.0)*
                                  sin(PI*sptr[2]/1800.0);
       ptr->VGSM[cnt*3+2]=sptr[0]*sin(PI*sptr[1]/1800.0);
    
       ptr->vth[cnt]=0;

       sptr=(int16 *) data[2].data;
     
       ptr->den[cnt]=sptr[0]/100.0;
       
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
  RCDFFree(data,3);

  ptr->cnt=cnt;
  return 0;
}
