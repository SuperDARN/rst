/* fitidl.c
   ======== 
   Author R.J.Barnes
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
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
     2022-01-23 Emma Bland (UNIS): Added "elv_error" and "elv_fitted" fields to support FitACF v3
 

*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <zlib.h>

#include "idl_export.h"

#include "rtypes.h"
#include "dmap.h"
#include "rprm.h"
#include "fitdata.h"
#include "rprmidl.h"
#include "fitidl.h"

#define ALGORITHM_SIZE 80

void IDLCopyFitDataFromIDL(int nrang,int xcf,struct FitIDLData *ifit,
                           struct FitData *fit) {

  int n;

  FitSetRng(fit,nrang);
  if (xcf !=0) {
     FitSetXrng(fit,nrang);
     FitSetElv(fit,nrang);
  }

  if (strlen(IDL_STRING_STR(&ifit->algorithm)) !=0)
    FitSetAlgorithm(fit,IDL_STRING_STR(&ifit->algorithm));

  fit->revision.major=ifit->revision.major;
  fit->revision.minor=ifit->revision.minor;
  fit->noise.skynoise=ifit->noise.sky;
  fit->noise.lag0=ifit->noise.lag0;
  fit->noise.vel=ifit->noise.vel;
 
  for (n=0;n<nrang;n++) {
    fit->rng[n].p_0=ifit->pwr0[n];
    fit->rng[n].nump=ifit->nlag[n];
    fit->rng[n].qflg=ifit->qflg[n];
    fit->rng[n].gsct=ifit->gflg[n];
    fit->rng[n].p_l=ifit->p_l[n];
    fit->rng[n].p_l_err=ifit->p_l_e[n];
    fit->rng[n].p_s=ifit->p_s[n];
    fit->rng[n].p_s_err=ifit->p_s_e[n];
    fit->rng[n].v=ifit->v[n];
    fit->rng[n].v_err=ifit->v_e[n];
    fit->rng[n].w_l=ifit->w_l[n];
    fit->rng[n].w_l_err=ifit->w_l_e[n];
    fit->rng[n].w_s=ifit->w_s[n];
    fit->rng[n].w_s_err=ifit->w_s_e[n];
    fit->rng[n].sdev_l=ifit->sd_l[n];
    fit->rng[n].sdev_s=ifit->sd_s[n];
    fit->rng[n].sdev_phi=ifit->sd_phi[n];
    if (xcf !=0) {
      fit->elv[n].low=ifit->elv_low[n];
      fit->elv[n].normal=ifit->elv[n];
      fit->elv[n].high=ifit->elv_high[n];
      fit->elv[n].error=ifit->elv_error[n];
      fit->elv[n].fitted=ifit->elv_fitted[n];
      fit->xrng[n].qflg=ifit->x_qflg[n];
      fit->xrng[n].gsct=ifit->x_gflg[n];
      fit->xrng[n].p_l=ifit->x_p_l[n];
      fit->xrng[n].p_l_err=ifit->x_p_l_e[n];
      fit->xrng[n].p_s=ifit->x_p_s[n];
      fit->xrng[n].p_s_err=ifit->x_p_s_e[n];
      fit->xrng[n].v=ifit->x_v[n];
      fit->xrng[n].v_err=ifit->x_v_e[n];
      fit->xrng[n].w_l=ifit->x_w_l[n];
      fit->xrng[n].w_l_err=ifit->x_w_l_e[n];
      fit->xrng[n].w_s=ifit->x_w_s[n];
      fit->xrng[n].w_s_err=ifit->x_w_s_e[n];
      fit->xrng[n].sdev_l=ifit->x_sd_l[n];
      fit->xrng[n].sdev_s=ifit->x_sd_s[n];
      fit->xrng[n].sdev_phi=ifit->x_sd_phi[n];
      fit->xrng[n].phi0=ifit->phi0[n];
      fit->xrng[n].phi0_err=ifit->phi0_e[n];
    }
  }
}



void IDLCopyFitDataToIDL(int nrang,int xcf,struct FitData *fit,
                           struct FitIDLData *ifit) {

  int n;

  char algorithmtmp[ALGORITHM_SIZE+1];

  memset(&algorithmtmp,0,ALGORITHM_SIZE+1);

  memset(ifit,0,sizeof(struct FitIDLData));

  if (fit->algorithm !=NULL) {
    strncpy(algorithmtmp,fit->algorithm,ALGORITHM_SIZE);
    IDL_StrStore(&ifit->algorithm,algorithmtmp);
  }

  ifit->revision.major=fit->revision.major;
  ifit->revision.minor=fit->revision.minor;
  ifit->noise.sky=fit->noise.skynoise;
  ifit->noise.lag0=fit->noise.lag0;
  ifit->noise.vel=fit->noise.vel;

  for (n=0;n<nrang;n++) {
    ifit->pwr0[n]=fit->rng[n].p_0;
    ifit->nlag[n]=fit->rng[n].nump;
    ifit->qflg[n]=fit->rng[n].qflg;
    ifit->gflg[n]=fit->rng[n].gsct;
    ifit->p_l[n]=fit->rng[n].p_l;
    ifit->p_l_e[n]=fit->rng[n].p_l_err;
    ifit->p_s[n]=fit->rng[n].p_s;
    ifit->p_s_e[n]=fit->rng[n].p_s_err;
    ifit->v[n]=fit->rng[n].v;
    ifit->v_e[n]=fit->rng[n].v_err;
    ifit->w_l[n]=fit->rng[n].w_l;
    ifit->w_l_e[n]=fit->rng[n].w_l_err;
    ifit->w_s[n]=fit->rng[n].w_s;
    ifit->w_s_e[n]=fit->rng[n].w_s_err;
    ifit->sd_l[n]=fit->rng[n].sdev_l;
    ifit->sd_s[n]=fit->rng[n].sdev_s;
    ifit->sd_phi[n]=fit->rng[n].sdev_phi;
    if ((xcf !=0) && (fit->elv !=NULL) && (fit->xrng !=NULL)) {
      ifit->elv_low[n]=fit->elv[n].low;
      ifit->elv[n]=fit->elv[n].normal;
      ifit->elv_high[n]=fit->elv[n].high;
      ifit->elv_fitted[n]=fit->elv[n].fitted;
      ifit->elv_error[n]=fit->elv[n].error;
      ifit->x_qflg[n]=fit->xrng[n].qflg;
      ifit->x_gflg[n]=fit->xrng[n].gsct;
      ifit->x_p_l[n]=fit->xrng[n].p_l;
      ifit->x_p_l_e[n]=fit->xrng[n].p_l_err;
      ifit->x_p_s[n]=fit->xrng[n].p_s;
      ifit->x_p_s_e[n]=fit->xrng[n].p_s_err;
      ifit->x_v[n]=fit->xrng[n].v;
      ifit->x_v_e[n]=fit->xrng[n].v_err;
      ifit->x_w_l[n]=fit->xrng[n].w_l;
      ifit->x_w_l_e[n]=fit->xrng[n].w_l_err;
      ifit->x_w_s[n]=fit->xrng[n].w_s;
      ifit->x_w_s_e[n]=fit->xrng[n].w_s_err;
      ifit->x_sd_l[n]=fit->xrng[n].sdev_l;
      ifit->x_sd_s[n]=fit->xrng[n].sdev_s;
      ifit->x_sd_phi[n]=fit->xrng[n].sdev_phi;
      ifit->phi0[n]=fit->xrng[n].phi0;
      ifit->phi0_e[n]=fit->xrng[n].phi0_err;
    }
  }
}

struct FitIDLData *IDLMakeFitData(IDL_VPTR *vptr) {
  
  void *s=NULL;
  
  static IDL_MEMINT rdim[]={1,MAX_RANGE};
 
  static IDL_STRUCT_TAG_DEF revision[]={
    {"MAJOR",0,(void *) IDL_TYP_LONG},
    {"MINOR",0,(void *) IDL_TYP_LONG},
    {0}};

  static IDL_STRUCT_TAG_DEF noise[]={
    {"SKY",0,(void *) IDL_TYP_FLOAT},
    {"LAG0",0,(void *) IDL_TYP_FLOAT},
    {"VEL",0,(void *) IDL_TYP_FLOAT},
    {0}};

  
  static IDL_STRUCT_TAG_DEF fitdata[]={    
    {"ALGORITHM",0,(void *) IDL_TYP_STRING}, /* 0 */
    {"REVISION",0,NULL},   /* 1 */
    {"NOISE",0,NULL},   /* 2 */ 
    {"PWR0",rdim,(void *) IDL_TYP_FLOAT}, /* 3 */
    {"NLAG",rdim,(void *) IDL_TYP_INT}, /* 4 */
    {"QFLG",rdim,(void *) IDL_TYP_BYTE}, /* 5 */
    {"GFLG",rdim,(void *) IDL_TYP_BYTE}, /* 6 */
    {"P_L",rdim,(void *) IDL_TYP_FLOAT}, /* 7 */
    {"P_L_E",rdim,(void *) IDL_TYP_FLOAT}, /* 8 */
    {"P_S",rdim,(void *) IDL_TYP_FLOAT}, /* 9 */
    {"P_S_E",rdim,(void *) IDL_TYP_FLOAT}, /* 10 */
    {"V",rdim,(void *) IDL_TYP_FLOAT}, /* 11 */
    {"V_E",rdim,(void *) IDL_TYP_FLOAT}, /* 12 */
    {"W_L",rdim,(void *) IDL_TYP_FLOAT}, /* 13 */
    {"W_L_E",rdim,(void *) IDL_TYP_FLOAT}, /* 14 */
    {"W_S",rdim,(void *) IDL_TYP_FLOAT}, /* 15 */
    {"W_S_E",rdim,(void *) IDL_TYP_FLOAT}, /* 16 */
    {"SD_L",rdim,(void *) IDL_TYP_FLOAT}, /* 17 */
    {"SD_S",rdim,(void *) IDL_TYP_FLOAT}, /* 18 */
    {"SD_PHI",rdim,(void *) IDL_TYP_FLOAT}, /* 19 */
    {"X_QFLG",rdim,(void *) IDL_TYP_BYTE}, /* 20 */
    {"X_GFLG",rdim,(void *) IDL_TYP_BYTE}, /* 21 */
    {"X_P_L",rdim,(void *) IDL_TYP_FLOAT}, /* 22 */
    {"X_P_L_E",rdim,(void *) IDL_TYP_FLOAT}, /* 23 */
    {"X_P_S",rdim,(void *) IDL_TYP_FLOAT}, /* 24 */
    {"X_P_S_E",rdim,(void *) IDL_TYP_FLOAT}, /* 25 */
    {"X_V",rdim,(void *) IDL_TYP_FLOAT}, /* 26 */
    {"X_V_E",rdim,(void *) IDL_TYP_FLOAT}, /* 27 */
    {"X_W_L",rdim,(void *) IDL_TYP_FLOAT}, /* 28 */
    {"X_W_L_E",rdim,(void *) IDL_TYP_FLOAT}, /* 29 */
    {"X_W_S",rdim,(void *) IDL_TYP_FLOAT}, /* 30 */
    {"X_W_S_E",rdim,(void *) IDL_TYP_FLOAT}, /* 31 */
    {"PHI0",rdim,(void *) IDL_TYP_FLOAT}, /* 32 */
    {"PHI0_E",rdim,(void *) IDL_TYP_FLOAT}, /* 33 */
    {"ELV",rdim,(void *) IDL_TYP_FLOAT}, /* 34 */  
    {"ELV_LOW",rdim,(void *) IDL_TYP_FLOAT}, /* 35 */
    {"ELV_HIGH",rdim,(void *) IDL_TYP_FLOAT}, /* 36 */
    {"ELV_FITTED",rdim,(void *) IDL_TYP_FLOAT}, /* 37 */  
    {"ELV_ERROR",rdim,(void *) IDL_TYP_FLOAT}, /* 38 */ 
    {"X_SD_L",rdim,(void *) IDL_TYP_FLOAT}, /* 39 */
    {"X_SD_S",rdim,(void *) IDL_TYP_FLOAT}, /* 40 */
    {"X_SD_PHI",rdim,(void *) IDL_TYP_FLOAT}, /* 41 */
 
    {0}};

  static IDL_MEMINT ilDims[IDL_MAX_ARRAY_DIM];
 
    
  fitdata[1].type=IDL_MakeStruct("RLSTR",revision);
  fitdata[2].type=IDL_MakeStruct("NFSTR",noise);

  s=IDL_MakeStruct("FITDATA",fitdata);
           
  ilDims[0]=1;
  
  return (struct FitIDLData *) IDL_MakeTempStruct(s,1,ilDims,vptr,TRUE);
  
}


struct FitIDLInx *IDLMakeFitInx(int num,IDL_VPTR *vptr) {
  
  void *s=NULL;
 
  static IDL_MEMINT idim[1]={0};
 
  static IDL_STRUCT_TAG_DEF fitinx[]={
    {"TIME",0,(void *) IDL_TYP_DOUBLE},
    {"OFFSET",0,(void *) IDL_TYP_LONG},
    {0}};

   s=IDL_MakeStruct("FITINX",fitinx);  
   idim[0]=num;

   return (struct FitIDLInx *) IDL_MakeTempStruct(s,1,idim,vptr,TRUE);
}
  

