<!--
(C) Copyright 2020 VT SuperDARN, Virginia Polytechnic Institute & State University
author: Kevin Sterne
-->

# Structure in C for the raw I and Q data

## Summary

A description of the data structure in the C language used to store the raw I and Q  Data.

## Description

The raw I and Q data are the samples observed by the analogue to digital converter or the digital receiver. They can be used to completely characterize and reconstruct the observations at the radar  Listed below is the C data structure `struct FitData` used to store the raw  I and Q data.

### Array Limits

The array limits are defined as macros in the header `superdarn/limit.h`.

`#define MAXNAVE 300`

### `struct IQData`

The raw I and Q data structure is defined in the header `superdarn/iqdata.h`.

```
struct IQData {
  struct {
    int major;                     /* major revision */
    int minor;                     /* minor revision */
  } revision;           
  int chnnum;                      /* number of channels sampled */
  int smpnum;                      /* number of samples per sequence */
  int skpnum;                      /* number of samples to skip at start of sequence */
  int seqnum;                      /* number of pulse sequences taken */
  struct timespec tval[MAXNAVE];   /* time of each pulse sequence */
  int atten[MAXNAVE];              /* attenuation setting for each sequence */
  float noise[MAXNAVE];            /* noise level for each sequence */
  int offset[MAXNAVE];             /* offset into sample buffer */
  int size[MAXNAVE];               /* size of buffer for each sequence */
};
```

### Sample buffer

The raw I and Q data are stored in a one-dimensional array (vector) of type `int16`. The I and Q samples are interleaved, so the first *smpnum*\*2 samples represent the samples taken for the first pulse sequence. If more than one channel was taken, then the next *smpnum\**2 samples represent he next channel.

## References

- This information is sourced from the RFC: 0028 previously in the RST RFC documentation that was written by R.J. Barnes

## History from RFC Documenation

- 2008/03/25  Initial Revision


