<!--
(C) copyright 2020 VT SuperDARN, Virginia Polytechnic Institute & State University
author: Kevin Sterne
-->

# IDL Data Structure for the Radar Parameter Block


## Summary

A description of the IDL data structure used to store the radar parameter block.

## Description

The radar parameter block is used throughout SuperDARN software to store the radar operating parameters. Within IDL a structure is used to store the parameter block.

### Constants

A number of constants are used to define array limits
```
PULSE_SIZE=64
LAG_SIZE=96
MAX_RANGE=300
```

### Radar parameter block structure `RadarParm`

```
  prm={RadarPrm, $
       revision: {rbstr, major: 0B, $  ; major revision
                         minor: 0B}, $ ; minor revision
       origin: { ogstr, code: 0B, $ ; origin code
                 time: ' ', $ ; time of generation
                 command: ' '}, $ ; command line or control program

       cp: 0, $                        ; program identifier
       stid: 0, $                      ; station identifier
       time: {tmstr, yr:0, $           ; year
                    mo:0, $            ; month
                    dy:0, $            ; day
                    hr:0, $            ; hour
                    mt:0, $            ; minute
                    sc:0, $            ; second
                    us:0  $            ; microsecond
            }, $
       txpow: 0, $                     ; transmitted power (kw)
       nave: 0, $                      ; number of pulse sequences transmitted
       atten: 0, $                     ; attenuation level
       lagfr: 0, $                     ; lag to first range (microseconds)
       smsep: 0, $                     ; sample separation (microseconds)
       ercod: 0, $                     ; error code
       stat: {ststr, agc:0, $          ; AGC status word
                    lopwr:0}, $        ; LOPWR status word
       noise: {nsstr, search:0.0, $    ; noise from clear frequency search
                     mean:0.0}, $      ; average noise across frequency band
       channel: 0, $                   ; channel number for stereo radar
       bmnum: 0, $                     ; beam number
       bmazm: 0.0, $                   ; beam azimuth
       scan: 0, $                      ; scan flag
       rxrise: 0, $                    ; receiver rise time
       intt: {itstr, sc:0, $           ; number of seconds integration time
                     us:0}, $          ; number of microseconds integration time
       txpl: 0, $                      ; transmit pulse length (microseconds)
       mpinc: 0, $                     ; multi-pulse increment (microseconds)
       mppul: 0, $                     ; number of pulses in sequence
       mplgs: 0, $                     ; number of lags in sequence
       nrang: 0, $                     ; number of ranges
       frang: 0, $                     ; distance to first range (kilometers)
       rsep: 0, $                      ; range separation (kilometers)
       xcf: 0, $                       ; XCF flag
       tfreq: 0, $                     ; transmitted frequency
       offset: 0, $                    ; offset between channels
       mxpwr: 0L, $                    ; maximum power
       lvmax: 0L, $                    ; maximum noise level
       pulse: intarr(PULSE_SIZE), $    ; pulse table
       lag: intarr(LAG_SIZE,2), $      ; lag table
       combf: '' $                     ; comment buffer
    }
```

## References

- [Radar Parameter Structure in C](radar_parameter_c.md)
- This information is sourced from the RFC: 0020 previously in the RST RFC documentation was written by R.J. Barnes.

## History from RFC Documentation

- 2007/03/26  Revision to include origin and beam azimuth.
- 2004/11/22  Initial Revision.

