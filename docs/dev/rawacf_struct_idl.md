<!--
(C) copyright 2020 VT SuperDARN, Virginia Polytechnic Institute & State University
author: Kevin Sterne
-->

# IDL Data Structure for Raw ACF data

## Summary

A description of the IDL data structure used to store Raw ACF data.

## Description

The raw ACF data is the primary (first order) data product from the SuperDARN radars. Within IDL a structure is used to store the ACF data.

### Constants

A number of constants are used to define array limits
```
LAG_SIZE=96
MAX_RANGE=300
```

### Raw ACF data structure `RawData`
```
raw={RawData, $
       revision: {rlstr, major: 0L, $        ; major revision 
                            minor: 0L}, $    ; minor revision
       thr: 0.0, $                           ; lag-zero power threshold level
       pwr0: fltarr(MAX_RANGE), $            ; lag zero powers
       acfd: fltarr(MAX_RANGE,LAG_SIZE,2), $ ; ACF data
       xcfd: fltarr(MAX_RANGE,LAG_SIZE,2) $  ; XCF data
    }
```

## References

- [RawACF Structure in C](rawacf_struct_c.md)
- This information is sourced from the RFC: 0021 previously in the RST RFC documentation that was written by R.J. Barnes.

## History from RFC Documentation

- 2007/03/26  Change in array sizes.
- 2004/11/22  Initial Revision.
