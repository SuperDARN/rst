<!--
(C) copyright 2020 VT SuperDARN, Virginia Polytechnic Institute & State University
author: Kevin Sterne
-->

# Structure in C for the Convection Map data

## Summary

A description of the data structure in the C language used to store the convection map data.

## Description

SuperDARN Convection map data consists of a superset of the gridded data. Listed below is the C data structure <code>struct CnvMapData</code> used to store the gridded data.

### `struct CnvMapData`

The convection map data structure is defined in the header <code>superdarn/cnvmap.h</code>.
```
struct CnvMapData {
  int major_rev;                /* major revision number */
  int minor_rev;                /* minor revision number */
  char source[256];             /* source of convection map */

  double st_time;               /* record start time (seconds of UNIX Epoch) */
  double ed_time;               /* record end time  (seconds of UNIX Epoch) */

  int num_model;                /* number of model points */
  int doping_level;             /* model doping level */
  int model_wt;                 /* model weighting */
  int error_wt;                 /* error weighting */
  int imf_flag;                 /* IMF availability flag */
  int imf_delay;                /* IMF delay in minutes */
  
  double Bx;                    /* IMF Bx component */
  double By;                    /* IMF By component */
  double Bz;                    /* IMF Bz component */

  double Vx;			/* Solar wind speed */
  double tilt;                  /* Dipole tilt angle */
  double Kp;                    /* Kp value  */
  
  char imf_model[4][64];        /* statistical model */
   
  int hemisphere;               /* hemisphere flag */

  int noigrf;                   /* set when IGRF model is _not_ used */
  int fit_order;                /* order of fit */
  double latmin;                /* lower latitude limit */
   
  int num_coef;                 /* number of Spherical Harmonic Coefficients */

  double *coef;                 /* Spherical Harmonic Coefficients */

  double chi_sqr;               /* true chi-squared error */
  double chi_sqr_dat;           /* chi-squared error from data */
  double rms_err;               /* RMS error */
  
  double lon_shft;              /* longitudinal pole shift */
  double lat_shft;              /* latitudinal pole shift */

  struct {
    double start;               /* MLT at the start of the record */
    double end;                 /* MLT at the end of the record */
    double av;                  /* MLT in the middle of the record */ 
  } mlt;

  double pot_drop;              /* cross-polar cap potential drop */
  double pot_drop_err;          /* cross-polar cap potential drop error */

  double pot_max;               /* potential maximum */
  double pot_max_err;           /* potential maximum error */

  double pot_min;               /* potential minimum */
  double pot_min_err;           /* potential minimum error */ 

  struct GridGVec *model;       /* model vectors */

  int num_bnd;                  /* number of boundary points */
  double *bnd_lat;              /* boundary latitude */
  double *bnd_lon;              /* boundary longitude */

};
```

## References

- This information is sourced from the RFC: 0019 previously in the RST RFC documentation that was written by R.J. Barnes.

## History

- 2004/11/19  Initial Revision from RFC
