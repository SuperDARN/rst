<!--
(C) copyright 2020 VT SuperDARN, Virginia Polytechnic Institute & State University
author: Kevin Sterne
-->

# Structure in C for the Fitted ACF data

## Summary

A description of the data structure in the C language used to store the Fitted ACF Data.

## Description

The FitACF algorithm is used to derive doppler velocities, spectral widths and powers from the raw ACF data.  Listed below is the C data structure <code>struct FitData</code> used to store the fitted ACF data.

### Array Limits

The array limits are defined as macros in the header <code>superdarn/limit.h</code>.
```
#define MAX_RANGE 300	
```

### `struct FitData`

The fitted ACF data structure is defined in the header <code>superdarn/fitdata.h</code>.
```
struct FitData {
  struct {
    int major;                     /* major revision */
    int minor;                     /* minor revision */
  } revision;             
  struct FitNoise noise;           /* noise parameters */
  struct FitRange rng[MAX_RANGE];  /* ACF range data */
  struct FitRange xrng[MAX_RANGE]; /* XCF range data */
  struct FitElv  elv[MAX_RANGE];   /* elevation angle data */
};
```

### `struct FitNoise`
```
struct FitNoise { 
  double vel;                      /* velocity from fitting the noise ACF */
  double skynoise;                 /* sky noise */
  double lag0;                     /* lag-zero power of noise ACF */
};
```

### `struct FitRange`
```
struct FitRange { 
  double v;                        /* velocity from ACF */
  double v_err;                    /* velocity error from ACF */
  double p_0;                      /* lag zero power */
  double p_l;                      /* power from lambda fit of ACF */
  double p_l_err;                  /* power error from lambda fit of ACF */
  double p_s;                      /* power from sigma fit of ACF */
  double p_s_err;                  /* power error from sigma fit of ACF */
  double w_l;                      /* spectral width from lambda fit of ACF */
  double w_l_err;                  /* spectral width error from lambda fit of ACF */
  double w_s;                      /* spectral width from sigma fit of ACF */
  double w_s_err;                  /* spectral width error from sigma fit of ACF*/ 
  double phi0;                     /* phase determination at lag zero of the ACF */
  double phi0_err;                 /* phase determination error at lag zero of the ACF */
  double sdev_l;                   /* standard deviation of lambda fit */
  double sdev_s;                   /* standard deviation of sigma fit */
  double sdev_phi;                 /* standard deviation of phase fit */
  int qflg;                        /* quality flag */
  int gsct;                        /* ground scatter flag */
  char nump;                       /* number of points in the fit */
};
```

### `struct FitElv`
```
struct FitElv { 
  double normal;                   /* angle of arrival estimate */
  double low;                      /* lowest estimate of angle of arrival */
  double high;                     /* highest estimate of angle of arrival */
};
```

## References

- This information is sourced from the RFC: 0014 previously in the RST RFC documentation that was written by R.J. Barnes.

## History

- 2007/03/26  Revision to reflect changes in array size. 
- 2004/11/19  Initial Revision.

