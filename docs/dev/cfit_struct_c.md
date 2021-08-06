<!--
(C) copyright 2020 VT SuperDARN, Virginia Polytechnic Institute & State University
author: Kevin Sterne
-->

# Structure in C for the cFit compressed fitted ACF data

## Summary

A description of the data structure in the C language used to store the cFit Data.

## Description

The cFit format is a compressed format for storing a limited sub-set of data from the output of the FitACF algorithm. Listed below is the C data structure <code>struct CFitData</code> used to store the cFit data.

### Array Limits

The array limits are defined as macros in the header <code>superdarn/limit.h</code>.
```
#define MAX_RANGE 300	
```

### `struct CFitData`

The fitted ACF data structure is defined in the header <code>superdarn/cfitdata.h</code>.
```
struct CFitdata {
  struct {
    int major;                     /* major revision */
    int minor;                     /* minor revision */
  } version;
  int16 stid;                      /* station identifier */
  double time;                     /* time in seconds from UNIX EPOCH */
  int16 scan;                      /* scan flag */
  int16 cp;                        /* program identifier */
  int16 bmnum;                     /* beam number */
  float bmazm;                     /* beam azimuth */
  int16 channel;                   /* channel */
  struct { 
    int16 sc;                      /* whole number of seconds integration time */
    int16 us;                      /* fraction number of microseconds of integration time */
  } intt;
  int16 frang;                     /* distance to first range (kilometers) */
  int16 rsep;                      /* range separation (kilometers) */
  int16 rxrise;                    /* receiver rise time (microseconds) */
  int16 tfreq;                     /* transmitted frequency */
  int32 noise;                     /* calculated noise from clear frequency search
  int16 atten;                     /* attenuation level */
  int16 nave;                      /* number of integrations */
  int16 nrang;                     /* number of ranges */

  unsigned char num;               /* number of stored ranges */
  unsigned char rng[MAX_RANGE];    /* range look-up table */
  struct CFitCell data[MAX_RANGE]; /* range data */
};
```

### `struct CFitCell`
```
struct CFitCell {
  int gsct;
  double p_0;                      /* lag zero power */
  double p_0_e;                    /* lag zero power error */
  double v;                        /* velocity */
  double v_e;                      /* velocity error */
  double w_l;                      /* lambda spectral width */
  double w_l_e;                    /* lambda spectral width error */
  double p_l;                      /* lambda power */
  double p_l_e;                    /* lambda power error */
};
```

## References

- This information is sourced from the RFC: 0015 previously in the RST RFC documentation that was written by R.J. Barnes

## History

- 2007/03/26  Revision to reflect changes in array sizes and beam azimuth.
- 2004/11/19  Initial Revision.



