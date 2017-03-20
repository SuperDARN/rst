/* fitidl.h
   ======== 
   Author R.J.Barnes
*/

/*
   See license.txt
*/

#ifndef _FITIDL_H
#define _FITIDL_H


struct FitIDLData {
  struct {
    IDL_LONG major;
    IDL_LONG minor;
  } revision;
  struct {
    float sky;
    float lag0;
    float vel;
  } noise;
  float pwr0[MAX_RANGE];
  short nlag[MAX_RANGE]; 
  UCHAR qflg[MAX_RANGE]; 
  UCHAR gflg[MAX_RANGE]; 
  float p_l[MAX_RANGE]; 
  float p_l_e[MAX_RANGE];
  float p_s[MAX_RANGE];
  float p_s_e[MAX_RANGE];
  float v[MAX_RANGE];
  float v_e[MAX_RANGE];
  float w_l[MAX_RANGE];
  float w_l_e[MAX_RANGE];
  float w_s[MAX_RANGE];
  float w_s_e[MAX_RANGE];
  float sd_l[MAX_RANGE];
  float sd_s[MAX_RANGE];
  float sd_phi[MAX_RANGE];
  UCHAR x_qflg[MAX_RANGE];
  UCHAR x_gflg[MAX_RANGE];
  float x_p_l[MAX_RANGE];
  float x_p_l_e[MAX_RANGE];
  float x_p_s[MAX_RANGE];
  float x_p_s_e[MAX_RANGE];
  float x_v[MAX_RANGE];
  float x_v_e[MAX_RANGE];
  float x_w_l[MAX_RANGE];
  float x_w_l_e[MAX_RANGE];
  float x_w_s[MAX_RANGE];
  float x_w_s_e[MAX_RANGE];
  float phi0[MAX_RANGE];
  float phi0_e[MAX_RANGE];
  float elv[MAX_RANGE];
  float elv_low[MAX_RANGE];
  float elv_high[MAX_RANGE];
  float x_sd_l[MAX_RANGE];
  float x_sd_s[MAX_RANGE];
  float x_sd_phi[MAX_RANGE];   
};

struct FitIDLInx {
  double time;
  IDL_LONG offset;
};

void IDLCopyFitDataToIDL(int nrang,int xcf,struct FitData *fit,
			 struct FitIDLData *ifit);

void IDLCopyFitDataFromIDL(int nrang,int xcf,struct FitIDLData *ifit,
                           struct FitData *fit);

struct FitIDLData *IDLMakeFitData(IDL_VPTR *vptr);

struct FitIDLInx *IDLMakeFitInx(int num,IDL_VPTR *vptr);


#endif
