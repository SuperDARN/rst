/* cnvmap.h 
   ========
   Author: R.J.Barnes
*/


/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
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

  double Vx;
  double tilt;
  double Kp;

  char imf_model[4][64]; /* the statistical model used */

  int hemisphere;

  int noigrf;  /* set when IGRF model is _not_ used */
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

