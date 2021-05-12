<!--
(C) copyright 2020 VT SuperDARN, Virginia Polytechnic Institute & State University
author: Kevin Sterne
-->

# Structure in C for the Gridded data

## Summary

A description of the data structure in the C language used to store the gridded data.

## Description

Gridded SuperDARN data consists of median filtered, geo-magnetically located ionospheric scatter, fitted to an equi-area grid.  Listed below is the C data structure <code>struct GridData</code> used to store the gridded data.

### `struct GridData`

The gridded data structure is defined in the header `superdarn/griddata.h`

```
struct GridData {
  double st_time;              /* record start time */
  double ed_time;              /* record end time */
 
  int stnum;                   /* number of stations */
  int vcnum;                   /* number of vectors  */
  unsigned char xtd;           /* extended data flag */
  struct GridSVec *sdata;      /* station data */
  struct GridGVec *data;       /* vector data */
};
```

### `struct GridSVec`
```
struct GridSVec {
  int st_id;                    /* station identifier */
  int chn;                      /* channel number */
  int npnt;                     /* number of vectors from this station */
  double freq0;                 /* operating frequency */
  char major_revision;          /* algorithm major revision */
  char minor_revision;          /* algorithm minor revision */
  int prog_id;                  /* program identifier */
  char gsct;                    /* ground scatter componenet */

  struct {
    double mean;                /* mean noise */
    double sd;                  /* noise standard deviation */
  } noise;

  struct {
    double min;                 /* velocity minimum threshold */
    double max;                 /* velocity maximum threshold */
  } vel;

  struct {
    double min;                 /* power minimum threshold */
    double max;                 /* power maximum threshold */
  } pwr;

  struct {
    double min;                 /* spectral width minimum threshold */
    double max;                 /* spectral width maximum threshold */
  } wdt;

  struct {
    double min;                 /* velocity error minimum threshold */
    double max;                 /* velocity error maximum threshold */
  } verr;
};
```

### `struct GridGVec`
```
struct GridGVec {
  double mlat;                  /* magnetic latittude */
  double mlon;                  /* magnetic longitude */
  double azm;                   /* magnetic azimuth */
  struct {
   double median;               /* velocity median */
   double sd;                   /* velocity standard deviation */
  } vel;

  struct {
   double median;               /* power median */
   double sd;                   /* power standard deviation */
  } pwr;

  struct {
   double median;               /* spectral width median */ 
   double sd;                   /* spectral width standard deviation */
  } wdt;
  int st_id;                    /* station identifier */
  int chn;                      /* channel number */
  int index;                    /* grid index */
};
```

## References

- This information is sourced from the RFC: 0017 previously in the RST RFC documentation that was written by R.J. Barnes


## History

- 2004/11/19  Initial Revision from RFC


