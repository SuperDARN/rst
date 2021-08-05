<!--
(C) copyright 2020 VT SuperDARN, Virginia Polytechnic Institute & State University
author: Kevin Sterne
-->

# IDL Data Structure for Convection Map data

## Summary

A description of the IDL data structure used to store convection map data.

## Description

SuperDARN Convection map data consists of a superset of the gridded data. Within IDL four structures are used to store the convection map data.

### Convection map parameters data structure `CnvMapPrm`
```
prm={CnvMapPrm, stme: $
         {CnvMapTime,yr:0, $        ; start year
                   mo:0, $          ; start month
                   dy:0, $          ; start day
                   hr:0, $          ; start hour
                   mt:0, $          ; start minute
                   sc:0D}, $        ; start second
         etme: $
         {CnvMapTime,yr:0, $        ; end year
                   mo:0, $          ; end month
                   dy:0, $          ; end day
                   hr:0, $          ; end hour
                   mt:0, $          ; end minute
                   sc:0D}, $        ; end second
         stnum: 0L, $               ; number of contributing stations
         vcnum: 0L, $               ; number of vectors
         xtd:0 $                    ; extended format flag
         major_rev:0, $             ; major revision number
         minor_rev:0, $             ; minor revision number
         source: '', $              ; source of convection map
         modnum: 0L,  $             ; number of model points
         doping_level: 0, $         ; model doping level 
         model_wt: 0, $             ; model weighting 
         error_wt: 0, $             ; error weighting
         imf_flag: 0, $             ; IMF availability flag
         imf_delay: 0, $            ; IMF delay in minutes 
         Bx: 0.0D, $                ; IMF Bx component
         By: 0.0D, $                ; IMF By component 
         Bz: 0.0D, $                ; IMF Bz component 
         imf_model: strarr(2), $    ; statistical model
         hemisphere: 0, $           ; hemisphere flag
         fit_order: 0, $            ; order of fit
         latmin: 0.0, $             ; lower latitude limit
         coefnum: 0L, $             ; number of coefficients 
         chi_sqr: 0.0D, $           ; true chi-squared error 
         chi_sqr_dat: 0.0D, $       ; chi-squared error from data
         rms_err: 0.0D, $           ; RMS error 
         lon_shft: 0.0, $           ; longitudinal pole shift  
         lat_shft: 0.0, $           ; latitudinal pole shift 
         mlt: {CnvMapMLT,st:0.0D, $ ; MLT at start of record
                         ed:0.0D, $ ; MLT at end of record
                        av:0.0D}, $ ; MLT in middle of record
         pot_drop:0.0D, $           ; cross-polar cap potential drop 
         pot_drop_err:0.0D, $       ; cross-polar cap potential drop error
         pot_max:0.0D, $            ; potential maximum
         pot_max_err:0.0D, $        ; potential maximum error
         pot_min:0.0D, $            ; potential minimum
         pot_min_err:0.0D, $        ; potential minimum error 
         bndnum: 0L $               ; model vectors 
       }
```

### Convection map boundary data structure `CnvMapBnd`
```
bvec={CnvMapBND, lat:0.0, $ ; boundary latitude
                 lon:0.0}   ; boundary longitude
```

## References

- [Grid Structure in C](grid_struct_c.md)
- [`cnvmap` Variable Definitions](../references/general/map.md)
- [Grid Structure in IDL](grid_struct_idl.md)
- This information is soruced from the RFC: 0024 previously in the RST RFC documentation that was written by R.J. Barnes

## History

- 2004/11/22  Initial Revision.


