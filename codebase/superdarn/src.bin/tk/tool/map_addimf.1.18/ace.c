/* ace.c
   =====
   Author: R.J.Barnes
*/

/*
   See license.txt
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "rtypes.h"
#include "rtime.h"
#include "geopack.h"
#include "cdf.h"
#include "rcdf.h"
#include "istp.h"

int acemfi_imf(CDFid id,struct imfdata *ptr,double stime,double etime,
               int cnv) {
  long  yr,mo,dy,hr,mt,sc,ms;
  double rtime;
  int i,cnt=0;
  char *varlist[]={"Epoch","BGSEc","BGSM",NULL};
  struct RCDFData *data=NULL;

  int max;
  int rmax;
  int status;

  double *dptr;
  float *fptr;
  
  double gsex,gsey,gsez;
  double gsmx,gsmy,gsmz;
  

  if (cnv==1) varlist[2]=NULL;

  strcpy(ptr->sat,"ac");
  strcpy(ptr->ins,"mfi");

  cnt=ptr->cnt;
  rmax=RCDFMaxRecR(id,varlist[0]);
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
    status=RCDFReadR(id,i,varlist,data);
    if (status==0) {
       dptr=(double *) data[0].data;
       EPOCHbreakdown(dptr[0],&yr,&mo,&dy,&hr,&mt,&sc,&ms); 
       rtime=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc+(ms/1000.0));
       if (rtime<stime) continue;
       if (rtime>etime) break;
       ptr->time[cnt]=rtime;
       fptr=(float *) data[1].data;
          
       ptr->BGSEc[cnt*3]=fptr[0];
       ptr->BGSEc[cnt*3+1]=fptr[1];
       ptr->BGSEc[cnt*3+2]=fptr[2];

       if (cnv==1) {
         gsex=fptr[0];
         gsey=fptr[1];
         gsez=fptr[2];

         GeoPackRecalc(yr,mo,dy,hr,mt,(sc+1.0*ms/1000.0));
         GeoPackGseGsm(gsex,gsey,gsez,&gsmx,&gsmy,&gsmz);

         ptr->BGSMc[cnt*3]=gsmx;
         ptr->BGSMc[cnt*3+1]=gsmy;
         ptr->BGSMc[cnt*3+2]=gsmz;
       } else {
         fptr=(float *) data[2].data;
         ptr->BGSMc[cnt*3]=fptr[0];
         ptr->BGSMc[cnt*3+1]=fptr[1];
         ptr->BGSMc[cnt*3+2]=fptr[2];
       }
       cnt++;
    }
  }

  RCDFFree(data,3); 
  ptr->cnt=cnt;
  return 0;
}





