/* wind.c
   ======
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
#include "cdf.h"
#include "rcdf.h"
#include "istp.h"

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









