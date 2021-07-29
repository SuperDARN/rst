<!--
(C) copyright 2020 VT SuperDARN, Virginia Polytechnic Institute & State University
author: Kevin Sterne
-->

# IDL Data Structure for raw I and Q data</b>

## Summary

A description of the IDL data structure used to store raw I and Q data.

## Description

The raw ACF data is the lowest level (zero order) data product from the SuperDARN radars. Within IDL a structure is used to store the I and Q data.

### Constants

A constant is used to define array limits

`MAXNAVE=300`

### Raw I and Q  data structure `IQData`

```
raw={IQData, $
       revision: {rlstr, major: 0L, $        ; major revision 
                            minor: 0L}, $    ; minor revision
       chnnum: 0L, $                         ; number of channels
       smpnum: 0L, $                         ; number of samples per sequence
       skpnum: 0L, $                         ; number of samples to skip
       seqnum: 0L, $                         ; number of sequences taken
       tval: make_array(MAX_NAVE,VALUE={unxtmstr, sec:0L, nsec:0L}), $
                                             ; time of each sequence
       atten: intarr(MAX_NAVE), $            ; attenuation for each sequence
       noise: fltarr(MAX_NAVE), $            ; noie for each sequence
       offset: lonarr(MAX_NAVE), $           ; offset into sample array
       size: lonarr(MAX_NAVE) $              ; number of words in sample array
    }
```

## References

- [IQDat Structure in C](iqdat_struct_c.md)
- This information is sourced from the RFC: 0029 previously in the RST RFC documentation that was written by R.J. Barnes

## History from RFC Documenation

- 2008/03/25  Initial Revision.

