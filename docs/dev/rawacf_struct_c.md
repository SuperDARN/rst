
<!--
(C) copyright 2020 VT SuperDARN, Virginia Polytechnic Institute & State University
author: Kevin Sterne
-->

# Structure in C for the Raw ACF data

## Summary

A description of the data structure in the C language used to store the Raw ACF Data.

## Description

The raw ACF data is the primary (first order) data product from the SuperDARN radars.  Listed below is the C data structure <code>struct RawData</code> used to store the ACF data.

### Array Limits

The array limits are defined as macros in the header <code>superdarn/limit.h</code>.

```
#define LAG_SIZE 96	       
#define MAX_RANGE 300	
```

### `struct RawData`

The ACF data structure is defined in the header <code>superdarn/rawdata.h</code>.

```
struct RawData {
  struct {
    int major;                        /* major revision */
    int minor;                        /* minor revision */
  } revision;            
  float thr;                          /* lag-zero power threshold level */
  float pwr0[MAX_RANGE];              /* lag-zero powers
  float acfd[MAX_RANGE][LAG_SIZE][2]; /* ACF data */
  float xcfd[MAX_RANGE][LAG_SIZE][2]; /* XCF data */
};
```

## References

- This information is sourced from the RFC: 0013 previously in the RST RFC docmentation that was written by R.J. Barnes.

## History

- 2007/03/26  Revision to reflect change in array sizes
- 2004/11/19  Initial Revision.

