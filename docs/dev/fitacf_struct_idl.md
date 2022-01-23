<!--
(C) copyright VT SuperDARN, Virginia Polytechnic Institute & State University
author: Kevin Sterne
-->

# IDL Data Structure for Fitted ACF data

## Summary

A description of the IDL data structure used to store Fitted ACF data.

## Description

The FitACF algorithm is used to derive doppler velocities, spectral widths and powers from the raw ACF data. Within IDL a structure is used to store the fitted ACF data.

### Constants

A number of constants are used to define array limits
```
MAX_RANGE=300
```

### Raw ACF data structure `RawData`

```
fit={FitData, $
      revision: {rlstr, major: 0L, $  ; major revision
                        minor: 0L}, $ ; minor revision
      noise: {nfstr, sky: 0.0, $      ; sky noise
                    lag0: 0.0, $      ; lag-zero power of noise ACF
                     vel: 0.0}, $     ; velocity from fitting noise ACF
      pwr0: fltarr(MAX_RANGE), $      ; lag-zero power
      nlag: intarr(MAX_RANGE), $      ; number of points in the fit
      qflg: bytarr(MAX_RANGE), $      ; quality flag
      gflg: bytarr(MAX_RANGE), $      ; ground scatter flag
      p_l:  fltarr(MAX_RANGE), $      ; power from lambda fit 
      p_l_e: fltarr(MAX_RANGE), $     ; power error from lambda fit 
      p_s: fltarr(MAX_RANGE), $       ; power from sigma fit 
      p_s_e: fltarr(MAX_RANGE), $     ; power error from sigma fit  
      v: fltarr(MAX_RANGE), $         ; velocity
      v_e: fltarr(MAX_RANGE), $       ; velocity error
      w_l: fltarr(MAX_RANGE), $       ; spectral width from lambda fit
      w_l_e: fltarr(MAX_RANGE), $     ; spectral width error from lambda fit
      w_s: fltarr(MAX_RANGE), $       ; spectral width from sigma fit
      w_s_e: fltarr(MAX_RANGE), $     ; spectral width error from sigma fit 
      sd_l: fltarr(MAX_RANGE), $      ; standard deviation of lambda fit
      sd_s: fltarr(MAX_RANGE), $      ; standard deviation of sigma fit
      sd_phi: fltarr(MAX_RANGE), $    ; standard deviation of phase fit 
      x_qflg: bytarr(MAX_RANGE), $    ; XCF quality flag
      x_gflg: bytarr(MAX_RANGE), $    ; XCF ground scatter flag
      x_p_l: fltarr(MAX_RANGE), $     ; XCF power from lambda fit
      x_p_l_e: fltarr(MAX_RANGE), $   ; XCF power error from lambda fit 
      x_p_s: fltarr(MAX_RANGE), $     ; XCF power from sigma fit
      x_p_s_e: fltarr(MAX_RANGE), $   ; XCF power error from sigma fit 
      x_v: fltarr(MAX_RANGE), $       ; XCF velocity
      x_v_e: fltarr(MAX_RANGE), $     ; XCF velocity error
      x_w_l: fltarr(MAX_RANGE), $     ; XCF spectral width from lambda fit
      x_w_l_e: fltarr(MAX_RANGE), $   ; XCF spectral width error from lambda fit
      x_w_s: fltarr(MAX_RANGE), $     ; XCF spectral width from sigma fit
      x_w_s_e: fltarr(MAX_RANGE), $   ; XCF spectral width error from sigma fit
      phi0: fltarr(MAX_RANGE), $      ; phase determination
      phi0_e: fltarr(MAX_RANGE), $    ; phase determination error  
      elv: fltarr(MAX_RANGE), $       ; angle of arrival estimate recommended for scientific use
      elv_fitted(MAX_RANGE), $        ; Alternative elevation angle determined from fitted phase (FitACF v3 only)
      elv_error(MAX_RANGE), $         ; Least-square elevation angle error (FitACF v3 only)
      elv_low: fltarr(MAX_RANGE), $   ; lowest estimate of angle of arrival (FitACF v1-2 only)
      elv_high: fltarr(MAX_RANGE), $  ; highest estimate of angle of arrival (FitACF v1-2 only)
      x_sd_l: fltarr(MAX_RANGE), $    ; XCF standard deviation of lambda fit
      x_sd_s: fltarr(MAX_RANGE), $    ; XCF standard deviation of sigma fit
      x_sd_phi: fltarr(MAX_RANGE) $   ; XCF standard deviation of phase fit 
   }
```

## References
- [FitACF Structure in C](fitacf_struct_c.md)
- This information is sourced from the RFC: 0022 previously in the RST RFC documentation that was written by R.J. Barnes.

## History from RFC Documentation

- 2007/03/26  Change in array sizes.
- 2004/11/22  Initial Revision.
