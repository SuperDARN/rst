/* cnvmapidl.h
   ======== 
   Author R.J.Barnes
*/

/*
   See license.txt
*/

#ifndef _CNVMAPIDL_H
#define _CNVMAPIDL_H

struct CnvMapIDLPrm {
  struct {
    int16 yr;
    int16 mo;
    int16 dy;
    int16 hr;
    int16 mt;
    double sc;
  } start;
  struct {
    int16 yr;
    int16 mo;
    int16 dy;
    int16 hr;
    int16 mt;
    double sc;
  } end;
  int32 stnum;
  int32 vcnum;
  int16 xtd;
  int16 major_rev;
  int16 minor_rev;
  
  IDL_STRING source;

  int32 modnum;
  int16 doping_level;
  int16 model_wt;
  int16 error_wt;
  int16 imf_flag;
  int16 imf_delay;
  double Bx;
  double By;
  double Bz;
  
  IDL_STRING model[2];

  int16 hemisphere;
  int16 fit_order;
  float latmin;
  int16 coefnum;
  double chi_sqr;
  double chi_sqr_dat;
  double rms_err;
  float lon_shft;
  float lat_shft;
  struct {
    double st;
    double ed;
    double av;
  } mlt;
  double pot_drop;
  double pot_drop_err;
  double pot_max;
  double pot_max_err;
  double pot_min;
  double pot_min_err;
  int32 bndnum;
};


struct CnvMapIDLBnd {
  float lat;
  float lon;
};

struct CnvMapIDLInx {
  double time;
  IDL_LONG offset;
};

void IDLCopyCnvMapPrmFromIDL(struct CnvMapIDLPrm *iprm,
			     struct CnvMapData *map,struct GridData *grd);

void IDLCopyCnvMapGVecFromIDL(struct GridIDLGVec *igvec,
			      int nvec,int size,struct CnvMapData *map);

void IDLCopyCnvMapBndFromIDL(struct CnvMapIDLBnd *ibnd,
                             int nvec,int size,struct CnvMapData *map);


void IDLCopyCnvMapPrmToIDL(struct CnvMapData *map,struct GridData *grd,
                         struct CnvMapIDLPrm *iprm);

void IDLCopyCnvMapGVecToIDL(struct CnvMapData *map,int nvec,int size,
			    struct GridIDLGVec *igvec);

void IDLCopyCnvMapBndToIDL(struct CnvMapData *map,int nvec,int size,
			   struct CnvMapIDLBnd *ibnd);

struct CnvMapIDLPrm *IDLMakeCnvMapPrm(IDL_VPTR *vptr);
struct CnvMapIDLBnd *IDLMakeCnvMapBnd(int num,IDL_VPTR *vptr);
struct CnvMapIDLInx *IDLMakeCnvMapInx(int num,IDL_VPTR *vptr);


#endif
