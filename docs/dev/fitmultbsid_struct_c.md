<!--
This is a U.S. government work and not under copyright protection in the U.S.
author: Angeline G Burrell
-->

# Structure in C for the Fitted ACF data with additional flags

## Summary

A description of the data structure in the C language used to store the Fitted
ACF Data with additional flags that denote the origin Field of View (FoV),
backscatter region, and nearest-neighbour groups.

## Description

The FitACF algorithm is used to derive doppler velocities, spectral widths and
powers from the raw ACF data.  Improved information about the origin and scatter
type may be obtained by running additional algorithms.  This data structure is
intended to work with any additional level of processing that improves the
fitted backscatter processing.  Listed below is the C data structures used to
store the fitted ACF and additional necessary data
<code>struct FitMultBSID</code>, <code>struct FitBSIDScan</code>,
<code>struct FitBSIDBeam</code>, <code>struct CellBSIDLoc</code>, and
<code>struct CellBSIDFlgs</code>.  All these structures are defined in the
header <code>superdarn/fitmultbsid.h</code>. Additional structures contained
within <code>struct FitBSIDBeam</code> are found in
<code>superdarn/fitblk.h</code> and <code>superdarn/scandata.h</code>.

### `struct FitMultBSID`

This structure organizes the data in a linked list heirarchically by scan, beam,
and range gate.

```
struct FitMultBSID
{
  int stid;                    /* Radar station ID */
  struct
  {
    int major;                  /* major version */
    int minor;                  /* minor version */
  } version;

  double st_time;               /* Starting time of data */
  double ed_time;               /* Ending time of data */
  int num_scans;                /* Total number of scans */

  struct FitBSIDScan *scan_ptr; /* Pointer to the first scan */
  struct FitBSIDScan *last_ptr; /* Pointer to the last scan */
};
```

### `struct FitBSIDScan`

This structure organizes the data for a single scan that contains data for each
beam indexed in chronological order and then numerically by range gate.  It may
also be linked to the previous and subsequent scans.

```
struct FitBSIDScan
{
  double st_time;                /* Starting time of scan data */
  double ed_time;                /* Ending time of scan data */
  int num_bms;                   /* Total number of beams in this scan */
  struct FitBSIDBeam *bm;        /* Pointer of `num_bms` beam data */
  struct FitBSIDScan *next_scan; /* Pointer to the next scan */
  struct FitBSIDScan *prev_scan; /* Pointer to the previous scan */
};
```

### `struct FitBSIDBeam`

This structure organizes the data for a single beam, with range gate data
stored in the zero-indexed pointer that corresponds to each data's range gate.

```
struct FitBSIDBeam
{
  /* Set the beam constants */
  int cpid;    /* Radar program ID */
  int bm;      /* Beam number */
  float bmazm; /* Beam azimuth in degrees */
  double time; /* Epoch time (does not include seconds or microseconds */
  struct {
    int sc;    /* Seconds */
    int us;    /* Microseconds */
  } intt;

  /* Set the beam parameter values */
  int nave;    /* Number of pulse sequences transmitted */
  int frang;   /* Lag to the first range gate in km */
  int rsep;    /* Range gate seperation in km */
  int rxrise;  /* Receiver rise time in microseconds */
  int freq;    /* Transmission frequency in kHz */
  int noise;   /* Noise level */
  int atten;   /* Beam attenuation level */
  int channel; /* Channel number */

  /* Set the beam range-gate information */
  int nrang;                     /* Number of range gates at this beam */
  unsigned char *sct;            /* Flag denoting presence or absence of scatter */
  struct RadarCell *rng;         /* Fitted ACF data for each range gate */
  struct RadarCell *med_rng;     /* Median smoothed data for each range gate */
  struct FitElv *front_elv;      /* Elevation angles, assuming front FoV */
  struct FitElv *back_elv;       /* Elevation angles, assuming rear FoV */
  struct CellBSIDFlgs *rng_flgs; /* Additional flag data for each range gate */
  struct CellBSIDLoc *front_loc; /* Location data, assuming front FoV */
  struct CellBSIDLoc *back_loc;  /* Location data, assuming rear FoV */
};
```

### `struct CellBSIDLoc`

This structure contains location data, assuming an origin FoV.

```
struct CellBSIDLoc
{
  float vh;       /* Virtual height in km */
  float vh_e;     /* Virtual height error in km */
  char vh_m[2];   /* Virtual height calculation method: E (elv), S (standard), or C (Chisham) */
  char region[2]; /* Ionospheric region: U (unset), D, E, or F */
  float hop;      /* Number of hops in the propagation path */
  float dist;     /* Slant distance to the 1/2 hop location in km */
};
```

### `struct CellBSIDFlgs`

This structure contains flag data, assuming an origin FoV.

```
struct CellBSIDFlgs
{
  int fov;       /* FoV flag values: 1 front Fov, 0 unknown FoV, -1 rear FoV */
  int fov_past;  /* Past FoV flag values */
  int grpflg;    /* Group flag: 0 if not a member, 1 if a member */
  int grpnum;    /* Group number */
  char grpid[2]; /* Group ID string (U if unset) */
};
```

## References

- The algorithms for which these structures were designed were published in:

1. Burrell, A. G., Milan, S. E., Perry, G. W., Yeoman, T. K., and Lester, M.
   (2015), Automatically determining the origin direction and propagation mode
   of high-frequency radar backscatter, Radio Sci., 50, 1225– 1245,
   [doi:10.1002/2015RS005808](https://agupubs.onlinelibrary.wiley.com/doi/full/10.1002/2015RS005808).
2. Burrell, A. G., Perry, G. W., Yeoman, T. K., Milan, S. E., & Stoneback, R.
   (2018). Solar influences on the return direction of high-frequency radar
   backscatter. Radio Science, 53, 577– 597.
   [doi:10.1002/2017RS006512](https://agupubs.onlinelibrary.wiley.com/doi/10.1002/2017RS006512)
3. Ribeiro, A. J., Ruohoniemi, J. M., Baker, J., Clausen, L. B. N., de Larquier,
   S., & Greenwald, R. A. (2011). A new approach for identifying ionospheric
   backscatter in midlatitude SuperDARN HF radar observations. Radio Science,
   46, RS4011. [doi:10.1029/2011RS004676](https://doi.org/10.1029/2011RS004676)
4. J Ribeiro, Kevin Sterne, Sebastien de Larquier, Ashton Reimer, Matt Wessel,
   Muhammad Rafiq (Maimaitirebike Maimaiti), Jef Spaleta, Angeline Burrell,
   Bharat Kunduri, Xueling Shi, Christer van der Meeren, Pål Ellingsen,
   Ray Greenwald, Nathaniel Frissell, Anurag Sharma, & Phil Erickson. (2020).
   vtsuperdarn/davitpy: Final release of davitpy (v0.9). Zenodo.
   [doi:10.5281/zenodo.3824466](https://doi.org/10.5281/zenodo.3824466)

## History

- 2021/11/23  Initial Revision.

