/* cnvmap.h 
   ========
   Author: R.J.Barnes
*/


/*
   See license.txt
*/




#ifndef _CNVMAP_H
#define _CNVMAP_H

struct CnvMapData {

  int major_rev,minor_rev;
  char source[256];

  double st_time;
  double ed_time;

  int num_model;
  int doping_level;
  int model_wt;
  int error_wt;
  int imf_flag;
  int imf_delay;
   
  /* imf values */ 

  double Bx;
  double By;
  double Bz;
  
  char imf_model[2][64]; /* the IMF models used */
   
  int hemisphere;

  int fit_order;
  double latmin;
   
  int num_coef;

  double *coef; /* nx4 array of co-efficients */

  double chi_sqr;
  double chi_sqr_dat;
  double rms_err;
  
  double lon_shft;
  double lat_shft;

  struct {
    double start;
    double end;
    double av;
  } mlt;

  double pot_drop;
  double pot_drop_err;

  double pot_max;
  double pot_max_err;

  double pot_min;
  double pot_min_err;
  struct GridGVec *model;

  int num_bnd;
  double *bnd_lat;
  double *bnd_lon;


};

struct CnvMapData *CnvMapMake();
void CnvMapFree(struct CnvMapData *ptr);


#endif









