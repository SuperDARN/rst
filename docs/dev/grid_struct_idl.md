<!--
(C) copyright 2020 VT SuperDARN, Virginia Polytechnic Institute & State University
author: Kevin Sterne
-->

# IDL Data Structure for Gridded data

## Summary

A description of the IDL data structure used to store gridded data.

## Description

Gridded SuperDARN data consists of median filtered, geo-magnetically located ionospheric scatter, fitted to an equi-area grid. Within IDL three structures are used to store the gridded data.

### Grid parameters data structure `GridPrm`
```
prm={GridPrm, stme: $
         {GridTime,yr:0, $   ; start year
                   mo:0, $   ; start month
                   dy:0, $   ; start day
                   hr:0, $   ; start hour
                   mt:0, $   ; start minute
                   sc:0D}, $ ; start second
         etme: $
         {GridTime,yr:0, $   ; end year
                   mo:0, $   ; end month
                   dy:0, $   ; end day
                   hr:0, $   ; end hour
                   mt:0, $   ; end minute
                   sc:0D}, $ ; end second
         stnum: 0L, $        ; number of contributing stations
         vcnum: 0L, $        ; number of vectors
         xtd:0 $             ; extended format flag
       }
```

### Grid station information data structure `GridSVec`
```
stvec={GridSVec,st_id:0, $    ; station identifier 
            chn:0, $                       ; channel number
            npnt:0, $                      ; number of vectors
            freq:0.0, $                    ; operating frequency
            major_revision:0, $            ; major revision 
            minor_revision:0, $            ; minor revision
            prog_id: 0, $                  ; program identifier
            gsct: 0, $                     ; ground scatter flag
            noise: {GridNoise,mean:0.0, $  ; noise mean
                              sd:0.0}, $   ; noise standard deviation
            vel: {GridLimit,min:0.0, $     ; velocity threshold minimum
                            max:0.0}, $    ; veloctiy threshold maximum
            pwr: {GridLimit,min:0.0, $     ; power threshold minimum
                            max:0.0}, $    ; power threshold maximum 
            wdt: {GridLimit,min:0.0, $     ; spectral width threshold minimum
                            max:0.0}, $    ; spectral width threshold maximum
            verr: {GridLimit,min:0.0, $    ; velocity error threshold minimum
                             max:0.0} $    ; velocity error threshold maximum
       }
```

### Grid vectors data structure <code>GridGVec</code></h4>
```
gvec={GridGVec, mlat:0.0, $      ; magnetic latitude
              mlon:0.0, $                    ; magnetic longitude
              azm: 0.0, $                    ; magnetic azimuth
              vel: {GridValue,median:0.0, $  ; velocity mean
                              sd:0.0}, $     ; velocity standard deviation
              pwr: {GridValue,median:0.0, $  ; power mean
                              sd:0.0}, $     ; power standard deviation
              wdt: {GridValue,median:0.0, $  ; spectral width mean
                              sd:0.0}, $     ; spectral width standard deviation
              st_id: 0, $                    ; station identifier
              chn: 0, $                      ; channel number
              index: 0L $                    ; grid index
        }
```

## References

- [Grid Structure in C](grid_struct_c.md)
- This information is sourced from the RFC: 0023 previously in the RST RFC documentation that was written by R.J. Barnes

## History from RFC Documentation

- 2004/11/22  Initial Revision.


